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

#include <math.h>
#include <gsl/gsl_matrix.h>

#include "sol.h" // why is this needed here?
#include "par.h"

int rhs (double t, const double y[], double dydt[], void *params) {
	pendulum_configuration* data = (pendulum_configuration*) params;

	double M_G = - sin(y[0]) * data->temp.moment_gravity_substitution;
	double M_D = - y[1] * data->bearing.friction_linear
			- copysign(
				y[1] * y[1] * data->bearing.friction_quadratic + 
				data->bearing.friction_constant,
			y[1]);

	dydt[0] = y[1];
	dydt[1] = (M_G + M_D) / data->temp.moment_of_inertia;

	return GSL_SUCCESS; 
};

int jac (double t, const double y[], double *dfdy, double dfdt[], void *params) {
	pendulum_configuration* data = (pendulum_configuration*) params;
	
	gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 2, 2);
	gsl_matrix * m = &dfdy_mat.matrix; 
	gsl_matrix_set(m, 0, 0, 0.0);
	gsl_matrix_set(m, 0, 1, 1.0);
	gsl_matrix_set(m, 1, 0, - cos(y[0]) * data->temp.moment_gravity_substitution / data->temp.moment_of_inertia );
	gsl_matrix_set(m, 1, 1, (- data->bearing.friction_linear + 2.0 * data->bearing.friction_quadratic * y[1]) / data->temp.moment_of_inertia );
	dfdt[0] = 0.0;
	dfdt[1] = 0.0;
	
	return GSL_SUCCESS;
}

int rhs_linear (double t, const double y[], double dydt[], void *params) {
	pendulum_configuration* data = (pendulum_configuration*) params;	

	double M_G = - y[0] * data->temp.moment_gravity_substitution;
	double M_D = - y[1] * data->bearing.friction_linear
			- copysign(
				y[1] * y[1] * data->bearing.friction_quadratic + 
				data->bearing.friction_constant,
			y[1]);

	dydt[0] = y[1];
	dydt[1] = (M_G + M_D) / data->temp.moment_of_inertia;

	return GSL_SUCCESS; 
}

int jac_linear (double t, const double y[], double *dfdy, double dfdt[], void *params) {
	pendulum_configuration* data = (pendulum_configuration*) params;
	
	gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 2, 2);
	gsl_matrix * m = &dfdy_mat.matrix; 
	gsl_matrix_set(m, 0, 0, 0.0);
	gsl_matrix_set(m, 0, 1, 1.0);
	gsl_matrix_set(m, 1, 0, - data->temp.moment_gravity_substitution / data->temp.moment_of_inertia );
	gsl_matrix_set(m, 1, 1, (- data->bearing.friction_linear + 2.0 * data->bearing.friction_quadratic * y[1]) / data->temp.moment_of_inertia );
	dfdt[0] = 0.0;
	dfdt[1] = 0.0;
	
	return GSL_SUCCESS;
}
