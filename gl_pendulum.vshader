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

attribute vec4 VertexPosition;
uniform float ScreenRatio;
uniform float Rotation;
uniform vec4 Translation;
void main () {
	float sin_phi = sin(Rotation);
	float cos_phi = cos(Rotation);
	mat4 RotScale = mat4(	cos_phi,  sin_phi*ScreenRatio, 0.0, 0.0,
				-sin_phi, cos_phi*ScreenRatio, 0.0, 0.0,
				0.0,      0.0,                 1.0, 0.0,
				0.0,      0.0,                 0.0, 1.0);
	gl_Position = (RotScale * VertexPosition) + Translation;
}
