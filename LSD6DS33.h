/*
 * Module to read accelerometer
 * Author: Aditi (aditijb@stanford.edu)
 * Based on Julie Zelenski's version from Mar 1 2024 https://github.com/cs107e/cs107e.github.io/tree/master/lectures/Sensors/code/accel
 *  Adafruit LSM6DS3TR + LIS3MDL - Precision 9 DoF IMU Accel + Gyro + Magnetometer 
 *          https://www.adafruit.com/product/5543 
 *          Datasheet https://cdn-learn.adafruit.com/downloads/pdf/adafruit-lsm6ds3tr-c-lis3mdl-precision-9-dof-imu.pdf
 */

#pragma once
#include <stdbool.h>

// FROM CS107E PROVIDED CODE ////////////////////////////////////////////////////////////

// initializes the accelerometer
void lsm6ds33_init(void) ;

// ADITI'S NEW FUNCTIONS ////////////////////////////////////////////////////////////////

enum { LEFT = 0, HOME, RIGHT };
enum { X_HOME = 0, X_FAST, X_SWAP };

// GENERAL-PURPOSE FUNCTIONS

/* lsm6ds33_read_accelerometer_all
 * @params short *x, short *y, short *z - user-passed shorts which will be updated to the raw x, y, z values read from accelerometer
 * @return - through the params
 * @functionality - reads x, y, z values from accelerometer
*/
void lsm6ds33_read_accelerometer_all(short *x, short *y, short *z) ;

/* lsm6ds33_read_accelerometer_x
 * @params short *x - user-passed shorts which will be updated to the raw x value read from accelerometer
 * @return - through the params
 * @functionality - reads x values from accelerometer
*/
void lsm6ds33_read_accelerometer_x(short *x) ;

/* lsm6ds33_read_accelerometer_y
 * @params short *y - user-passed shorts which will be updated to the raw y value read from accelerometer
 * @return - through the params
 * @functionality - reads y values from accelerometer
*/
void lsm6ds33_read_accelerometer_y(short *y) ;

/* lsm6ds33_read_accelerometer_z
 * @params short *z - user-passed shorts which will be updated to the raw z value read from accelerometer
 * @return - through the params
 * @functionality - reads z values from accelerometer
*/
void lsm6ds33_read_accelerometer_z(short *z) ;


// TETRIS-SPECIFIC FUNCTIONS

/* lsm6ds33_read_accelerometer_x_y
 * @params short *x, short *y - user-passed shorts which will be updated to the raw x, y values read from accelerometer
 * @return - through the params
 * @functionality - reads only x and y values of accelerometer. z readings not used by tetris game
*/
void lsm6ds33_read_accelerometer_x_y(short *x, short *y);

/* lsm6ds33_read_durable_pos
 * @params short *x, short *y - user-passed shorts which will be updated to the raw x, y values read from accelerometer
 * @params int *y_state, int *x_state - user-passed shorts which will be updated to the user-friendly x- and y- angle ranges that the accelerometer is in
 * @return - through all params
 * @functionality - reads an avg of x,y,z and returns what 
 *                       y_state: tilt the accelerometer is at (LEFT/HOME/RIGHT) - roll
 *                       x_state: tilt the accelerometer is at (HOME/FAST/SLAM) - pitch
*/
void lsm6ds33_read_durable_pos(short *x, short *y, int *y_state, int *x_state) ;

