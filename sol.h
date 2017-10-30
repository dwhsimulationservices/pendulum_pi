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

#include <gsl/gsl_odeiv2.h>

#include "par.h"

int rhs (double t, const double y[], double dydt[], void *params);
int jac (double t, const double y[], double *dfdy, double dfdt[], void *params);
int rhs_linear (double t, const double y[], double dydt[], void *params);
int jac_linear (double t, const double y[], double *dfdy, double dfdt[], void *params);


//double t_sol_final, t_frame_duration; // dont move to data/params!

int sol_solver_init(pendulum_configuration* conf);
int sol_solver_terminate(pendulum_configuration* conf);
void sol_save_start_time();
void sol_debug_time ();
void sol_solve_next_frame(pendulum_configuration* conf);
void sol_calculate_frame_duration ();
void sol_calculate_time_next_frame ();
void sol_debug_time_integrity();
