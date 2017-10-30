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

#include <fcntl.h>
#include <stdarg.h>
#include <cdk_test.h>

#include "ui.h"
#include "gl.h"
#include "par.h"

#define UI_ANGLE_DELTA 0.02f
#define UI_ANGLE_DELTA_FINE 0.001f
#define UI_LEN_DELTA 0.01f

CDKSCREEN *cdkscreen = 0;
CDKLABEL* textwidget = 0;
CDKLABEL* textwidget2 = 0;
boolean functionKey;
CDKSWINDOW *commandOutput = 0;

int ui_listen_config (pendulum_configuration* conf, volatile sig_atomic_t* stopflag) {

	//drawCDKLabel(textwidget2, FALSE);
	drawCDKLabel(textwidget, FALSE);

	char* configname = "conf-default";
	switch ( (chtype) getchCDKObject(ObjOf(textwidget), &functionKey) ) {
		case 4:
			*stopflag = 1;
			return 0;
		case '1':
			configname = "conf-default";
			break;
		case '2':
			configname = "conf-earth-damped";
			break;
		case '3':
			configname = "conf-earth-damped-pointmass";
			break;
		case '4':
			configname = "conf-earth-undamped";
			break;
		case '5':
			configname = "conf-earth-undamped-pointmass-linear";
			break;
		case '6':
			configname = "conf-jupiter-damped";
			break;
		case '7':
			configname = "conf-moon-undamped";
			break;
		case '8':
			configname = "conf-water-damped";
			break;
		default:
			return 0;
	}

	par_load_configuration((const char*) configname, conf, PAR_NOT_RESET);
	return 1;
}

void ui_listen_simulation (pendulum_configuration* conf, volatile sig_atomic_t* simflag) {
	int input = getchCDKObject(ObjOf(textwidget), &functionKey);
	if ( input == 343 || input == 32 || input == 27 )
		*simflag = 1;
	else if ( input == 118 )
		gl_toggle_alpha(conf);
}

void ui_listen_setup (pendulum_configuration* conf, volatile sig_atomic_t* simflag, volatile sig_atomic_t* setupflag) {
	int input = getchCDKObject(ObjOf(textwidget), &functionKey);
	switch ( input ) {
		// transparency
		case 118:
			gl_toggle_alpha(conf);
			return;
		// rodlength (+, -)
		case 43:
			// plus
			gl_update_geometry(UI_LEN_DELTA, 0.0f, 0.0f, conf);
			return;
		case 45:
			// minus
			gl_update_geometry(-UI_LEN_DELTA, 0.0f, 0.0f, conf);
			return;
		// suspension position (w a s d)
		case 119:
			// up
			gl_update_geometry(0.0f, 0.0f, UI_LEN_DELTA, conf);
			return;
		case 115:
			// down
			gl_update_geometry(0.0f, 0.0f, -UI_LEN_DELTA, conf);
			return;
		case 100:
			// right
			gl_update_geometry(0.0f, UI_LEN_DELTA, 0.0f, conf);
			return;
		case 97:
			// left
			gl_update_geometry(0.0f, -UI_LEN_DELTA, 0.0f, conf);
			return;
		// starting angle 
		case 260:
			// left
			conf->temp.angle = conf->temp.angle - UI_ANGLE_DELTA;
			return;
		case 261:
			// right
			conf->temp.angle = conf->temp.angle + UI_ANGLE_DELTA;
			return;
		// starting angle (fine tune) TODO

		// end simulation (ESC)
		case 27:
			*simflag = 1;
			return;
		// finish setup (ENTER or SPACE)
		case 343:
			*setupflag = 1;
			return;
		case 32:
			*setupflag = 1;
			return;
		// default
		default:
			//if (input > 0) printf("%i\n",input);
			return;
	}	
}


int ui_init () {

	/* *INDENT-EQLS* */
	
	WINDOW *cursesWin = 0;

	/* Set up CDK. */
	cursesWin = initscr();
	cdkscreen = initCDKScreen(cursesWin);

	/* Start CDK Colors. */
	initCDKColor();

	const char *mesg[12];
	mesg[0] = "<L>A) Choose configuration/parameter presetting:";
	mesg[1] = "<L><#HL(70)>";
	mesg[2] = "<L>Press </B/32>1<!B!32> for </B/32>default";
	mesg[3] = "<L>Press </B/32>2<!B!32> for </B/32>Earth - damped";
	mesg[4] = "<L>Press </B/32>3<!B!32> for </B/32>Earth - damped - pointmass";
	mesg[5] = "<L>Press </B/32>4<!B!32> for </B/32>Earth - undamped";
	mesg[6] = "<L>Press </B/32>5<!B!32> for </B/32>Earth - undamped - pointmass - linear";
	mesg[7] = "<L>Press </B/32>6<!B!32> for </B/32>Jupiter - damped";
	mesg[8] = "<L>Press </B/32>7<!B!32> for </B/32>Moon - undamped";
	mesg[9] = "<L>Press </B/32>8<!B!32> for </B/32>Water - damped";
	mesg[10] = "<L><#HL(70)>";
	mesg[11] = "<L>Press CTRL+D to quit";
	textwidget = newCDKLabel(cdkscreen, RIGHT, TOP, (CDK_CSTRING2) mesg, 12, FALSE, FALSE);

	const char *mesg2[7];
	mesg2[0] = "<L>B) Adjust settings:";
	mesg2[1] = "<L><#HL(70)>";
	mesg2[2] = "<L>Use </B/24>w, a, s, d<!B!24> to change the </B/24>position of the pivot";
	mesg2[3] = "<L>Use </B/24>+, -<!B!24> to change the </B/24>virtual length of the pendulum";
	mesg2[4] = "<L>Use </B/24>left, right<!B!24> to change the </B/24>initial angle";
	mesg2[5] = "<L>Use </B/24>v<!B!24> to </B/24>toggle text display";
	mesg2[6] = "<L><#HL(70)>";
	textwidget2 = newCDKLabel(cdkscreen, RIGHT, CENTER, (CDK_CSTRING2) mesg2, 7, FALSE, FALSE);

	commandOutput = newCDKSwindow(cdkscreen, RIGHT, BOTTOM, 10, 70, "output:", 9, TRUE, FALSE);

	refreshCDKScreen(cdkscreen);

	if ( fcntl(0, F_SETFL, O_NONBLOCK) ) {
		fprintf(stderr, "Could not set input mode to non-blocking!\n\r");
		return -1;
	}

	return 0;
}


void ui_terminate () {
	destroyCDKLabel(textwidget);
	destroyCDKLabel(textwidget2);
	destroyCDKSwindow(commandOutput);
	destroyCDKScreen(cdkscreen);
	endCDK();
}

void ui_clear () {
	cleanCDKSwindow(commandOutput);
}

void ui_print (const char *format, ...) {
	char msg[256];
	va_list arglist;

	va_start(arglist, format);
	vsnprintf(msg, sizeof(msg), format, arglist);
	va_end(arglist);

	jumpToLineCDKSwindow(commandOutput, BOTTOM);
	addCDKSwindow(commandOutput, msg, BOTTOM);
}


