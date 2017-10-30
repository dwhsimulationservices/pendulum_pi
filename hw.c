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

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "hw.h"

int hw_magnet_check () {

	char string[3];
	int fd;
	fd = open("/sys/class/gpio/gpio17/direction", O_RDONLY);
	if ( -1 == fd ) {
		fprintf(stderr, "Failed to open gpio DIRECTION for testing!\n");
		return(-1);
	}

	if ( -1 == read(fd, string, 3) ) {
		fprintf(stderr, "Failed to read gpio DIRECTION for testing!\n");
		return(-1);
	}

	if ( 0 != strcmp (string, "out") ) {
		fprintf(stderr, "Gpio DIRECTION is not set to OUT!\n");
		return(-1);
	}

	close(fd);
	
	return(0);
}

int gpio_write (int value) {

	static const char s_values_str[] = "01";

	int fd;

	fd = open(GPIO_PATH, O_WRONLY);
	if ( -1 == fd ) {
		fprintf(stderr, "Failed to open gpio VALUE for writing!\n");
		return(-1);
	}

	if ( 1 != write(fd, &s_values_str[0 == value ? 0 : 1], 1) ) {
		fprintf(stderr, "Failed to write value!\n");
		return(-1);
	}

	close(fd);

	return(0);
}

inline int hw_magnet_acquire () {

	return gpio_write(1);
}

inline int hw_magnet_release () {

	return gpio_write(0);
}

