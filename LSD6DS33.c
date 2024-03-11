/*
 * Module to read accelerometer
 * Author: Aditi Bhaskar (aditijb@stanford.edu)
 * Based on Julie Zelenski's version from Mar 1 2024
 */

/*
    Adafruit LSM6DS3TR + LIS3MDL - Precision 9 DoF IMU
    Accel + Gyro + Magnetometer
    https://www.adafruit.com/product/5543
    Datasheet https://cdn-learn.adafruit.com/downloads/pdf/adafruit-lsm6ds3tr-c-lis3mdl-precision-9-dof-imu.pdf

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

// TODO -- new idea -- define a struct that holds onto the previous few lsd angle values (do avg/throw out outliers math)
//      use those fields to add a few function "get diff" and if the difference is over a certain value, return elements in the left/right/down enum

static struct {
    short prev_x;
    short prev_y;
    short prev_z;
    int state;
} LSD6DS33_device = {
    .prev_x = 0,
    .prev_y = 0,
    .prev_z = 0,
    .state = HOME // LEFT = 0, HOME, RIGHT
};

// LSM6DS33 6-Axis IMU (0x6A or 0x6B) - https://learn.adafruit.com/i2c-addresses/the-list
// static const unsigned MY_I2C_ADDR = 0x6A; // confirm device id, components can differ!
static const unsigned MY_I2C_ADDR = 0x6B; // connected 3.3Vout to VDD (https://learn.adafruit.com/lsm6ds33-6-dof-imu=accelerometer-gyro/arduino)

static void write_reg(unsigned char reg, unsigned char val) {
    unsigned char data[2] = {reg, val};
	i2c_write(MY_I2C_ADDR, data, 2);
}

static unsigned read_reg(unsigned char reg) {
	i2c_write(MY_I2C_ADDR, &reg, 1);
	unsigned char val = 0;
	i2c_read(MY_I2C_ADDR, &val, 1);

	return val;
}

void lsm6ds33_read_accelerometer(short *x, short *y, short *z) {
    *x =  read_reg(OUTX_L_XL);
    *x |= read_reg(OUTX_H_XL) << 8;
    *y =  read_reg(OUTY_L_XL);
    *y |= read_reg(OUTY_H_XL) << 8;
    *z =  read_reg(OUTZ_L_XL);
    *z |= read_reg(OUTZ_H_XL) << 8;
}

static void lsm6ds33_read_accelerometer_durable(short *x, short *y, short *z) {
    // todo write function
    // take the lsm6ds33_read_accelerometer() like 10 times, avg.

    int x_sum = 0 ; int y_sum = 0 ; int z_sum = 0 ;
    unsigned int n = 5 ;
    for(int i = 0; i < n; i++) {
        int x_read = read_reg(OUTX_L_XL);
        x_read |= read_reg(OUTX_H_XL) << 8;
        int y_read =  read_reg(OUTY_L_XL);
        y_read |= read_reg(OUTY_H_XL) << 8;
        int z_read =  read_reg(OUTZ_L_XL);
        z_read |= read_reg(OUTZ_H_XL) << 8;
        x_sum += x_read ;
        y_sum += y_read ;
        z_sum += z_read ;

    }

    *x = (short)(x_sum/n) ;
    *y = (short)(y_sum/n) ;
    *z = (short)(z_sum/n) ;

}

void lsm6ds33_init(void) {
    printf("in accel init 0") ;

    unsigned id = read_reg(WHO_AM_I);  // confirm id, expect 0x69
    printf("in accel init 1") ;

    assert(id == 0x69); 
    
	write_reg(CTRL1_XL, 0x80);  // 1600Hz (high perf mode)
    // accelerator _XL registers
    write_reg(CTRL9_XL, 0x38);  // ACCEL: x,y,z enabled (bits 4-6)

    lsm6ds33_read_accelerometer_durable(&LSD6DS33_device.prev_x, &LSD6DS33_device.prev_y, &LSD6DS33_device.prev_z) ;
}

// use this function to get what the tilt status of accelerometer is
int lsm6ds33_read_accelerometer_delta(short *dx, short *dy, short *dz) {
    // todo write function

    // get reading
    short x = 0 ; short y = 0 ; short z = 0 ;
    lsm6ds33_read_accelerometer_durable(&x, &y, &z) ;

    // store deltas
    *dx = x - LSD6DS33_device.prev_x ;
    *dy = y - LSD6DS33_device.prev_y ;
    *dz = z - LSD6DS33_device.prev_z ;

    // update "previous" readings for next time
    LSD6DS33_device.prev_x = x;
    LSD6DS33_device.prev_y = y;
    LSD6DS33_device.prev_z = z;

    printf("\ndeltas! dx %d, dy %d, dz %d\n", *(dx), *dy, *dz) ;

    // decide what the deltas mean and return a useful enum to the user!
    // dy > 10000 -> clockwise/right
    // dy > 20000 -> 180 right
    // dy < -10000 -> clockwise/left
    // dy < -20000 -> 180 left
    
    int _d_180 = 20000 ; // todo fiddle and figure out this one
    int _d_90 = 6000 ; // used to be 10000

    int ret = NONE ;
    if (*dy > _d_180) {
        ret = RIGHT_180 ;
    } else if (*dy < -_d_180) {
        ret = LEFT_180 ;
    } else if (*dy > _d_90) {
        ret = RIGHT_90 ;
    } else if (*dy < -_d_90) {
        ret = LEFT_90 ;
    } 

    return ret ;
}

// updates dx, dy, dz to the most recent durable read
// returns the device's state: home (center), left tilt, or right tilt. use enum for easy access
int lsm6ds33_read_accelerometer_pos(short *dx, short *dy, short *dz) {

    // read in an update to accelerometer state
    int update = lsm6ds33_read_accelerometer_delta(dx, dy, dz) ;

    // update the state 
    if (LSD6DS33_device.state < RIGHT && update == RIGHT_90) {
        LSD6DS33_device.state ++ ;
    } else if (LSD6DS33_device.state > LEFT && update == LEFT_90) {
        LSD6DS33_device.state -- ;
    } else if (LSD6DS33_device.state == RIGHT && update == LEFT_180) {
        LSD6DS33_device.state = LEFT ;
    } else if (LSD6DS33_device.state == LEFT && update == RIGHT_180) {
        LSD6DS33_device.state = RIGHT ;
    } else if (update != NONE) {
        printf("\n*** illegal attempt to change tilt status ***\n") ;
    }

    printf("\n device.state %s\n", LSD6DS33_device.state==1?"home":(LSD6DS33_device.state==0?"left":"right")) ;
    
    return LSD6DS33_device.state ;
}

// returns true if there was a down-flick of the remote
bool lsm6ds33_read_accelerometer_drop(short dz) {
    if (dz > 2000) { // flick remote tip towards ground
        return true ;
    }
    return false ;
}

