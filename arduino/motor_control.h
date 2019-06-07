#ifndef MOTOR_CTRL_H_
#define MOTOR_CTRL_H_

#include <bluefruit.h>

void motor_init();
void cmd_motor(uint8_t motor_no, int8_t speed);
void cmd_servo(uint8_t servo_no, uint8_t angle, uint8_t speed);
void i2c_scanner();


#endif