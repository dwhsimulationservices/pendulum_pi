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
#include <stdarg.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <gsl/gsl_odeiv2.h>
#include <math.h>

#include "par.h"
#include "sol.h"

xmlXPathContextPtr xpathcontext;

/* low level xml handling */

static inline int open_xml (const char* configname, xmlDocPtr xmldoc) {

	char filename[256];
	snprintf(filename, sizeof(filename), "configs/%s.xml", configname);

	xmldoc = xmlReadFile(filename, NULL, 0);
	
	if ( xmldoc == NULL ) {
		fprintf(stderr, "falling back to default configuration\n\r");
		xmldoc = xmlReadFile("configs/conf-default.xml", NULL, 0);
	}

	if ( xmldoc == NULL ) {
		fprintf(stderr, "cannot load any configuration\n\r");
		xmlCleanupParser();
		return -1;
	}
	
	xpathcontext = xmlXPathNewContext(xmldoc);
	if ( xpathcontext == NULL ) {
		fprintf(stderr, "unable to create XPath context\n\r");
		xmlFreeDoc(xmldoc);
		xmlCleanupParser();
		return -1;
	}
	
	xmlCleanupParser();
	
	return 0;
}

static inline int close_xml (xmlDocPtr xmldoc) {
	xmlXPathFreeContext(xpathcontext);
	xmlFreeDoc(xmldoc); // no it is not uninitialized
	xmlCleanupParser();
	return 0;
}

static int get_parameter (const char* path, PAR_TYPE_T type, ...) {

	// generate xpath query string

	xmlChar xpathExpr[256];
	if ( type == DOUBLE || type == FLOAT || type == INT )
		xmlStrPrintf(xpathExpr, 255, "number(%s)", xmlCharStrdup(path));
	else
		xmlStrPrintf(xpathExpr, 255, "string(%s)", xmlCharStrdup(path));

	// execute xpath query

	const xmlXPathObjectPtr xpathObj = xmlXPathEval(xpathExpr, xpathcontext);  
	if (xpathObj == NULL) {
		fprintf(stderr, "unable to evaluate XPath expression \"%s\"\n\r", xpathExpr);
		xmlXPathFreeObject(xpathObj);
		return -1;
	}

	// check if numeric value is not nan or if string value is empty

	char str[256];
	if ( type == DOUBLE || type == FLOAT || type == INT ) {
		if ( isnan((double) xpathObj->floatval) ) {
			fprintf(stderr, "XPath \"%s\" is nan!\n\r", path);
			xmlXPathFreeObject(xpathObj);
			return -1;
		}
	} else if ( type == STRING || type == BOOL ) {
		snprintf(str, 255, "%s", xpathObj->stringval);
		if ( strcmp(str, "") == 0 ) {
			fprintf(stderr, "XPath \"%s\" is empty string!\n\r", path);
			xmlXPathFreeObject(xpathObj);
			return -1;
		}
	} else {
		fprintf(stderr, "unknown type!\n\r");
		xmlXPathFreeObject(xpathObj);
		return -1;
	}

	// get pointer and store value at address

	va_list arglist;
	va_start(arglist, type);

	if ( type == DOUBLE ) {
		double* variable = va_arg(arglist, double*);
		*variable = ((double) xpathObj->floatval);

	} else if ( type == FLOAT ) {
		float* variable = va_arg(arglist, float*);
		*variable = ((float) xpathObj->floatval);

	} else if ( type == INT ) {
		int* variable = va_arg(arglist, int*);
		*variable = ((int) xpathObj->floatval);

	} else if ( type == STRING ) {
		char* variable = va_arg(arglist, char*);
		snprintf(variable, 255, "%s", str);

	} else if ( type == BOOL ) {
		int* variable = va_arg(arglist, int*);
		if ( strcmp(str, "true") == 0 )
			*variable = 1;
		else
			*variable = 0;

	} else {
		va_end(arglist);
		xmlXPathFreeObject(xpathObj);
		return -1;
	}

	va_end(arglist);
	xmlXPathFreeObject(xpathObj);
	return 0;
}

/* high level function, extracts all parameters from loaded xml */

static inline int load_configuration (pendulum_configuration * data, PAR_RESET_T reset) {

	int errors = 0;

	// geometry parameters

	// TODO persistent
	errors += get_parameter("/pendulum/geometry/suspension_x", FLOAT, &(data->geometry.suspension_x));
	errors += get_parameter("/pendulum/geometry/suspension_y", FLOAT, &(data->geometry.suspension_y));
	errors += get_parameter("/pendulum/geometry/screen_width", FLOAT, &(data->geometry.screen_width));
	data->geometry.transparency = 1;

	// physical parameters

	errors += get_parameter("/pendulum/environment/gravity", DOUBLE, &(data->environment.gravity));
	errors += get_parameter("/pendulum/bearing/friction_constant", DOUBLE, &(data->bearing.friction_constant));
	errors += get_parameter("/pendulum/bearing/friction_linear", DOUBLE, &(data->bearing.friction_linear));
	errors += get_parameter("/pendulum/bearing/friction_quadratic", DOUBLE, &(data->bearing.friction_quadratic));
	errors += get_parameter("/pendulum/rod/mass", DOUBLE, &(data->rod.mass));
	errors += get_parameter("/pendulum/rod/length", DOUBLE, &(data->rod.length));
	errors += get_parameter("/pendulum/bob/mass", DOUBLE, &(data->bob.mass));
	errors += get_parameter("/pendulum/bob/radius", DOUBLE, &(data->bob.radius));

	data->geometry.virtual_rod_length = (data->rod.length + data->bob.radius) / data->geometry.screen_width * 2.0;

	// solver parameters

	errors += get_parameter("/pendulum/solver/initialstep", DOUBLE, &(data->solver.initialstep) );
	errors += get_parameter("/pendulum/solver/maxstep", DOUBLE, &(data->solver.maxstep));
	errors += get_parameter("/pendulum/solver/abserr", DOUBLE, &(data->solver.abserr));
	errors += get_parameter("/pendulum/solver/relerr", DOUBLE, &(data->solver.relerr));
	errors += get_parameter("/pendulum/solver/substeps", INT, &(data->solver.substeps));
	errors += get_parameter("/pendulum/solver/adaptive", BOOL, &(data->solver.adaptive));
	char stepper[256];
	errors += get_parameter("/pendulum/solver/stepper", STRING, stepper);

	// model parameters

	errors += get_parameter("/pendulum/model/linear", BOOL, &(data->model.linear));
	errors += get_parameter("/pendulum/model/pointmass", BOOL, &(data->model.pointmass));
	errors += get_parameter("/pendulum/model/gyration", BOOL, &(data->model.gyration));
	errors += get_parameter("/pendulum/model/initial_angle", DOUBLE, &(data->model.initial_angle));
	
	// check for parameter input errors

	if ( errors != 0 ) {
		fprintf(stderr,"at least one parameter not loaded corretly!\n\r");
		return -1;
	}

	// determine solver stepper from string

	if ( strcmp(stepper, "rk4") == 0 )
		data->solver.stepper = (gsl_odeiv2_step_type*) gsl_odeiv2_step_rk4;
	else if ( strcmp(stepper, "rkf45") == 0 )
		data->solver.stepper = (gsl_odeiv2_step_type*) gsl_odeiv2_step_rkf45;
	else if ( strcmp(stepper, "rk8pd") == 0 )
		data->solver.stepper = (gsl_odeiv2_step_type*) gsl_odeiv2_step_rk8pd;
	else if ( strcmp(stepper, "adams") == 0 )
		data->solver.stepper = (gsl_odeiv2_step_type*) gsl_odeiv2_step_msadams;
	else {
		fprintf(stderr,"unknown stepper defined in configuration!\n\r");
		return -1;
	}
	// TODO	add further stepper functions



	// calculate distance of center of mass

	data->rod.distance = data->rod.length / 2.0;
	data->bob.distance = data->rod.length + data->bob.radius;

	// calculate moments of inertia

	data->rod.moment_of_inertia = data->rod.mass * data->rod.distance * data->rod.distance;
	data->bob.moment_of_inertia = data->bob.mass * data->bob.distance * data->bob.distance;

	// calclulate moments of inertia with respect to distributed mass

	data->rod.moment_of_inertia_distributedmass = data->rod.mass * data->rod.length * data->rod.length / 3.0;
	data->bob.moment_of_inertia_distributedmass = data->bob.mass * data->bob.radius * data->bob.radius * 2.0 / 5.0 +
		data->bob.mass * data->bob.distance * data->bob.distance;

	// calculate radius of gyration

	data->rod.distance_gyration = data->rod.moment_of_inertia_distributedmass / data->rod.mass / data->rod.distance;
	data->bob.distance_gyration = data->bob.moment_of_inertia_distributedmass / data->bob.mass / data->bob.distance;

	// calculate final moment of inertia

	if ( data->model.pointmass ) {
		data->temp.moment_of_inertia = data->rod.moment_of_inertia +
			data->bob.moment_of_inertia;
	} else {
		data->temp.moment_of_inertia = data->rod.moment_of_inertia_distributedmass +
			data->bob.moment_of_inertia_distributedmass;
	}

	// calculate gravitational moment

	if ( data->model.gyration ) {
		// TODO: this is actually not correct! the radius of gyration only applies to the moment of inertia
		data->temp.moment_gravity_substitution = data->environment.gravity * 
			( data->rod.mass * data->rod.distance_gyration + data->bob.mass * data->bob.distance_gyration );
	} else {
		data->temp.moment_gravity_substitution = data->environment.gravity * 
			( data->rod.mass * data->rod.distance + data->bob.mass * data->bob.distance );
	}

	// switch between linear and nonlinear model
	
	data->model.equation.dimension = 2;
	data->model.equation.params = data;
	if ( data->model.linear ) {
		data->model.equation.function = rhs_linear;
		data->model.equation.jacobian = jac_linear;
	} else {
		data->model.equation.function = rhs;
		data->model.equation.jacobian = jac;
	}

	// default initial condition
	
	data->temp.angle = data->model.initial_angle; // TODO persistent?

	return 0;
}

int par_load_configuration (const char* configname, pendulum_configuration * data, PAR_RESET_T reset) {

	xmlDocPtr xmldoc;

	if ( open_xml(configname, xmldoc) ) return -1;
	if ( load_configuration(data, reset) ) return -1;
	if ( close_xml(xmldoc) ) return -1;
	return 0;
}

/*
static int save_xml (const char* configname) {

	const char * default_configname = "conf-default";

	if ( strcmp(configname, default_configname) == 0 ) {
		fprintf(stderr, "saving to default configuration file not allowed!\n");
		return -1;
	}

	if (xmldoc == NULL) {
		fprintf(stderr, "cannot save configuration!\n");
		return -1;
	}
	
	char filename[256];
	snprintf(filename, sizeof(filename), "configs/%s.xml", configname);	
	
	FILE* filepointer = fopen(filename, "w");
	
	if ( filepointer == NULL ) {
		fprintf(stderr, "cannot create/open configuration file for writing!\n");
		return -1;
	}
	
	if ( xmlDocDump(filepointer, xmldoc) == -1 ) {
		fprintf(stderr, "cannot write configuration to file!\n");
		fclose(filepointer);
		return -1;
	}

	fclose(filepointer);
	return 0;
}
*/

/*
static int set_parameter (const char* path, double value) {

	const xmlXPathObjectPtr xpathObj = xmlXPathEval(xmlCharStrdup(path), xpathcontext);  
	if ( xpathObj == NULL ) {
		fprintf(stderr,"unable to evaluate XPath expression \"%s\"\n\r", path);
		xmlXPathFreeObject(xpathObj);
		return -1;
	}

	const int size = (xpathObj->nodesetval) ? xpathObj->nodesetval->nodeNr : 0;
	
	if ( size != 1 ) {
		fprintf(stderr,"invalid xpath for setting parameter!\n\r");
		xmlXPathFreeObject(xpathObj);
		return -1;
	}
	
	xmlChar buf[256];
	xmlStrPrintf(buf, 255, (const xmlChar*) "%f", value);
	
	xmlNodeSetContent(xpathObj->nodesetval->nodeTab[0], buf);
	xmlXPathFreeObject(xpathObj);
	return 0;
}
*/

/*
int par_store_configuration (const char* configname, pendulum_configuration * data) {

	return 0;
} 
*/
