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

#include <signal.h>
#include "par.h"

void ui_listen_simulation (pendulum_configuration* conf, volatile sig_atomic_t* simflag);
void ui_listen_setup (pendulum_configuration* conf, volatile sig_atomic_t* simflag, volatile sig_atomic_t* setupflag);
int ui_listen_config (pendulum_configuration* conf, volatile sig_atomic_t* stopflag);

int ui_init ();
void ui_terminate ();
void ui_clear ();
void ui_print (const char *format, ...);
