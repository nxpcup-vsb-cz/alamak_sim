/** 
 * @file trackview.cpp
 * @brief Module trackview
 * @author michal.vasut.st@vsb.cz
 * @author supervisor: petr.olivka@vsb.cz
 *
 * This project was developed as part of Bachelor thesis (2019) by michal.vasut.st@vsb.cz, see http://dspace.vsb.cz.
 *
 * This demo is remote control program of car model in CoppeliaSim. 
 * The simple demo shows how to control car using class \ref CoppSimCar. 
 * The car is periodically a while moving forward and a while moving backward. 
 *
 * For more information see header files or use doxygen. 
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

#include "copsim_car.h"

int main(int argc,char* argv[])
{

    int portNb=0;

    if (argc>=2)
    {
        portNb=atoi(argv[1]);
    }
    else
    {
        printf("Parametr port!\n");
        return 1;
    }

    CoppeliaSimCar l_coppsim_car;
    if ( l_coppsim_car.init( portNb ) < 0 ) 
    {
        fprintf( stderr, "CoppeliaSim not connected!\n" );
        exit( 1 );
    }

    fprintf( stderr, "Type 'quit'...\n" );
    pollfd l_pfd = { 0, POLLIN };

    int l_turn = 0;
    int l_tout_limit = 100;
    int l_tout = l_tout_limit / 2; 

    while ( true ) 
    {
        // Image from camera is ignored. Function is used only for synchronization with CoppeliaSim. 
        if ( l_coppsim_car.getImage( nullptr ) < 0 )
        {
            fprintf( stderr, "Unable to get image!\n" );
            break;
        }

        if ( !l_tout-- ) 
        {
            l_tout = l_tout_limit;
            l_turn = !l_turn;
        }

        if ( l_turn )
        {
            l_coppsim_car.setServo( -0.5 );
            l_coppsim_car.setMotorPWM( 0.5, 0.1 );
        }
        else
        {
            l_coppsim_car.setServo( 0.5 );
            l_coppsim_car.setMotorPWM( -0.1, -0.5 );
        }


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
    fprintf( stderr, "...done.\n" );

    return 0;
}


