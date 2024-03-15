/*
 * Module to read accelerometer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 * Based on Julie Zelenski's version from Mar 1 2024 https://github.com/cs107e/cs107e.github.io/tree/master/lectures/Sensors/code/accel
 *  Adafruit LSM6DS3TR + LIS3MDL - Precision 9 DoF IMU Accel + Gyro + Magnetometer 
 *          https://www.adafruit.com/product/5543 
 *          Datasheet https://cdn-learn.adafruit.com/downloads/pdf/adafruit-lsm6ds3tr-c-lis3mdl-precision-9-dof-imu.pdf
 */

// FROM CS107E ********************************************************

#pragma once
#include <stdbool.h>

// initializes the accelerometer
void lsm6ds33_init(void) ;

// ADITI'S NEW FUNCTIONS***********************************************

enum { LEFT = 0, HOME, RIGHT };
enum { X_HOME = 0, X_FAST, X_SWAP };

//// general-purpose:

void lsm6ds33_read_accelerometer_all(short *x, short *y, short *z) ;
void lsm6ds33_read_accelerometer_x(short *x) ;
void lsm6ds33_read_accelerometer_y(short *y) ;
void lsm6ds33_read_accelerometer_z(short *z) ;

//// specific to tetris:

// reads only x and y values of accelerometer
void lsm6ds33_read_accelerometer_x_y(short *x, short *y);

// reads an avg of x,y,z and returns what 
//  y: tilt the accelerometer is at (LEFT/HOME/RIGHT) - roll
//  x: tilt the accelerometer is at (HOME/FAST/SLAM) - pitch
void lsm6ds33_read_durable_pos(short *x, short *y, int *y_state, int *x_state) ;
