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

#include <stdlib.h>
#include <stdio.h>

#include "gl_tools.h"

void init_ogl_ng (CUBE_STATE_T *state) {

	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	static const EGLint attribute_list[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_DEPTH_SIZE, 8,
		EGL_STENCIL_SIZE, 4,
		EGL_SAMPLE_BUFFERS, 1,
		EGL_SAMPLES, 4,
		EGL_NONE
	};

	static const EGLint context_attributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLConfig config;

	// get an EGL display connection
	state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(state->display!=EGL_NO_DISPLAY);
	check();

	// initialize the EGL display connection
	result = eglInitialize(state->display, NULL, NULL);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);
	check();

	// create an EGL rendering context
	state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
	assert(state->context!=EGL_NO_CONTEXT);
	check();

	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
	assert( success >= 0 );

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = state->screen_width;
	dst_rect.height = state->screen_height;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = state->screen_width << 16;
	src_rect.height = state->screen_height << 16;        

	dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	dispman_update = vc_dispmanx_update_start( 0 );
	 
	//VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FROM_SOURCE , 255, 0 };
	dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
		0/*layer*/, &dst_rect, 0/*src*/,
		&src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);

	nativewindow.element = dispman_element;
	nativewindow.width = state->screen_width;
	nativewindow.height = state->screen_height;
	vc_dispmanx_update_submit_sync( dispman_update );

	check();

	state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
	assert(state->surface != EGL_NO_SURFACE);
	check();

	// connect the context to the surface
	result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
	assert(EGL_FALSE != result);
	check();

	// Set background color and clear buffers
	glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	check();
}

char* read_file (const char* filename) {

	FILE *filepointer;
	long filesize;
	char *buffer;

	if ( !(filepointer = fopen(filename,"rb")) )
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fseek(filepointer,0L,SEEK_END);
	filesize = ftell(filepointer);
	rewind(filepointer);

	if ( !(buffer = calloc(1,filesize+1)) )
	{
		fclose(filepointer);
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	if( fread(buffer,filesize,1,filepointer) != 1 )
	{
		fclose(filepointer);
		free(buffer);
		perror("fread");
		exit(EXIT_FAILURE);
	}

	fclose(filepointer);
	
	// remove newlines
	/*
	int i;
	for(i = 0; i < filesize; i++) {
		if(buffer[i] == '\n' || buffer[i] == '\r') buffer[i] = ' ';
	}
	*/
	
	// TODO: shader file must not end with "};" !!!!!

	return buffer;
}
