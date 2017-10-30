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
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "gl_geometries.h"

#define PEN_GL_ROD_WIDTH	0.008f
#define PEN_GL_BOB_RADIUS	0.035f
#define PEN_GL_ROD_LENGTH	1.0f		// will be replaced on config load
#define PI 3.1415926535897932384626433832795f

void GeneratePendulumGeometry (geometry_data* geometry) {

	// geometry parameters

	unsigned int rod_numelements = 2;
	unsigned int rod_numvertices = 4;
	unsigned int bob_numelements = 64;
	unsigned int bob_numvertices = bob_numelements + 1;

	// generate rod geometry

	float rod_vertices[4][6] = {	{0.0f - PEN_GL_ROD_WIDTH ,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
					{0.0f + PEN_GL_ROD_WIDTH ,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
					{0.0f - PEN_GL_ROD_WIDTH , -PEN_GL_ROD_LENGTH, 0.0f, 0.0f, 0.0f, 0.0f},
					{0.0f + PEN_GL_ROD_WIDTH , -PEN_GL_ROD_LENGTH, 0.0f, 0.0f, 0.0f, 0.0f},	};
	unsigned short rod_indices[6] = {0,2,1,   1,2,3};

	// generate bob geometry (combined array)
	
	float pen_vertices[bob_numvertices + rod_numvertices][6];
	unsigned short pen_indices[bob_numelements*3 + 6];
	
	unsigned int i, j;
	for ( i = 0; i < rod_numvertices; i++ )
	{
		for ( j = 0; j < rod_numelements*3; j++ )
			pen_vertices[i][j] = rod_vertices[i][j];
	}
	for ( i = 0; i < rod_numelements*3; i++ )
	{
		pen_indices[i] = rod_indices[i];
	}

	pen_vertices[rod_numvertices][0] = 0.0f; pen_vertices[rod_numvertices][1] = -PEN_GL_ROD_LENGTH;
	pen_vertices[rod_numvertices][2] = 0.0f; pen_vertices[rod_numvertices][3] = 0.0f;
	pen_vertices[rod_numvertices][4] = 0.0f; pen_vertices[rod_numvertices][5] = 0.0f;
	float angleStep = (2.0f * PI) / ((float) bob_numelements);
	for ( i = 1; i < bob_numvertices; i++ )
	{
		pen_vertices[rod_numvertices + i][0] = 0.0f + PEN_GL_BOB_RADIUS * cosf(angleStep * (float)i);
		pen_vertices[rod_numvertices + i][1] = -PEN_GL_ROD_LENGTH + PEN_GL_BOB_RADIUS * sinf(angleStep * (float)i);
		pen_vertices[rod_numvertices + i][2] = 0.0f;
		pen_vertices[rod_numvertices + i][3] = pen_vertices[rod_numvertices + i][0];
		pen_vertices[rod_numvertices + i][4] = pen_vertices[rod_numvertices + i][1] + PEN_GL_ROD_LENGTH;
		pen_vertices[rod_numvertices + i][5] = 0.0f;
	}

	pen_indices[3*rod_numelements + 0] = rod_numvertices + 0;
	pen_indices[3*rod_numelements + 1] = rod_numvertices + bob_numvertices-1;
	pen_indices[3*rod_numelements + 2] = rod_numvertices + 1;
	for ( i = 1; i < bob_numelements; i++ )
	{
		pen_indices[3*rod_numelements + i*3+0] = rod_numvertices + 0;
		pen_indices[3*rod_numelements + i*3+1] = rod_numvertices + i;
		pen_indices[3*rod_numelements + i*3+2] = rod_numvertices + i+1;
	}

	// initialize geometry data container

	geometry->stride = 6 * sizeof(float);
	geometry->vertex_offset = 0;
	geometry->normal_offset = 3 * sizeof(float);

	geometry->vertex_count = rod_numvertices + bob_numvertices;
	geometry->vertices_size = geometry->stride * geometry->vertex_count;
	geometry->vertices = malloc(geometry->vertices_size);
	
	geometry->element_count = (bob_numelements + rod_numelements);
	geometry->indices_size = sizeof(pen_indices);
	geometry->indices = malloc(geometry->indices_size);

	memcpy(geometry->vertices, pen_vertices, sizeof(pen_vertices));
	memcpy(geometry->indices, pen_indices, sizeof(pen_indices));
}

void GeometryFree (geometry_data* geometry) {
	free(geometry->vertices);
	free(geometry->indices);
}

void GeometryUpdatePendulum (geometry_data* geometry, float rodlen) {

	if ( rodlen < 0.0f ) return;

	// rodlen is a positive value, but the tip of the pendulum points downwards
	float rodlen_delta = -geometry->vertices[2*6+1] - rodlen;

	int i;
	for (i = 2; i < geometry->vertex_count ; i++) {
		geometry->vertices[i*6+1] += rodlen_delta;
	}
}

