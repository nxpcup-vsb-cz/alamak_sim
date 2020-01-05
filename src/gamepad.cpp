/** 
 * @file gamepad.cpp
 * @brief Module gamepad
 * @author michal.vasut.st@vsb.cz
 * @author supervisor: petr.olivka@vsb.cz
 *
 * This project was developed as part of Bachelor thesis (2019) by michal.vasut.st@vsb.cz, see http://dspace.vsb.cz.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <linux/joystick.h>

#include "gamepad.h"

/// Internal thread function
void *gamepadThread( void *t_args )
{
    GamepadThreadData *t_gamepad_data = ( ( GamepadThreadData * ) t_args );
    struct js_event js_ev;

    pollfd l_pfds = { t_gamepad_data->gamepad_fd, POLLIN, 0 };

    // test stop request
    while( !t_gamepad_data->stop_thread )     
    {
        // wait for data
        int l_err = poll( &l_pfds, 1, 10 );
        if ( !l_err ) continue; // timeout
        if ( l_err < 0 ) break; // error

        // get data
        l_err = read( t_gamepad_data->gamepad_fd, &js_ev, sizeof( js_ev ) );
        if ( l_err < 0 ) break;

        bool l_new_data = false;

        if ( js_ev.type == JS_EVENT_AXIS )
        {
            if ( js_ev.number == JS_AXIS_STEER_WHEEL ) {
                t_gamepad_data->axis_steer_wheel = js_ev.value;
                l_new_data = true;
            }

            if ( js_ev.number == JS_AXIS_SPEED )
            {
                t_gamepad_data->axis_speed = js_ev.value;
                l_new_data = true;
            }
        }
        else if ( js_ev.type == JS_EVENT_BUTTON )
        {
            if ( js_ev.number == JS_BUTTON_RESET )
            {
                if ( js_ev.value == 1 )
                {
                    t_gamepad_data->restart_button = true;
                    l_new_data = true;
                }
            }
        }
        t_gamepad_data->new_data = l_new_data;
    }

    printf( "Gamepad thread finished.\n" );
    return nullptr;
}


int gamepadStart( GamepadThreadData &t_gamepad_data, const char *t_dev_name )
{
    t_gamepad_data.stop_thread = false;
    t_gamepad_data.thread_id = -1;
    t_gamepad_data.new_data = false;
    t_gamepad_data.axis_speed = 0;
    t_gamepad_data.axis_steer_wheel = 0;
    t_gamepad_data.restart_button = 0;

    // open device
    if ( !t_dev_name ) t_dev_name = JS_DEFAULT_DEVICENAME;
    t_gamepad_data.gamepad_fd = open( t_dev_name, O_RDONLY );

    if ( t_gamepad_data.gamepad_fd < 0 ) return t_gamepad_data.gamepad_fd;

    // create thread
    pthread_create( &t_gamepad_data.thread_id, nullptr, gamepadThread, &t_gamepad_data );

    return t_gamepad_data.gamepad_fd;
}


int gamepadStop( GamepadThreadData &t_gamepad_data )
{
    if ( t_gamepad_data.gamepad_fd < 0 ) return -1;

    // request to stop thread
    t_gamepad_data.stop_thread = true;

    // wait for the thread
    pthread_join( t_gamepad_data.thread_id, nullptr );

    // close device
    close( t_gamepad_data.gamepad_fd );

    t_gamepad_data.gamepad_fd = -1;

    return 0;
}

