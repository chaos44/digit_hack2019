#include <bluefruit.h>
#include <Servo.h>

#include <nrf.h>

// #define SERVO0 11
// #define SERVO1 16
// #define SERVO2 14
// Servo servo0;
// Servo servo1;
// Servo servo2;

#define B_IA_PIN 2
#define B_IB_PIN 3
#define A_IA_PIN 4
#define A_IB_PIN 5

#define B_IA_DUTY_IDX 0
#define B_IB_DUTY_IDX 1
#define A_IA_DUTY_IDX 2
#define A_IB_DUTY_IDX 3

#define B_IA_DUTY 16000
#define B_IB_DUTY 16000
#define A_IA_DUTY 16000
#define A_IB_DUTY 16000

static uint16_t pwm_seq[4] = {B_IA_DUTY, B_IB_DUTY, A_IA_DUTY,
                              A_IB_DUTY};

static void start_pwm0(void);
static void set_duty(uint8_t pin, uint8_t duty);

static void set_duty(uint8_t pin, uint8_t duty)
{
  uint32_t comp = 16000 - (16000 * duty) / 100;
  switch (pin)
  {
  case B_IA_PIN:
    pwm_seq[B_IA_DUTY_IDX] = comp;
    NRF_PWM0->TASKS_SEQSTART[0] = 1;
    break;
  case B_IB_PIN:
    pwm_seq[B_IB_DUTY_IDX] = comp;
    NRF_PWM0->TASKS_SEQSTART[0] = 1;
    break;
  case A_IA_PIN:
    pwm_seq[A_IA_DUTY_IDX] = comp;
    NRF_PWM0->TASKS_SEQSTART[0] = 1;
    break;
  case A_IB_PIN:
    pwm_seq[A_IB_DUTY_IDX] = comp;
    NRF_PWM0->TASKS_SEQSTART[0] = 1;
    break;
  }
}

static void start_pwm0(void)
{
  NRF_PWM0->PSEL.OUT[0] = (B_IA_PIN << PWM_PSEL_OUT_PIN_Pos) |
                          (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
  NRF_PWM0->PSEL.OUT[1] = (B_IB_PIN << PWM_PSEL_OUT_PIN_Pos) |
                          (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
  NRF_PWM0->PSEL.OUT[2] = (A_IA_PIN << PWM_PSEL_OUT_PIN_Pos) |
                          (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
  NRF_PWM0->PSEL.OUT[3] = (A_IB_PIN << PWM_PSEL_OUT_PIN_Pos) |
                          (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);

  NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
  NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
  NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
  NRF_PWM0->COUNTERTOP = (16000 << PWM_COUNTERTOP_COUNTERTOP_Pos); //1 msec
  NRF_PWM0->LOOP = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);
  NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) |
                      (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);
  NRF_PWM0->SEQ[0].PTR = ((uint32_t)(pwm_seq) << PWM_SEQ_PTR_PTR_Pos);
  NRF_PWM0->SEQ[0].CNT = ((sizeof(pwm_seq) / sizeof(uint16_t)) << PWM_SEQ_CNT_CNT_Pos);
  NRF_PWM0->SEQ[0].REFRESH = 0;
  NRF_PWM0->SEQ[0].ENDDELAY = 0;
  NRF_PWM0->TASKS_SEQSTART[0] = 1;
}

void motor_onboard_init(void)
{
  pinMode(B_IA_PIN, OUTPUT);
  pinMode(B_IB_PIN, OUTPUT);
  pinMode(A_IA_PIN, OUTPUT);
  pinMode(A_IB_PIN, OUTPUT);

  // servo0.attach(SERVO0);
  // servo1.attach(SERVO1);
  // servo2.attach(SERVO2);

  start_pwm0();
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
    // servo0.write(angle);
  }
  else if (servo == 1)
  {
    // servo1.write(angle);
  }
  else if (servo == 2)
  {
    // servo2.write(angle);
  }
  else
  {
  }
}

void motor_control_onboard(uint8_t motor, uint8_t speed)
{
  int _speed = (int8_t)speed;

  Serial.print("motor:");
  Serial.print(motor);
  Serial.print("\tspeed:");
  Serial.print(_speed);
  Serial.println("");

  if (motor == 0)
  {
    if (_speed > 0)
    {
      set_duty(A_IA_PIN, _speed);
      set_duty(A_IB_PIN, 0);
    }
    else if (_speed == 0)
    {
      set_duty(A_IA_PIN, 0);
      set_duty(A_IB_PIN, 0);
    }
    else
    {
      set_duty(A_IA_PIN, 0);
      set_duty(A_IB_PIN, -_speed);
    }
  }
  else if (motor == 1)
  {
    if (_speed > 0)
    {
      set_duty(B_IA_PIN, _speed);
      set_duty(B_IB_PIN, 0);
    }
    else if (_speed == 0)
    {
      set_duty(B_IA_PIN, 0);
      set_duty(B_IB_PIN, 0);
    }
    else
    {
      set_duty(B_IA_PIN, 0);
      set_duty(B_IB_PIN, -_speed);
    }
  }
}
