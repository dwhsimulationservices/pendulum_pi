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

#include <gsl_odeiv2.h>
#include <gsl/gsl_errno.h>
#include <math.h>

#include "par.h"

int equation_standard (double t, const double y[], double dydt[], pendulum_configuration* p) {

	//pendulum_data* p = (pendulum_data*) params;

	dydt[0] = y[1];
	dydt[1] = - 8.9 / 0.2 * sin(y[0]);

	return GSL_SUCCESS;
}

int main (int argc, char *argv[]) {

	pendulum_configuration conf;

	open_xml("conf-default");
	load_configuration(&conf);
	close_xml();	

	gsl_odeiv2_system equation = {equation_standard, NULL, 2, &conf};
	
	return 0;
}
