/*
 * Module to read accelerometer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 * Based on Julie Zelenski's version from Mar 1 2024
 */

// from https://github.com/cs107e/cs107e.github.io/tree/master/lectures/Sensors/code/accel
// Adafruit LSM6DS3TR + LIS3MDL - Precision 9 DoF IMU Accel + Gyro + Magnetometer https://www.adafruit.com/product/5543 Datasheet https://cdn-learn.adafruit.com/downloads/pdf/adafruit-lsm6ds3tr-c-lis3mdl-precision-9-dof-imu.pdf
// Sample code to read the accelerometer in LSD6DS33.c

#pragma once
#include <stdbool.h>

enum { LEFT_180 = 0, LEFT_90, NONE, RIGHT_90, RIGHT_180, DOWN };
enum { LEFT = 0, HOME, RIGHT };

void lsm6ds33_init(void) ;
void lsm6ds33_read_accelerometer(short *x, short *y, short *z);
int lsm6ds33_read_accelerometer_delta(short *dx, short *dy, short *dz) ;
int lsm6ds33_read_accelerometer_pos(short *dx, short *dy, short *dz) ;
bool lsm6ds33_read_accelerometer_drop(short dz) ;