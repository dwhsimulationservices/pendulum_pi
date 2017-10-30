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
#include "hw.h"

int main (int argc, char *argv[]) {

	while (1 == 1) {
		printf("turning on GPIO 17\n");
		if ( -1 == hw_magnet_acquire() ) {
			fprintf(stderr, "error turning on!\n");
			return(1);
		}

		usleep(1000 * 1000);

		printf("turning off GPIO 17\n");
		if ( -1 == hw_magnet_release() ) {
			fprintf(stderr, "error turning off!\n");
			return(1);
		}

		usleep(1000 * 1000);
	}
}
