/** 
 * @file copsim_car.cpp
 * @brief Module copsim_car
 * @author michal.vasut.st@vsb.cz
 * @author supervisor: petr.olivka@vsb.cz
 *
 * This project was developed as part of Bachelor thesis (2019) by michal.vasut.st@vsb.cz, see http://dspace.vsb.cz.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>

#include "copsim_car.h"


CoppeliaSimCar::CoppeliaSimCar() 
{
    m_copsim_initialized = false;

    m_client_id = -1;
}


CoppeliaSimCar::~CoppeliaSimCar() 
{
    if ( m_client_id >= 0 )
        simxFinish( m_client_id );
}


int CoppeliaSimCar::init( int t_port_number )
{
    m_client_id = simxStart( ( simxChar * ) "127.0.0.1", t_port_number, true, true, 2000, 5 );
    if ( m_client_id < 0 ) 
    {
        fprintf( stderr, "Unable to connect CoppeliaSim!\n" );
        return -1;
    }

    int l_simx_ret;
    l_simx_ret = simxGetObjectHandle( m_client_id, COPPSIM_OBJNAME_LEFT_MOTOR, &m_left_motor_handle, simx_opmode_blocking );
    if ( l_simx_ret != simx_return_ok )
    {
        fprintf( stderr, "Unable to get handle for %s\n", COPPSIM_OBJNAME_LEFT_MOTOR );
        return -1;
    }
    l_simx_ret = simxGetObjectHandle( m_client_id, COPPSIM_OBJNAME_RIGHT_MOTOR, &m_right_motor_handle, simx_opmode_blocking );
    if ( l_simx_ret != simx_return_ok )
    {
        fprintf( stderr, "Unable to get handle for %s\n", COPPSIM_OBJNAME_RIGHT_MOTOR );
        return -1;
    }
    l_simx_ret = simxGetObjectHandle( m_client_id, COPPSIM_OBJNAME_SERVO, &m_servo_handle, simx_opmode_blocking );
    if ( l_simx_ret != simx_return_ok )
    {
        fprintf( stderr, "Unable to get handle for %s\n", COPPSIM_OBJNAME_SERVO );
        return -1;
    }
    l_simx_ret = simxGetObjectHandle( m_client_id, COPPSIM_OBJNAME_VISION_SENSOR, &m_vision_sensor_handle, simx_opmode_blocking );
    if ( l_simx_ret != simx_return_ok )
    {
        fprintf( stderr, "Unable to get handle for %s\n", COPPSIM_OBJNAME_VISION_SENSOR );
        return -1;
    }

    return 0;
}


int CoppeliaSimCar::getImage( unsigned char *t_image )
{
    int l_retval;
    simxUChar* l_image_camera;
    int l_cam_resolution[ 2 ] = { CAR_CAM_RESOLUTION, 1 };  //resolution of vision sensor

    // current connection is valid?
    if ( simxGetConnectionId( m_client_id ) < 0  ) return -1;

    // start visual sensor streaming
    if ( !m_copsim_initialized )
    {

        l_retval = simxGetVisionSensorImage( m_client_id, m_vision_sensor_handle, 
                l_cam_resolution, &l_image_camera, 1, simx_opmode_streaming );
        if ( l_retval != simx_return_novalue_flag && l_retval != simx_return_ok ) return -1;

        m_copsim_initialized = true; 
    }
    
    // 5s timeout should be enough even on slow computer
    int l_time_limit = CAR_GETIMAGE_TIMEOUT_MS;

    // wait for next image
    while ( l_time_limit-- &&
           ( ( l_retval = simxGetVisionSensorImage( m_client_id, m_vision_sensor_handle, 
                            l_cam_resolution, &l_image_camera, 1, simx_opmode_buffer ) ) != simx_return_ok )
            && ( simxGetConnectionId( m_client_id ) != -1 ) 
         )
    {
        usleep( 1000 ); // wait 1 ms
    }

    // timeout or some error? 
    if ( l_time_limit < 0 || l_retval != simx_return_ok ) return -1;

    // copy data
    if ( t_image )
        memcpy( t_image, l_image_camera, sizeof( simxUChar ) * CAR_CAM_RESOLUTION );
  
    // remove image from the internal API buffer
    l_retval = simxGetVisionSensorImage(  m_client_id, m_vision_sensor_handle, 
            l_cam_resolution, &l_image_camera, 1, simx_opmode_remove );

    if ( l_retval != simx_return_ok ) return -1;

    return 0; 
}


void CoppeliaSimCar::setServo( float t_position )
{
    // verify allowed range of values
    t_position = MIN( t_position, 1.0 );
    t_position = MAX( t_position, - 1.0 );

    // recalculate position to angle and call internal method
    copsimSetServoPosition( t_position * CAR_5TH_WHEEL_ANGLE_RAD );
}


void CoppeliaSimCar::setMotorPWM( float t_l_pwm, float t_r_pwm )
{
    // verify allowed range of the both pwm values
    t_l_pwm = MIN( t_l_pwm, 1.0 );
    t_l_pwm = MAX( t_l_pwm, - 1.0 );

    t_r_pwm = MIN( t_r_pwm, 1.0 );
    t_r_pwm = MAX( t_r_pwm, - 1.0 );

    // recalculate PWM to torque and call internal method
    copsimSetMotorTorque( t_l_pwm * CAR_MAX_TORQUE_N_M, t_r_pwm * CAR_MAX_TORQUE_N_M );
}


void CoppeliaSimCar::resetCar()
{
    copsimSetServoPosition( 0.0 );
    copsimSetMotorTorque( 0.0, 0.0 );
    if ( simxCallScriptFunction( m_client_id, "Board", sim_scripttype_childscript , "restart", 0, NULL, 0, NULL, 0,NULL,0, NULL,0, NULL, 0, NULL, 0, NULL, 0, NULL, simx_opmode_blocking ) != simx_return_ok )
        fprintf( stderr, "Method %s failed!\n", __FUNCTION__ );
}


int CoppeliaSimCar::copsimSetServoPosition( float t_angle )
{
    t_angle = MIN( t_angle, CAR_5TH_WHEEL_ANGLE_RAD );
    t_angle = MAX( t_angle, -CAR_5TH_WHEEL_ANGLE_RAD );

    // call Remote API
    int l_retval = simxSetJointTargetPosition( m_client_id, m_servo_handle, t_angle, simx_opmode_oneshot );
    if ( l_retval != simx_return_ok )
    {
        fprintf( stderr, "Method %s failed!\n", __FUNCTION__ );
        return -1;
    }

    return 0;
}


int CoppeliaSimCar::copsimSetMotorTorque( float t_l_torque, float t_r_torque )
{
    t_l_torque = MIN( t_l_torque, CAR_MAX_TORQUE_N_M );
    t_l_torque = MAX( t_l_torque, -CAR_MAX_TORQUE_N_M );
    t_r_torque = MIN( t_r_torque, CAR_MAX_TORQUE_N_M );
    t_r_torque = MAX( t_r_torque, -CAR_MAX_TORQUE_N_M );

    float l_l_speed = CAR_MAX_SPEED_DEG_S;
    float l_r_speed = CAR_MAX_SPEED_DEG_S;

    // the left motor use the positive rotation direction for a forward moving
    if ( t_l_torque < 0 ) 
    {
        l_l_speed = - l_l_speed;
        t_l_torque = - t_l_torque;
    }

    // the right motor use the negative rotation direction for a forward moving
    t_r_torque = - t_r_torque;
    if ( t_r_torque < 0 ) 
    {
        l_r_speed = - l_r_speed;
        t_r_torque = - t_r_torque;
    }

    int l_retval = 0;

    // call Remote API
    l_retval |= simxSetJointTargetVelocity( m_client_id, m_left_motor_handle, l_l_speed, simx_opmode_oneshot );
    l_retval |= simxSetJointForce( m_client_id, m_left_motor_handle, t_l_torque, simx_opmode_oneshot );
    l_retval |= simxSetJointTargetVelocity( m_client_id, m_right_motor_handle, l_r_speed, simx_opmode_oneshot );
    l_retval |= simxSetJointForce( m_client_id, m_right_motor_handle, t_r_torque, simx_opmode_oneshot );

    if ( l_retval != simx_return_ok )
    {
        fprintf( stderr, "Method %s failed!\n", __FUNCTION__);
        return -1;
    }

    return 0;
}

