#ifndef MOTOR_CTRL_ONBOARD_H_
#define MOTOR_CTRL_ONBOARD_H_

#include <bluefruit.h>

void motor_onboard_init(void);
void motor_control_onboard(uint8_t servo, uint8_t speed);
void servo_control_onboard(uint8_t motor, uint8_t angle, uint8_t speed);

#endif