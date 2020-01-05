# Alamak Car Simulation 

This project was created as part of Bachelor thesis by michal.vasut.st(at)vsb.cz. See http://dspace.vsb.cz.

The model of Alamak car was created in CoppeliaSim robotic environment in real dimensions and with real weight (including battery). 
Also all parts of racing track were created in real dimensions. 

The control of Alamak model is possible by CoppeliaSim Remote API in the same way as a real car. 
For more informations about programming interface see doxygen documentation from source codes. 

## Content

- [Requirements](#requirements)
- [Scene](#scene)
- [Track](#track)
- [Compile](#compile)
- [Start](#start)
- [Programming](#programming)

## Requirements

The list of requirements for this project:
- Ubuntu 18.04 or 16.04 with installed packages ``make``, ``g++`` and ``libopencv-dev``.
  Also the ``doxygen`` if you want generate programming documentation. 
- Download CoppeliaSim from http://www.coppeliarobotics.com/.
- Download this project.

This project should also work in Windows but not tested yet. 

## Scene

Start CoppeliaSim and open scene ``alamak.ttt`` from directory ``scene``:

``shell$ /your_path/coppeliaSim.sh scene/alamak.ttt``

Now should be visible the scene depicted in Figure 1. 

<img src="../img/copsim_scene_stop.jpg" width="640">

Figure. 1: CoppeliaSim scene with Alamak car

In Figure 1 is visible CoppeliaSim environment with Alamak car model. 
The hierarchy of all objects in the scene are visible on the left side, marked by red H. 
The names of objects are important, because they are used in this documentation and also in 
source codes:
in  the internal LUA scripts in scene and in the remote control programs for Remote API interface. 

Two windows are on right side of scene. 
The smaller one, marked by red V, will show output from ``Visual_sensor`` which simulates a line camera.
The larger one, marked by red C, will shows view from backside ``Camera``. 

The Alamak and all parts of the racing track are in the centre of scene. 
The detail of Alamak model is visible in Figure 2. 

<img src="../img/copsim_alamak_detail.jpg" width="480">

Figure 2: Detail of Alamak model

## Track

The racing track will be automatically assembled after starting simulation (see next chapter) by a LUA script in the object ``TrackGenerator``. 

The selection from a set of predefined tracks must be performed when a simulation is stopped. 
The track dialog will be open clicking on object ``TrackGenerator``, see Figure 3. 

<img src="../img/copsim_track_dialog.jpg">

Figure 3: Selection from a set of predefined tracks

The definition of own track is possible in script of the object ``TrackGenerator``.
There is possible define racing track as a sequence of individual parts of track in string.  

The list of parts is:
- S - Straight,
- L - Left,
- R - Right,
- H - Hill,
- U - Hill Up,
- D - Hill Down,
- I - Intersection,
- O - Offset,
- C - Chicane.

The simple racing track is visible in Figure 4 and it is defined as following string:

``track = "S R S L S R S R S S S O R S S O S R"``

For more information open script of the object ``TrackGenerator``.

Note: Hills are not included in the current racing track and they were adopted into this simulator from previous version of racing track. 

## Compile

Before the simulation will be started it is necessary to compile programs for remote control. 

Two programs are prepared in this project to demonstrate how to control Alamak model in simulator:

- ``demo_car_simple``
- ``demo_car_gamepad``

The both source codes and addition modules are in directory ``src``.
They can be easily compiled using ``make``. 
But at first it is necessary to modify a path where the CoppeliaSim is installed.
It is one line in file ``Makefile``:

``COPSIM_DIR=/opt/CoppeliaSim``

It will be the same directory as was used in chapter [Scene](#scene) to start CoppeliaSim. 

Now easy compile programs by ``make``:

``shell$ make``

When the compilation passed correctly, two programs are prepared for the next step.

## Start

The complete simulation must be started in two steps:

- start simulation in CoppeliaSim,
- start remote control program from command line. 

The Alamak simulation must be started in CoppeliaSim by Start button, in Figure 4 marked by red S. 
When a simulation is slow on some computer, it is possible to increase speed by rocket icon ``Threaded Rendering``, marked in Figure 4 by red R.

<img src="../img/copsim_scene_run.jpg" width="640">

Figure 4: Started simulation in CoppeliaSim

When simulation started, the port number for remote API is displayed in output box on the bottom of CoppeliaSim. 
In Figure 4 is marked by red P. 
This port number will be used for remote control program in following step.

When the simulation is running in the CoppeliaSim, it is possible to start remote control programs in directory ``src``:

``shell$ ./demo_car_simple port_number``

of

``shell$ ./demo_car_gamepad port_number``

The first one is very simple example how to control Alamak model. 
This program periodically switch between positive and negative power on rear wheels. 
It also switch steering servo between left and right direction. 

The second program uses gamepad to control Alamak model. 
Currently the Logitech F710 is tested. 
This program also shows output from ``Vision_sensor`` and it displays all captured line images in a separate window.
The simulation with gamepad is depicted in Figure 5. 

<img src="../img/copsim_scene_gamepad.jpg" width="640">

Figure 5: Simulation controlled by gamepad with trackview window on left side

### Timing

By default the CoppeliaSim uses timing 50 ms per simulation step. 
The scene with Alamak uses timing 25 ms. 
The autonomous control will need 10 ms to be synchronized with timing of a real line camera. 
The timing must be set before a simulation is started in toolbar of CoppeliaSim, in Figure 4 marked by red T. 

## Programming

The programming interface for remote car control consists of three method of class ``CopSimCar``
in files copsim_car.h and cpp. 

- ``int getImage( unsigned char *t_img );`` - get image from ``Vision_sensor``. This function is synchronized with CoppeliaSim.
- ``void setServo( float t_position );`` - set steering servo.
- ``void setMotorPWM( float t_l_pwm, float t_r_pwm );`` - set power of rear motors. 

To get more information generate programming documentation using ``doxygen`` in directory ``src``:

``shell$ doxygen doxygen.conf``

Documentation will be created in directory ``html``. 

**GOOD LUCK!**
