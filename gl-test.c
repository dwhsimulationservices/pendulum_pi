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

#include "gl.h"

/* returns difference in seconds */
inline double time_substract (struct timespec* time_later, struct timespec* time_earlier) {
	return (time_later->tv_sec - time_earlier->tv_sec) + (time_later->tv_nsec - time_earlier->tv_nsec)/1.0e9;
}

int main ( int argc, char *argv[] ) {	

	if ( gl_init() ) {
		fprintf(stderr,"GLES seems not to work.\n");
		return -1;
	}

	struct timespec time_now, time_final;

	clock_gettime(CLOCK_MONOTONIC, &time_final);
	time_final.tv_sec += 10;

	clock_gettime(CLOCK_MONOTONIC, &time_now);
	while ( time_substract(&time_final,&time_now) > 0 ) {
		gl_draw_frame(0.3);
		clock_gettime(CLOCK_MONOTONIC, &time_now);
		fprintf(stderr,"bla.\n");
	}

	gl_terminate();

	return 0;

}
