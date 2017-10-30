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
#include <time.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_errno.h>

#include "sol.h"
#include "pen.h"
#include "ui.h"

double y[2] = {0.0,0.0};
double t = 0.0;
struct timespec time_before, time_after, time_now, time_start;
double t_sol_final, t_frame_duration; // dont move to data/params!

/* timing functions */

/* returns difference in seconds */
double time_substract (struct timespec* time_later, struct timespec* time_earlier) {
	return (time_later->tv_sec - time_earlier->tv_sec) + (time_later->tv_nsec - time_earlier->tv_nsec)/1.0e9;
}

/* save current time to "time_start" */
void sol_save_start_time () {
	clock_gettime(CLOCK_MONOTONIC, &time_start);
}

/* calculate or retrieve frame rate */
void sol_calculate_frame_duration () {
	t_frame_duration = 1.0/60.0; // TODO

	/*
	int frame_nsec;

	clock_gettime(CLOCK_MONOTONIC, &time_after);
	frame_nsec = ( time_after.tv_nsec - time_before.tv_nsec );
	clock_gettime(CLOCK_MONOTONIC, &time_before);
	if (frame_nsec > 0)
		printf("difference: %f \n\r", frame_nsec/1.0e9 );
	*/
}

/* calculate the target time of the next frame and store to "t_sol_final" */
void sol_calculate_time_next_frame () {
	clock_gettime(CLOCK_MONOTONIC, &time_now);
	t_sol_final = time_substract(&time_now,&time_start) + t_frame_duration;
}

/* a debug function */
void sol_debug_time () {
	clock_gettime(CLOCK_MONOTONIC, &time_after);
	fprintf(stderr,"time passed: %f \n\r",
			time_substract(&time_after,&time_before) );
	time_before = time_after;
}

/* another debug function */
void sol_debug_time_integrity () {
	clock_gettime(CLOCK_MONOTONIC, &time_now);
	if ( time_substract(&time_now,&time_start) > t_sol_final ) {
		const double time_diff = time_substract(&time_now,&time_start) - t_sol_final;
		char errormsg[256];
		snprintf(errormsg, sizeof(errormsg), "Next frame is from the past! time delay: %f s\n\r", time_diff);
		ui_print(errormsg);
		if ( time_diff > 1.0 )
			simflag = 1;
	}
}

/* solving */

// Data Type: gsl_error_handler_t
void sol_gsl_error_handler (const char * reason, const char * file, int line, int gsl_errno) {
	char errormsg[256];
	snprintf(errormsg, sizeof(errormsg), "gsl %d: %s", gsl_errno, reason);
	ui_print(errormsg);
	simflag = 1;
	// maybe need sigkill or sigint?
}

/* this function is called before the simulation */
int sol_solver_init (pendulum_configuration* conf) {

	gsl_set_error_handler((gsl_error_handler_t*) sol_gsl_error_handler);

	conf->temp.driver = gsl_odeiv2_driver_alloc_y_new(&(conf->model.equation), conf->solver.stepper,
		conf->solver.initialstep, conf->solver.abserr, conf->solver.relerr);

	if (simflag)
		return -1;

	if ( gsl_odeiv2_driver_set_hmax(conf->temp.driver, conf->solver.maxstep) != GSL_SUCCESS ) {
		ui_print("failed to set max step!\n\r");
		return -1;
	}

	y[0] = conf->temp.angle;
	y[1] = 0.0;
	t = 0.0;

	return 0;
}

void sol_solve_next_frame (pendulum_configuration* conf) {
	
	int err;

	if ( conf->solver.adaptive )
		err = gsl_odeiv2_driver_apply(conf->temp.driver, &t, t_sol_final, y);
	else
		err = gsl_odeiv2_driver_apply_fixed_step(conf->temp.driver, &t,
			(t_sol_final-t) / conf->solver.substeps, conf->solver.substeps, y);

	if ( err != GSL_SUCCESS ) {
		char errormsg[256];
		snprintf(errormsg, sizeof(errormsg), "gsl step failed %d: %s", err,  gsl_strerror(err) );
		ui_print(errormsg);
		simflag = 1;
		return;
	}

	conf->temp.angle = y[0];
}

int sol_solver_terminate (pendulum_configuration* conf) {

	if ( gsl_odeiv2_driver_reset(conf->temp.driver) != GSL_SUCCESS )
		fprintf(stderr,"solver not reset correctly!\n\r");

	/* whoot?
	if ( gsl_odeiv2_driver_free(conf->temp.driver) != GSL_SUCCESS ) {
		fprintf(stderr,"solver not freed correctly!\n\r");
		return -1;
	}
	*/

	gsl_odeiv2_driver_free(conf->temp.driver);

	return 0;
}


