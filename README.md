    pendulum_pi -- Didactic Pendulum Simulation on the Raspberry Pi
    Copyright (C) 2014-2017  dwh simulation services

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

# Didactic Pendulum Simulation on the Raspberry Pi

The following setup of a classical pendulum serves the illustration of basic concepts of modeling and simulation:

- system abstraction and model simplification
- physical modeling with ordinary differential equations
- implementation of realtime simulation (ODE-solver) and visualization
- validation of simulation models

## Assembly and Structure of the Experiment

A pendulum with metal ball is placed in front of a computer monitor. An electromagnetic device at the side can keep the pendulum in its initial position and release it in a controlled manner. The electromagnet is controlled via a [Raspberry Pi](https://www.raspberrypi.org/) minicomputer which also runs the simulation program. The simulation is started synchronously to releasing the pendulum by turning off the magnet and starting a realtime visual animation of the simulated pendulum on the screen.

![assembly1](http://www.dwh.at/site/assets/files/1650/aufbau_beschreibung_en.300x0-is.jpg) ![assembly2](http://www.dwh.at/site/assets/files/1650/aufbau_closeup.314x0-is.jpg)

*Assembly with Lego components. The suspension of the pendulum is mounted on the monitor bezel.*


By adjusting the position of the electromagnetic release device, arbitrary initial conditions (displacement of the pendulum) can be chosen. Via keyboard entry, the initial displacement of the virtual pendulum can be adjusted to the pendulum in front of the screen.

With another keyboard entry the experiment is started. The electromagnet is turned off and simulation as well as visual output commence.

After a certain amount of time differences arise between the pendulum and its simulated counterpart. The differences are caused by multiple factors and are part of the presentation of the experiment.

Via configuration files and a configuration menu it is possible to adjust the parameters of the model. Hence the pendulum can be simulated with different parameters or parameter sets which can be predefined or modified in an interactive way. The configuration files must be adapted to fit the dimensions of the pendulum (masses, lengths, friction coefficients).

## Technical Implementation
### Hardware

The assembly requires basic knowledge about electricity and soldering of circuits. The following or similar electronic components are required:

**Warning: Improper handling with electricity can be dangerous! This documentation/report is no manual, guide or instruction and comes without any warranty!**

- Raspberry Pi 1 B
  - 32-bit ARMv6 Single Core 700 Mhz CPU
  - OpenGL-ES 2.0 compatible GPU
- Circuit
  - Transistor (TIP122)
  - Resistor 1kOhm
  - Diode (1n4007) 1kV 1A
  - Electromagnet specified for 12V
- Power Supply
  - 5V 1.5A and 12V 1.5A

The Raspberry Pi minicomputer is equipped with GPIO ports ([see documentation](https://www.raspberrypi.org/documentation/usage/gpio/)) which can be used for controlling the electromagnet. Additionally the minicomputer can be powered via GPIO ports instead of micro-USB.

**Warning: Improper use of the ports can damage the device ([see documentation](https://www.raspberrypi.org/documentation/usage/gpio/))!**

![raspi](http://www.dwh.at/site/assets/files/1650/raspi.291x0-is.jpg)

*A possible configuration of GPIO ports on the Raspberry Pi 1 B for connecting the circuit (see below) and for power supply.*

Via a transistor the power supply of the electromagnet is controlled. The transistor can be switched with a signal from the GPIO ports of the minicomputer. Power supplies and signal connection can be implemented in a single circuit .

![circuit1](http://www.dwh.at/site/assets/files/1650/schaltkreis.235x0-is.jpg) ![circuit2](http://www.dwh.at/site/assets/files/1650/schaltplan.204x0-is.png)

*Schematic circuit (without power supply of the minicomputer).*

### Software

The control and simulation program is written in C. The code is available under the [GPLv3](LICENSE) license. Following libraries are required:

| library | description | use |
| --- | --- | --- |
| libxml2 | | configuration files |
| ncurses, cdk | | configuration menu |
| OpenGL ES, EGL | Raspberry Pi VideoCore APIs | visualization |
| GSL | GSL – Ordinary Differential Equations | ODE-solver |


At startup the program is in configuration mode. Positioning, lengths and initial angle of displacement can be adjusted and saved via keyboard entry.

After triggering the simulation (and concurrent release of the magnet) alternating the pendulum equation is solved up to a precalculated time point and the displacement of the pendulum is drawn on the screen (realtime simulation).

When the simulation is canceled the program returns to its configuration mode.
