/*
 * Module to read accelerometer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 * Based on Julie Zelenski's version from Mar 1 2024
 */

/*
    accelerometer LSD6DS33.c  
    wiring!! sda = GPIO_PG13, scl = GPIO_PB7  -> from i2c.c
*/

#include "LSD6DS33.h"
#include "assert.h"
#include "i2c.h"
#include "printf.h"
#include "timer.h"
#include "interrupts.h"
#include "ringbuffer.h"
#include "malloc.h"
#include "console.h"

enum reg_address {
    WHO_AM_I  = 0x0F, // original
    CTRL1_XL  = 0x10,
    CTRL8_XL  = 0x17,
    CTRL9_XL  = 0x18,
    OUTX_L_XL = 0x28,
    OUTX_H_XL = 0x29,
    OUTY_L_XL = 0x2A,
    OUTY_H_XL = 0x2B,
    OUTZ_L_XL = 0x2C,
    OUTZ_H_XL = 0x2D,
};

// LSM6DS33 6-Axis IMU (0x6A or 0x6B) - https://learn.adafruit.com/i2c-addresses/the-list 
// static const unsigned MY_I2C_ADDR = 0x6A; // confirm device id, components can differ!
static const unsigned MY_I2C_ADDR = 0x6B; // connected 3.3Vout to SDO (https://learn.adafruit.com/lsm6ds33-6-dof-imu=accelerometer-gyro/arduino)

// writes to an accelerometer register
static void write_reg(unsigned char reg, unsigned char val) {
    unsigned char data[2] = {reg, val};
	i2c_write(MY_I2C_ADDR, data, 2);
}

// reads an accelerometer register
static unsigned read_reg(unsigned char reg) {
	i2c_write(MY_I2C_ADDR, &reg, 1);
	unsigned char val = 0;
	i2c_read(MY_I2C_ADDR, &val, 1);

	return val;
}

// initializes the accelerometer
void lsm6ds33_init(void) {
    printf("in accel init") ;

    unsigned id = read_reg(WHO_AM_I);  // confirm id, expect 0x69
    assert(id == 0x69); 
    
	write_reg(CTRL1_XL, 0x80);  // 1600Hz (high perf mode)
    // accelerator _XL registers
    write_reg(CTRL9_XL, 0x38);  // ACCEL: x,y,z enabled (bits 4-6)
}

/// GENERAL-PURPOSE FUNCTIONS

// reads the accelerometer x y z values
void lsm6ds33_read_accelerometer_all(short *x, short *y, short *z) {
    *x =  read_reg(OUTX_L_XL);
    *x |= read_reg(OUTX_H_XL) << 8;
    *y =  read_reg(OUTY_L_XL);
    *y |= read_reg(OUTY_H_XL) << 8;
    *z =  read_reg(OUTZ_L_XL);
    *z |= read_reg(OUTZ_H_XL) << 8;
}

// reads the accelerometer values for an axis
void lsm6ds33_read_accelerometer_x(short *x) {
    *x =  read_reg(OUTX_L_XL);
    *x |= read_reg(OUTX_H_XL) << 8;
}
void lsm6ds33_read_accelerometer_y(short *y) {
    *y =  read_reg(OUTY_L_XL);
    *y |= read_reg(OUTY_H_XL) << 8;
}
void lsm6ds33_read_accelerometer_z(short *z) {
    *z =  read_reg(OUTZ_L_XL);
    *z |= read_reg(OUTZ_H_XL) << 8;
}


/// USED FOR TETRIS

// Calibrated values; sensor-specific. 
// Tuned by Aditi Mar 11 2024
#define LEFT_ANGLE -8000 // for y
#define RIGHT_ANGLE 7000 // for y
#define HOME_ANGLE -4000 // for y
#define X_FAST_DOWN 9000 // for x
#define X_SWAP_UP -12000 // for x

// reads the accelerometer x y values
void lsm6ds33_read_accelerometer(short *x, short *y, short *z) {
    *x =  read_reg(OUTX_L_XL);
    *x |= read_reg(OUTX_H_XL) << 8;
    *y =  read_reg(OUTY_L_XL);
    *y |= read_reg(OUTY_H_XL) << 8;
}

// durably (n samples) reads accelerometer x y values
static void lsm6ds33_read_accelerometer_durable(short *x, short *y, short *z) {
    int x_sum = 0 ; int y_sum = 0 ; //int z_sum = 0 ;
    unsigned int n = 3 ;
    for(int i = 0; i < n; i++) {
        int x_read = read_reg(OUTX_L_XL);
        x_read |= read_reg(OUTX_H_XL) << 8;
        int y_read =  read_reg(OUTY_L_XL);
        y_read |= read_reg(OUTY_H_XL) << 8;
        x_sum += x_read ;
        y_sum += y_read ;
    }

    *x = (short)(x_sum/n) ;
    *y = (short)(y_sum/n) ;
}

// edits x_state and y_state, passed by reference with the (durably read)
//      y (roll) position's meaning  (LEFT/HOME/RIGHT) 
//      x (pitch) position's meaning  (HOME/FAST/SWAP) 
void lsm6ds33_read_durable_pos(short *x, short *y, short *z, int *x_state, int *y_state) {

    lsm6ds33_read_accelerometer_durable(x, y, z) ; // shorter bc I only read data once

    *x_state = X_HOME ;

    // update y info
    if (*y < LEFT_ANGLE) { 
        *y_state = LEFT;
    } else if (*y > RIGHT_ANGLE) {
        *y_state = RIGHT;
    } else {
        *y_state = HOME;
        if (*x > X_FAST_DOWN) {     
            *x_state = X_FAST ;// update x info
        } else if (*x < X_SWAP_UP) {
            *x_state = X_SWAP ;
        } 
    } 

}
