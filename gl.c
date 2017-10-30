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

#include "gl_tools.h"
#include "gl_geometries.h"
#include "gl.h"
#include "par.h"

#define PEN_GL_SUSPX 0.0f
#define PEN_GL_SUSPY 0.0f

static CUBE_STATE_T gl_state;
geometry_data geometry;

void gl_draw_frame (float angle) {

	glClear(GL_COLOR_BUFFER_BIT);
	check();	

	// hw pendulum (background)
	glUniform4f(gl_state.unif_color, 0.96f, 0.686f, 0.1176f, 1.0f);
	glUniform1f(gl_state.unif_rotation, angle);
	check();	

	glDrawElements(GL_TRIANGLES, geometry.element_count * 3, GL_UNSIGNED_SHORT, (void*) 0);
	check();	

	/*
	 * to draw a second pendulum: 
	 * glUniform4f ( gl_state.unif_color, 1.0f, 0.0f, 0.0f, 1.0f );
	 * glUniform1f ( gl_state.unif_rotation, angle + 0.5f );
	 * glDrawElements ( GL_TRIANGLES, geometry.element_count * 3, GL_UNSIGNED_SHORT, (void*) 0 );
	 */

	eglSwapBuffers(gl_state.display, gl_state.surface);
	check();
}

void init_shaders(CUBE_STATE_T *state) {

	double angle_offset = 0.0f; //TODO

	// create shaders

	const GLchar* vshadersource = read_file("gl_pendulum.vshader");
	const GLchar* fshadersource = read_file("gl_pendulum.fshader");

	state->vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(state->vshader, 1, &vshadersource, 0);
	glCompileShader(state->vshader);
	check();
	
	state->fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(state->fshader, 1, &fshadersource, 0);
	glCompileShader(state->fshader);
	check();

	// create and link shader program

	state->program = glCreateProgram();
	glAttachShader(state->program, state->vshader);
	glAttachShader(state->program, state->fshader);
        glLinkProgram(state->program);
	check();

	// get attrib and unif locations

	state->attr_vertex = glGetAttribLocation(state->program, "VertexPosition");
	state->unif_rotation = glGetUniformLocation(state->program, "Rotation");
	state->unif_translation = glGetUniformLocation(state->program, "Translation");
	state->unif_screenratio = glGetUniformLocation(state->program, "ScreenRatio");
	state->unif_color = glGetUniformLocation(state->program, "Color");
	check();

	// set values

	glUseProgram(state->program);
	glUniform1f(state->unif_screenratio, ((GLfloat) state->screen_width)/((GLfloat) state->screen_height));
	glUniform4f(state->unif_translation, PEN_GL_SUSPX, PEN_GL_SUSPY, 0.0f, 0.0f);
	glUniform1f(state->unif_rotation, angle_offset);
	glUniform4f(state->unif_color, 0.0f, 0.0f, 1.0f, 1.0f);
	check();
}

int gl_init () {

	CUBE_STATE_T* state = &gl_state;
	memset(state, 0, sizeof(gl_state));

	// initalize opengles

	bcm_host_init();
	init_ogl_ng(state);

	// initialize shaders
	
	init_shaders(state);

	// prepare viewport

	glViewport(0, 0, state->screen_width, state->screen_height);
	check();

	// initialize geometry

	GeneratePendulumGeometry(&geometry); // TODO is there a reason to move vertices and indices to stack?

	// upload vertex data

	GLuint vbo[2];
	glGenBuffers(2, vbo);
	check();
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, geometry.vertices_size, geometry.vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	check();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry.indices_size, geometry.indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	check();

	// upload shader program

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(state->attr_vertex, 3, GL_FLOAT, GL_FALSE, geometry.stride, (void *) geometry.vertex_offset);
	glEnableVertexAttribArray(state->attr_vertex);
	glUseProgram(state->program);
	check();

//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
//	glEnable(GL_CULL_FACE);
//	glEnable(GL_DEPTH_TEST);
//	glDepthRangef(0.0f,5.0f);

//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	check();

	return 0;
}

int gl_terminate () {

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	check();

	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(gl_state.display, gl_state.surface);
	check();

	eglMakeCurrent(gl_state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(gl_state.display, gl_state.surface);
	eglDestroyContext(gl_state.display, gl_state.context);
	eglTerminate(gl_state.display);
	check();	

	GeometryFree(&geometry);

	return 0;
}

int gl_update_geometry (float rodlen_delta, float x_delta, float y_delta, pendulum_configuration* data) {

	GeometryUpdatePendulum(&geometry, (data->geometry.virtual_rod_length) += rodlen_delta);
	glBufferData(GL_ARRAY_BUFFER, geometry.vertices_size, geometry.vertices, GL_STATIC_DRAW);
	check();

	glUniform4f(gl_state.unif_translation, 
		(data->geometry.suspension_x) += x_delta, (data->geometry.suspension_y) += y_delta, 0.0f, 0.0f);
	check();

	return 0;
}

void gl_toggle_alpha (pendulum_configuration* data) {

	if ( data->geometry.transparency == 1 ) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		check();
		data->geometry.transparency = 0;
	} else {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		check();
		data->geometry.transparency = 1;
	}
}
