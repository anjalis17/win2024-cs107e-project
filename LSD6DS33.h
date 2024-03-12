/*
 * Module to read accelerometer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 * Based on Julie Zelenski's version from Mar 1 2024 https://github.com/cs107e/cs107e.github.io/tree/master/lectures/Sensors/code/accel
 */
// Adafruit LSM6DS3TR + LIS3MDL - Precision 9 DoF IMU Accel + Gyro + Magnetometer https://www.adafruit.com/product/5543 Datasheet https://cdn-learn.adafruit.com/downloads/pdf/adafruit-lsm6ds3tr-c-lis3mdl-precision-9-dof-imu.pdf

#pragma once
#include <stdbool.h>

enum { LEFT = 0, HOME, RIGHT };
enum { X_HOME = 0, X_FAST, X_SLAM };


// initializes the accelerometer
void lsm6ds33_init(void) ;

// reads x, y, and z values from the accelerometer
void lsm6ds33_read_accelerometer(short *x, short *y, short *z);

// Aditi's new functions ~~

void lsm6ds33_read_durable_pos(short *x, short *y, short *z, int *y_state, int *x_state) ;

// reads an avg of x,y,z and returns what tilt the accelerometer is at (LEFT/HOME/RIGHT)
// int lsm6ds33_read_durable_pos_y(short *x, short *y, short *z) ;

// reads the x-tilt position of the sensor and returns whether the remote is pointing at the ground
// bool lsm6ds33_durable_pos_x(short x) ;
