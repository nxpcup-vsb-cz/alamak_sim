
#pragma once

/** 
 * @file gamepad.h
 * @brief Module gamepad
 * @author michal.vasut.st@vsb.cz
 * @author supervisor: petr.olivka@vsb.cz
 *
 * This project was developed as part of Bachelor thesis (2019) by michal.vasut.st@vsb.cz, see http://dspace.vsb.cz.
 *
 * The module gamepad is continuously gaining data from gamepad and it stores current positions of thumbsticks. 
 * Currently tested with Logitech F710
 */


#include <pthread.h>

/// Default name of gamepad device.
#define JS_DEFAULT_DEVICENAME           "/dev/input/js0"

/// The axis used for steering.
#define JS_AXIS_STEER_WHEEL             3
/// The axis used for speed (power) control.
#define JS_AXIS_SPEED                   1
/// The button for car model reset.
#define JS_BUTTON_RESET                 5
/// Resolution/range of gamepad axis
#define JS_AXIS_STEPS                   32767

/// Structure to control thread and storing gamepad data.
struct GamepadThreadData 
{
    int gamepad_fd;                     ///< File descriptor of opened device.
    bool stop_thread;                   ///< Request to stop gamepad thread.
    pthread_t thread_id;                ///< Thread ID.

    bool new_data;                      ///< Detect change on gamepad.
    int axis_speed;                     ///< Current position of speed axis.
    int axis_steer_wheel;               ///< Current position of steering axis.
    bool restart_button;                ///< Reset button pressed.
};

/**
 * @brief Function starts the gamepad module.
 *
 * This function initializes all necessary data for gamepad module and it also started standalone thread
 * to continuouly gain data from gamepad. 
 *
 * @param t_gamepad_data Structure for gamepad module control and storing data. 
 * @param t_dev_name Alternate device name
 * @return When gamepad module was initialized correctly, return 0. Otherwise -1.
 */
int gamepadStart( GamepadThreadData &t_gamepad_data, const char *t_dev_name = nullptr );

/** @brief Function stops gamepad module. 
 *  
 *  This function deactivates module gamepad, it stops associate thread and close opened device. 
 *  
 *  @param t_gamepad_data The structure with previously initalized data. 
 *  @return When module was stopped correctly, return 0. Otherwise -1.
 */
int gamepadStop( GamepadThreadData &t_gamepad_data );


