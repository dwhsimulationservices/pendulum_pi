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

#include <bcm_host.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <assert.h>

typedef struct {
	uint32_t screen_width;
	uint32_t screen_height;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;

	/* application specific following */

	GLuint program;
	GLuint vshader;
	GLuint fshader;

	/* attributes and uniforms */

	//GLuint buf; // TODO?
	GLuint attr_vertex;
	GLuint unif_color;
	GLuint unif_screenratio;
	GLuint unif_rotation;
	GLuint unif_translation;
} CUBE_STATE_T;

#define check() assert(glGetError() == 0)

void init_ogl_ng (CUBE_STATE_T *state);
char* read_file (const char* filename);
