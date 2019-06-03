#include <bluefruit.h>

#include "bleWriteEvent.h"
#include "buzzer.h"
#include "motor_control.h"

// Device and pin
#define SW1 29
#define SW2 28
#define LED_DS2 7
#define LED_DS3 11
#define LED_DS4 19
#define LED_DS5 17
#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define GPIO5 5
#define GPIO12 12
#define GPIO13 13
#define GPIO14 14
#define GPIO15 15
#define GPIO16 16

static volatile int g_flag_user_write = 0;
static volatile int g_data_user_write_led0 = 0;
static volatile int g_data_user_write_led1 = 0;
static volatile int g_data_user_write_led2 = 0;
static volatile int g_data_user_write_led3 = 0;
static volatile int g_data_user_write_buzzer = 0;
static volatile int g_data_user_write_io2 = 0;
static volatile int g_data_user_write_io3 = 0;
static volatile int g_data_user_write_io4 = 0;
static volatile int g_data_user_write_io5 = 0;
static volatile int g_data_user_write_io12 = 0;
static volatile int g_data_user_write_io13 = 0;
static volatile int g_data_user_write_io14 = 0;
static volatile int g_data_user_write_io15 = 0;
static volatile int g_data_user_write_io16 = 0;

volatile int g_flag_update_advertiseuuid = 0;
volatile int g_flag_error_advertiseuuid = 0;

/**
   Format

   <CMD(1Byte), don't care(2Byte), hash of payload/don't care(1Byte),
   Payload(16Byte)> CMD0 : Write to peripheral device. LED, buzzer and GPIO
      CMD0(0:1Byte), don't care(0:17Byte), Peripheral(x:2Byte)
   CMD1 : Write new service UUID : When update UUID, Should be self restart MPU:
      CMD1(1:1Byte), don't care(0,2Byte), hash of payload(x:1Byte),
      UUID(x:16Byte) UUID shoud be send binary.
*/
void bleWriteEvent(uint16_t conn_handle, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  byte cmd = data[0];
  byte index = data[1];
  byte length = data[2];
  byte hash = data[3];

  Serial.println("UUID Change ");

  if (cmd == 0) {
    // Write peripheral device and GPIO
    g_data_user_write_led0 = (data[19] >> 7) & 1;
    g_data_user_write_led1 = (data[19] >> 6) & 1;
    g_data_user_write_led2 = (data[19] >> 5) & 1;
    g_data_user_write_led3 = (data[19] >> 4) & 1;
    g_data_user_write_buzzer = (data[19] >> 3) & 1;
    g_data_user_write_io2 = (data[19] >> 2) & 1;
    g_data_user_write_io3 = (data[19] >> 1) & 1;
    g_data_user_write_io4 = data[19] & 1;
    g_data_user_write_io5 = (data[18] >> 7) & 1;
    g_data_user_write_io12 = (data[18] >> 6) & 1;
    g_data_user_write_io13 = (data[18] >> 5) & 1;
    g_data_user_write_io14 = (data[18] >> 4) & 1;
    g_data_user_write_io15 = (data[18] >> 3) & 1;
    g_data_user_write_io16 = (data[18] >> 2) & 1;
    g_flag_user_write = 1;

    int speed;
    if (g_data_user_write_io2 == 1) {
      speed = 100;
    } else {
      speed = 30;
    }

    if (g_data_user_write_led0 == 1) {
      cmd_motor(0, speed);
    } else if (g_data_user_write_led1 == 1) {
      cmd_motor(0, -speed);
    } else {
      cmd_motor(0, 0);
    }

    if (g_data_user_write_led2 == 1) {
      cmd_motor(1, speed);
    } else if (g_data_user_write_led3 == 1) {
      cmd_motor(1, -speed);
    } else {
      cmd_motor(1, 0);
    }

    if (g_data_user_write_io12 == 1) {
      cmd_servo(1, 150, 30);
    } else if (g_data_user_write_io13 == 1) {
      cmd_servo(1, 30, 30);
    } else {
      cmd_servo(1, 90, 100);
    }

  } else if (cmd == 1) {
    // Write New Advertising UUID
    byte generatedHash = 0;
    for (int i = 0; i < 16; i++) {
    //   blesv_user_uuid[15 - i] = data[4 + i];
      generatedHash += data[4 + i];
    }

    g_flag_update_advertiseuuid = 1;

    if (generatedHash != hash) {
      g_flag_error_advertiseuuid = 1;
    }
  }
}

void ble_write_loop(){
// Set LED and buzzer value
  if (g_flag_user_write) {
    board_user_write();
    g_flag_user_write = 0;
  }
}
  
void board_user_write() {
  // Set GPIO output mode
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);
  pinMode(GPIO4, OUTPUT);
  pinMode(GPIO5, OUTPUT);
  pinMode(GPIO12, OUTPUT);
  pinMode(GPIO13, OUTPUT);
  pinMode(GPIO14, OUTPUT);
  pinMode(GPIO15, OUTPUT);
  pinMode(GPIO16, OUTPUT);
  // Buzzer
  if (g_data_user_write_buzzer) {
    buzzerStart();
  } else {
    buzzerStop();
  }
  // LED
  digitalWrite(LED_DS2, g_data_user_write_led0);
  digitalWrite(LED_DS3, g_data_user_write_led1);
  digitalWrite(LED_DS4, g_data_user_write_led2);
  digitalWrite(LED_DS5, g_data_user_write_led3);
  // GPIO
  digitalWrite(GPIO2, g_data_user_write_io2);
  digitalWrite(GPIO3, g_data_user_write_io3);
  digitalWrite(GPIO4, g_data_user_write_io4);
  digitalWrite(GPIO5, g_data_user_write_io5);
  digitalWrite(GPIO12, g_data_user_write_io12);
  digitalWrite(GPIO13, g_data_user_write_io13);
  digitalWrite(GPIO14, g_data_user_write_io14);
  digitalWrite(GPIO15, g_data_user_write_io15);
  digitalWrite(GPIO16, g_data_user_write_io16);
}
