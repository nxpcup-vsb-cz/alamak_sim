
#pragma once

/** 
 * @file copsim_car.h
 * @brief Module copsim_car
 * @author michal.vasut.st@vsb.cz
 * @author supervisor: petr.olivka@vsb.cz
 *
 * This project was developed as part of Bachelor thesis (2019) by michal.vasut.st@vsb.cz, see http://dspace.vsb.cz.
 *
 * This module copsim_car represents interface between a car control program and the CoppeliaSim scene with car model. 
 */

/**
 * @mainpage List of all modules.
 * @see trackview.h
 * @see gamepad.h
 * @see copsim_car.h
 * @see demo_car_simple.cpp
 * @see demo_car_gamepad.cpp
 *
 * @brief The Alamak car model in the CoppeliaSim -- Robotics Simulator.
 *
 * The associated simulator scene contains virtual model of the Alamak car. 
 * This car is used for NXP Cup, the competition of autonomous driving cars. 
 * The model is created in real dimensions with real weight (including battery).
 *
 * The scene also contains all parts of racing track in real dimensions: 
 * the left and right curve, straight part, chicane and intersection. 
 * In addition the short and long hill parts are added. 
 * They is not official parts of racing track, 
 * but they were adopted from previous generation of racing track. 
 */

#include <math.h>

extern "C" {
    #include "extApi.h"
}

/// Line camera (vision sensor) resolution. 
#define CAR_CAM_RESOLUTION              128 

/// Rotation of 5th (a virtual front) steering wheel in degrees
#define CAR_5TH_WHEEL_ANGLE_DEG         30
/// Rotation of 5th wheel in RAD 
#define CAR_5TH_WHEEL_ANGLE_RAD         ( CAR_5TH_WHEEL_ANGLE_DEG * M_PI / 180 )

/// The Alamak wheel diameter 
#define CAR_WHEEL_DIAMETER_M           0.064
/// The limit of car speed in meters per seconds 
#define CAR_MAX_SPEED_M_S               1.2
/// The car speed recalculated to rotation speed 
#define CAR_MAX_SPEED_DEG_S             ( ( CAR_MAX_SPEED_M_S / ( CAR_WHEEL_DIAMETER_M * M_PI ) ) * 360 )

/// The real torque on a rear wheels. 
#define CAR_MAX_REAL_TORQUE_N_M         0.1
/// Reduced torque for model, the real torque is too high
#define CAR_MAX_TORQUE_N_M              ( CAR_MAX_REAL_TORQUE_N_M / 3 )

/// The timeout for next image from line camera (vision sensor)
#define CAR_GETIMAGE_TIMEOUT_MS         5000

/// The object names in CoppeliaSim scene 
/// @name 
/// @{
#define COPPSIM_OBJNAME_LEFT_MOTOR      "Motor_Left"
#define COPPSIM_OBJNAME_RIGHT_MOTOR     "Motor_Right"
#define COPPSIM_OBJNAME_SERVO           "Servo"
#define COPPSIM_OBJNAME_VISION_SENSOR   "Vision_Sensor"
/// @}

/**
 * @brief The interface between the car model in CoppeliaSim and a remote control program. 
 *
 * The implementation of this class must to integrate two different tasks:
 *   - The public interface must offers similar interface as can be expected in microcomputer: 
 *   power control of DC motors, set position of steering servo and capture image from line camera. 
 *   - The internal interface must substitute a missing real car hardware with car model in
 *   CoppeliaSim using the Remote API interface. 
 *
 */
class CoppeliaSimCar
{
public:

    /** Constructor initializes member variables. */
    CoppeliaSimCar();
    /** Destructor */
    ~CoppeliaSimCar();

    /**
     * @brief Initialization opens connection to CoppeliaSim.
     *
     * This method opens connection to CoppeliaSim on localhost and it also initializes all
     * necessary handles for crucial object in CoppeliaSim scene. 
     * Also the data streaming from CoppeliaSim is started. 
     *
     * @param t_port_number The port number opened in CoppeliaSim for Remote API connection. 
     * @return When the initialization passed correctly the function returns 0, otherwise -1.
     */
    int init( int t_port_number );

    /** @brief Capture single image from line camera (vision sensor).
     *
     * This method waits for next image from line camera, represented by vision sensor. 
     * The timeout for the next image is internally specified by @ref CAR_GETIMAGE_TIMEOUT_MS. 
     *
     * @param t_img Buffer for 8bit B&W image from line camera. 
     * The length of this buffer is defined by vision sensor resolution. 
     * In this code is represented by \ref CAR_CAM_RESOLUTION.
     * @return When an image from CoppeliaSim is captured correctly, it returns 0. Otherwise -1.  
     */
    int getImage( unsigned char *t_img );

    /** @brief Set servo position.
     *
     * This method set target position of steering servo. The servo position is equal to the position 
     * of the 5th (virtual) wheel of car
     *
     * @param t_position The position of servo is represented by value of range <-1.0, 1.0>. 
     * According to right-hand rule the value 1.0 is turning to the left and -1.0 to the right. 
     */
    void setServo( float t_position );

    /** @brief Set power of motors. 
     *
     * This method sets power of the both car motors. The power is virtually represented by values of range
     * <-1.0, 1.0>. The positive value is to move forward and negative value to move backward.
     * @note On the microcomputer the power of motors is typically controlled by PWM. 
     * In CoppeliaSim the motor power is represented by torque. 
     * Internally the PWM is recalculated to torque specified by reduced torque \ref CAR_MAX_TORQUE_N_M.
    */
    void setMotorPWM( float t_l_pwm, float t_r_pwm );

    /** @brief Move car back to starting position in the CoppeliaSim scene. 
     */
    void resetCar();

protected:

    /** @brief The interface between CoppeliaSim Remote API and \ref setServo.
     */
    int copsimSetServoPosition( float t_angle );

    /** @brief The interface between CoppeliaSim Remote API and \ref setMotorPWM.
     */
    int copsimSetMotorTorque( float t_l_torque, float t_r_torque );

    int m_client_id;                    ///< Remote API Client ID of connection to CoppeliaSim 
    int m_left_motor_handle;            ///< Handle for left motor \ref COPPSIM_OBJNAME_LEFT_MOTOR
    int m_right_motor_handle;           ///< Handle for right motor \ref COPPSIM_OBJNAME_RIGHT_MOTOR
    int m_servo_handle;                 ///< Handle for servo \ref COPPSIM_OBJNAME_SERVO
    int m_vision_sensor_handle;         ///< Handle for vision sensor \ref COPPSIM_OBJNAME_VISION_SENSOR !g!D

    bool m_copsim_initialized;          ///< Internal variable

};



