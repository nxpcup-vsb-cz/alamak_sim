
#pragma once

/** 
 * @file trackview.h
 * @brief Module trackview
 * @author michal.vasut.st@vsb.cz
 * @author supervisor: petr.olivka@vsb.cz
 *
 * This project was developed as part of Bachelor thesis (2019) by michal.vasut.st@vsb.cz, see http://dspace.vsb.cz.
 *
 * The module trackview displays images captured by vision sensor using OpenCV library.
 */

/** 
 * @brief Function starts trackview.
 *
 * This function initializes all necessary data to display captured images. 
 * It creates standalone thread and open an associate window to continuously display captured data. 
 * @return When trackview started without error, return 0. Otherwise return -1. 
 */
int trackviewStart();

/**
 * @brief Function stops trackview.
 *
 * This function stops the trackview thread and it closes associated windows.  
 *
 * @return When trackview finished correctly the function return 0, otherwise return -1.
 */
int trackviewStop();

/**
 * @brief Add captured single line image.
 *
 * This function adds one single line image at the top of previous images. 
 * The oldest one line is automatically removed. 
 * The content of associated windows is refreshed automatically. 
 */ 
void trackviewAddImage( unsigned char *t_img );



