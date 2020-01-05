/** 
 * @file demo_car_gamepad.cpp
 * @brief Module trackview
 * @author michal.vasut.st@vsb.cz
 * @author supervisor: petr.olivka@vsb.cz
 *
 * This project was developed as part of Bachelor thesis (2019) by michal.vasut.st@vsb.cz, see http://dspace.vsb.cz.
 *
 */

// Make sure to have the server side running in CoppeliaSim!
// Start the server from a child script with following command:
// simExtRemoteApiStart(portNumber) -- starts a remote API server service on the specified port

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#include "gamepad.h"
#include "copsim_car.h"
#include "trackview.h"

#define HELP                                                        \
    "Usage: %s [-h] [-notrack] port_number\n"                       \
    "  -h               this help\n"                                \
    "  -notrack         do not display track\n"                     \
    "  port_number      localhost port number for Remote API\n\n" 

int main( int argc, char* argv[] )
{

    int l_port_num = -1;
    int l_help = 0;
    int l_notrack = 0;

    for ( int i = 1; i < argc; i++ )
    {
        if ( !strcmp( argv[ i ], "-h" ) )
        {
            l_help = 1;
        }
        if ( !strcmp( argv[ i ], "-notrack" ) )
        {
            l_notrack = 1;
        }
        if ( *argv[ i ] != '-' )
        {
            l_port_num = atoi( argv[ i ] );
        }

    }
    if ( l_port_num < 0 || l_help )
    {
        printf( HELP, argv[ 0 ] );
        exit( 0 );
    }

    CoppeliaSimCar l_coppsim_car;
    if ( l_coppsim_car.init( l_port_num ) < 0 ) 
    {
        fprintf( stderr, "CoppeliaSim not connected!\n" );
        exit( 1 );
    }

    GamepadThreadData l_gamepad_data;
    if ( gamepadStart( l_gamepad_data ) < 0 )
    {
        fprintf( stderr, "Unable to initialize gamepad!\n" );
        exit( 1 );
    }

    if ( !l_notrack && trackviewStart() < 0 )
    {
        fprintf( stderr, "Unable to initialize trackview!\n" );
        exit( 1 );
    }

    fprintf( stderr, "Type 'quit'...\n" );
    pollfd l_pfd = { 0, POLLIN };

    while ( true ) 
    {
        unsigned char l_img[ CAR_CAM_RESOLUTION ];
        if ( l_coppsim_car.getImage( l_img ) < 0 )
        {
            fprintf( stderr, "Unable to get image!\n" );
            break;
        }
        if ( !l_notrack ) trackviewAddImage( l_img );

        if ( l_gamepad_data.restart_button )
        {
            l_coppsim_car.resetCar();
            l_gamepad_data.restart_button = false;
            continue;
        }
    
        // servo position computed from thumbstick position
        float l_servo = 0;
        if ( abs( l_gamepad_data.axis_steer_wheel ) > JS_AXIS_STEPS / 1000 )
            l_servo = - ( float ) l_gamepad_data.axis_steer_wheel / JS_AXIS_STEPS;

        // the motor's power is computed from the thumbstick position
        float l_pwm = 0;
        if ( abs( l_gamepad_data.axis_speed ) > JS_AXIS_STEPS / 1000 )
            l_pwm = - ( float ) l_gamepad_data.axis_speed / JS_AXIS_STEPS;
        l_pwm *= fabs( l_pwm ); // nonlinear power control
        float l_l_pwm = l_pwm;
        float l_r_pwm = l_pwm;

        // power reduced on an inner wheel
        float l_inner_wheel_reduction = 0.8;
        if ( l_servo > 0 )
            l_l_pwm = l_l_pwm * ( 1.0 - fabs( l_servo ) * l_inner_wheel_reduction ); 
        if ( l_servo < 0 )
            l_r_pwm = l_r_pwm * ( 1.0 - fabs( l_servo ) * l_inner_wheel_reduction ); 

        l_coppsim_car.setServo( l_servo );
        l_coppsim_car.setMotorPWM( l_l_pwm, l_r_pwm );

        if ( poll( &l_pfd, 1, 0 ) == 1 )
        {
            char l_line[ 128 ];
            int l_len = read( 0, l_line, sizeof( l_line ) );
            if ( l_len > 0 )
            {
                l_line[ l_len ] = 0;
                if ( strcasecmp( l_line, "quit" ) == 0 ) break;
            }
        }
    }

    fprintf( stderr, "Application exiting....\n" );

    gamepadStop( l_gamepad_data );
    if ( !l_notrack ) trackviewStop();

    fprintf( stderr, "...done.\n" );

    return 0;
}


