#include <bluefruit.h>
#include <Servo.h>

#define B_IA 2
#define B_IB 3
#define A_IA 4
#define A_IB 5

#define SERVO0 13
#define SERVO1 14
#define SERVO2 15

Servo servo0;
Servo servo1;
Servo servo2;

void motor_onboard_init(void)
{
  pinMode(B_IA, OUTPUT);
  pinMode(B_IB, OUTPUT);
  pinMode(A_IA, OUTPUT);
  pinMode(A_IB, OUTPUT);

  servo0.attach(SERVO0);
  servo1.attach(SERVO1);
  servo2.attach(SERVO2);
}

void servo_control_onboard(uint8_t servo, uint8_t angle, uint8_t speed)
{
  Serial.print("servo:");
  Serial.print(servo);
  Serial.print("\tangle:");
  Serial.print(angle);
  Serial.print("\tspeed:");
  Serial.print(speed);
  Serial.println("");

  if (servo == 0)
  {
    servo0.write(angle);
  }
  else if (servo == 1)
  {
    servo1.write(angle);
  }
  else if (servo == 2)
  {
    servo2.write(angle);
  }
  else
  {
  }
}

void motor_control_onboard(uint8_t motor, uint8_t speed)
{
  int _speed = (int8_t)speed;
  _speed = map(_speed, -100, 100, -255, 255);

  Serial.print("motor:");
  Serial.print(motor);
  Serial.print("\tspeed:");
  Serial.print(_speed);
  Serial.println("");

  if (motor == 0)
  {
    if (_speed > 0)
    {
      analogWrite(A_IA, _speed);
      analogWrite(A_IB, 0);
    }
    else if (_speed == 0)
    {
      analogWrite(A_IA, 0);
      analogWrite(A_IB, 0);
    }
    else
    {
      analogWrite(A_IA, 0);
      analogWrite(A_IB, -_speed);
    }
  }
  else if (motor == 1)
  {
    if (_speed > 0)
    {
      analogWrite(B_IA, _speed);
      analogWrite(B_IB, 0);
    }
    else if (_speed == 0)
    {
      analogWrite(B_IA, 0);
      analogWrite(B_IB, 0);
    }
    else
    {
      analogWrite(B_IA, 0);
      analogWrite(B_IB, -_speed);
    }
  }
}
