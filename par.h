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

#ifndef PEN_CONF
#define PEN_CONF

#include <gsl/gsl_odeiv2.h>

typedef struct {

	struct {
		float suspension_x;
		float suspension_y;
		float screen_width; //TODO do not store
		int transparency;
		/* internal variables following */
		float virtual_rod_length;
	} geometry; 

	struct {
		double gravity;
		//double density;
	} environment; 

	struct {
		double friction_constant;
		double friction_linear;
		double friction_quadratic;
	} bearing;

	struct {
		double mass;
		double length;
		/* internal variables following */
		double distance;
		double distance_gyration;
		double moment_of_inertia;
		double moment_of_inertia_distributedmass;
	} rod;
	
	struct {
		double mass;
		double radius;
		/* internal variables following */
		double distance;
		double distance_gyration;
		double moment_of_inertia;
		double moment_of_inertia_distributedmass;
	} bob;
	
	struct {
		double initialstep;
		double maxstep;
		double abserr;
		double relerr;
		int substeps;
		int adaptive;
		gsl_odeiv2_step_type* stepper; /* translated from string */
	} solver;
	
	struct {
		int linear;
		int pointmass;
		int gyration;
		double initial_angle;
		/* internal variables from here */
		gsl_odeiv2_system equation;
	} model;

	/* temporary and inernal variables */
	struct {
		double angle;
		
		gsl_odeiv2_driver* driver;
		
		double moment_of_inertia;
		double moment_gravity_substitution;
	} temp;

} pendulum_configuration;

typedef enum {DOUBLE, FLOAT, INT, STRING, BOOL} PAR_TYPE_T;
typedef enum {PAR_RESET, PAR_NOT_RESET} PAR_RESET_T;

int par_load_configuration (const char* configname, pendulum_configuration* data, PAR_RESET_T reset);

#endif
