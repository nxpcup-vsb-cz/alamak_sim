/** 
 * @file trackview.cpp
 * @brief Module trackview
 * @author michal.vasut.st@vsb.cz
 * @author supervisor: petr.olivka@vsb.cz
 *
 * This project was developed as part of Bachelor thesis (2019) by michal.vasut.st@vsb.cz, see http://dspace.vsb.cz.
 *
 * This demo is remote control program of car model in CoppeliaSim. 
 * The control of car is by gamepad. 
 * The left thumstick is used to control power and the right thumbstick to control steering. 
 *
 * The image from line camera is periodicaly captured and continuously display using module trackview. 
 *
 * For more information see header files or use doxygen. 
 *
 */


#include <unistd.h>
#include <pthread.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#include "copsim_car.h"
#include "trackview.h"

#define TRACKVIEW_WINNAME       "Track View"
#define TRACKVIEW_WINHEIGHT     256

cv::Mat g_trackview_img;

int g_trackview_initialized = 0;
int g_trackview_thread_stop = 0;
pthread_t g_trackview_thread_id;

pthread_mutex_t g_trackview_mutex = PTHREAD_MUTEX_INITIALIZER;


/// Internal thread function
void *trackviewThread( void *t_arg )
{
    if ( !g_trackview_initialized ) return nullptr;

    while ( !g_trackview_thread_stop )
    {
        pthread_mutex_lock( &g_trackview_mutex );

        cv::imshow( TRACKVIEW_WINNAME, g_trackview_img );

        cv::waitKey( 1 );
    }
    return nullptr;
}


int trackviewStart()
{
    if ( g_trackview_initialized ) return 0;

    g_trackview_img = cv::Mat( TRACKVIEW_WINHEIGHT, CAR_CAM_RESOLUTION, CV_8UC1 );
    g_trackview_img.setTo( 255 );
    cv::namedWindow( TRACKVIEW_WINNAME, CV_WINDOW_NORMAL );
    cv::resizeWindow( TRACKVIEW_WINNAME, CAR_CAM_RESOLUTION * 2, TRACKVIEW_WINHEIGHT * 2 );
    cv::imshow( TRACKVIEW_WINNAME, g_trackview_img );

    g_trackview_initialized = 1;
    g_trackview_thread_stop = 0;

    pthread_mutex_trylock( &g_trackview_mutex );

    pthread_create( &g_trackview_thread_id, nullptr, trackviewThread, nullptr );

    return 0;
}


int trackviewStop()
{
    if ( !g_trackview_initialized ) return -1;

    g_trackview_thread_stop = 1;
    pthread_mutex_unlock( &g_trackview_mutex );
    pthread_join( g_trackview_thread_id, nullptr );

    g_trackview_img.release();
    cv::destroyWindow( TRACKVIEW_WINNAME );

    g_trackview_initialized = 0;

    return 0;
}


void trackviewAddImage( unsigned char *t_img )
{
    memmove( g_trackview_img.ptr( 1 ), g_trackview_img.ptr( 0 ), sizeof( uchar ) * ( g_trackview_img.total() - g_trackview_img.cols ) );
    memcpy( g_trackview_img.ptr( 0 ), t_img, sizeof( uchar ) * g_trackview_img.cols );

    pthread_mutex_unlock( &g_trackview_mutex );
}


