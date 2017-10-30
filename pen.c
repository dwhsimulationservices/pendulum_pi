/*
 * pendulum_pi -- Didactic Pendulum Simulation on the Raspberry Pi
 * Copyright (C) 2014-2017  dwh simulation services
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "pen.h"
#include "ui.h"
#include "gl.h"
#include "hw.h"
#include "sol.h"
#include "par.h"

static inline int setup_and_sim (pendulum_configuration* data) {

	ui_print("starting gl, magnet...\r\n");

	// init gl
	if ( gl_init() ) {
		fprintf(stderr,"GLES seems not to work.\n");
		return -1;
	}
	gl_update_geometry(0.0f, 0.0f, 0.0f, data);

	// init magnet
	if ( hw_magnet_check() ) {
		fprintf(stderr,"magnet check failed! magnet wont work!\n");
		gl_terminate();
		return -1;
	}

	// start magnet
	if ( hw_magnet_acquire() ) {
		fprintf(stderr,"magnet could not be turned on.\n");
		gl_terminate();
		return -1;
	}

	simflag = 0;
	setupflag = 0;

	// reset the starting angle to a default value
	data->temp.angle = data->model.initial_angle;

	ui_print("graphical setup...\r\n");

	// allow user to set up initial condition (and adjust alignment)
	while ( !simflag && !setupflag && !stopflag ) {

		// get keyboard input
		ui_listen_setup(data, &simflag, &setupflag);

		// calculate/aestimate frame rate
		sol_calculate_frame_duration();

		// draw the frame
		gl_draw_frame(data->temp.angle);
	}

	if ( simflag ) {
		ui_print("aborted during setup...\r\n");
		hw_magnet_release();
		gl_terminate();
		return 0;
	}

	ui_print("initializing solver and starting simulation...\r\n");

	// initialize solver
	if ( sol_solver_init(data) ) {
		ui_print("solver initialization failed.\r\n");
		hw_magnet_release();
		gl_terminate();
		sol_solver_terminate(data);
		return 0;
	}

	// release pendulum
	if ( hw_magnet_release() ) {
		fprintf(stderr,"magnet not released?\n\r");
		gl_terminate();
		sol_solver_terminate(data);
		return -1;
	}

	// get start time
	sol_save_start_time();

	// simulation loop
	while ( !simflag && !stopflag ) {
		// get keyboard input if available
		ui_listen_simulation(data, &simflag);

		// calculate target time for next frame
		sol_calculate_time_next_frame();

		// solve the equation until the time for the next frame

		sol_solve_next_frame(data);

		sol_debug_time_integrity();

		// draw the frame
		gl_draw_frame(data->temp.angle);

		//sol_debug_time ();
	}

	// shutdown
	gl_terminate();
	sol_solver_terminate(data);

	ui_print("simulation terminated...\r\n");

	return 0;
}

void sigcatch (int sig) {
	fprintf(stderr,"program aborted!\n\r");
	stopflag = 1;
}

void init_signals () {
	if( (int) signal(SIGINT,sigcatch) < 0 ) {
		fprintf(stderr,"could not attach signal handler!\n\r");
	}
	if( (int)signal(SIGQUIT,sigcatch) < 0 ) {
		fprintf(stderr,"could not attach signal handler!\n\r");
	}
	if( (int) signal(SIGTERM,sigcatch) < 0 ) {
		fprintf(stderr,"could not attach signal handler!\n\r");
	}
}

int main (int argc, char *argv[]) {

	printf("initializing program...\r\n");

	stopflag = 0;
	struct timespec tpause, tremaining;
	tpause.tv_sec = 0;
	tpause.tv_nsec = 10000000;

	init_signals();

	pendulum_configuration conf;
	if ( par_load_configuration("conf-default", &conf, PAR_RESET) ) return -1;

	if ( ui_init() ) {
		fprintf(stderr,"ui initialization failed.\n\r");
		return -1;
	}

	while ( !stopflag ) {
		if ( ui_listen_config(&conf, &stopflag) == 1 ) {
			ui_clear();
			if ( setup_and_sim(&conf) != 0 )
				stopflag = 1;
		}
		nanosleep(&tpause,&tremaining);
	}

	ui_terminate();

	printf("shutdown complete...\r\n");

	return 0;
}
