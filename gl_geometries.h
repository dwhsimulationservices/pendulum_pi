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

typedef struct {
	unsigned int element_count;
	unsigned int vertex_count;
	float* vertices;
	unsigned short* indices;
	unsigned int stride;
	int vertices_size;
	int indices_size;
	int vertex_offset;
	int normal_offset;
	float suspx;
	float suspy;
} geometry_data;

void GeometryFree (geometry_data* geometry);
void GeneratePendulumGeometry (geometry_data* geometry);
void GeometryUpdatePendulum (geometry_data* geometry, float rodlen);
