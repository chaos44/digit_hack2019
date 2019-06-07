#include <bluefruit.h>
#include "motor_control.h"

// Device Name: Maximum 30 bytes
#define DEVICE_NAME "LINE Things Trial dev board"

// User service UUID: Change this to your generated service UUID
#define USER_SERVICE_UUID "3fd1e37b-83a3-4691-8a70-dc42cd486ef7"
// User service characteristics
#define WRITE_CHARACTERISTIC_UUID "E9062E71-9E62-4BC6-B0D3-35CDCD9B027B"
#define NOTIFY_CHARACTERISTIC_UUID "62FBD229-6EDD-4D1A-B554-5C4E1BB29169"

// PSDI Service UUID: Fixed value for Developer Trial
#define PSDI_SERVICE_UUID "E625601E-9E55-4597-A598-76018A0D293D"
#define PSDI_CHARACTERISTIC_UUID "26E2B12B-85F0-4F3F-9FDD-91D114270E6E"

#define BUTTON 29
#define LED1 7

uint8_t userServiceUUID[16];
uint8_t psdiServiceUUID[16];
uint8_t psdiCharacteristicUUID[16];
uint8_t writeCharacteristicUUID[16];
uint8_t notifyCharacteristicUUID[16];

BLEService userService;
BLEService psdiService;
BLECharacteristic psdiCharacteristic;
BLECharacteristic notifyCharacteristic;
BLECharacteristic writeCharacteristic;

volatile int btnAction = 0;

void setup()
{
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, 0);
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(BUTTON, buttonAction, CHANGE);

  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);

  setupServices();
  startAdvertising();

  motor_init();
}

void loop()
{
  uint8_t btnRead;

  while (btnAction > 0)
  {
    btnRead = !digitalRead(BUTTON);
    btnAction = 0;
    notifyCharacteristic.notify(&btnRead, sizeof(btnRead));
    delay(20);
  }
}

void setupServices(void)
{
  // Convert String UUID to raw UUID bytes
  strUUID2Bytes(USER_SERVICE_UUID, userServiceUUID);
  strUUID2Bytes(PSDI_SERVICE_UUID, psdiServiceUUID);
  strUUID2Bytes(PSDI_CHARACTERISTIC_UUID, psdiCharacteristicUUID);
  strUUID2Bytes(WRITE_CHARACTERISTIC_UUID, writeCharacteristicUUID);
  strUUID2Bytes(NOTIFY_CHARACTERISTIC_UUID, notifyCharacteristicUUID);

  // Setup User Service
  userService = BLEService(userServiceUUID);
  userService.begin();

  writeCharacteristic = BLECharacteristic(writeCharacteristicUUID);
  writeCharacteristic.setProperties(CHR_PROPS_WRITE);
  writeCharacteristic.setWriteCallback(writeLEDCallback);
  writeCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  writeCharacteristic.setFixedLen(1);
  writeCharacteristic.begin();

  notifyCharacteristic = BLECharacteristic(notifyCharacteristicUUID);
  notifyCharacteristic.setProperties(CHR_PROPS_NOTIFY);
  notifyCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
  notifyCharacteristic.setFixedLen(1);
  notifyCharacteristic.begin();

  // Setup PSDI Service
  psdiService = BLEService(psdiServiceUUID);
  psdiService.begin();

  psdiCharacteristic = BLECharacteristic(psdiCharacteristicUUID);
  psdiCharacteristic.setProperties(CHR_PROPS_READ);
  psdiCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_NO_ACCESS);
  psdiCharacteristic.setFixedLen(sizeof(uint32_t) * 2);
  psdiCharacteristic.begin();

  // Set PSDI (Product Specific Device ID) value
  uint32_t deviceAddr[] = {NRF_FICR->DEVICEADDR[0], NRF_FICR->DEVICEADDR[1]};
  psdiCharacteristic.write(deviceAddr, sizeof(deviceAddr));
}

void startAdvertising(void)
{
  // Start Advertising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(userService);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);
}

void buttonAction()
{
  btnAction++;
}

typedef enum
{
  LED_WRITE = 0,
  MOTOR_WRITE = 1,
  SERVO_WRITE = 2,
  MOTOR_CONTROL_MODE = 3,
} write_type_t;

typedef enum
{
  MOTOR_CONTROL_AUTO = 0,
  MOTOR_CONTROL_MANUAL = 1,
} motor_control_mode_t;

void writeLEDCallback(uint16_t conn_hdl, BLECharacteristic *chr, uint8_t *data, uint16_t len)
{
  static motor_control_mode_t motor_control_mode = MOTOR_CONTROL_AUTO;
  write_type_t type = (write_type_t)data[0];
  int value = *data;

  switch (type)
  {
  case LED_WRITE:
  {
    uint8_t state;
    if (data[1] == 0)
    {
      state = 0;
      Serial.print("LED:OFF");
    }
    else
    {
      state = 1;
      Serial.print("LED:ON");
    }
    digitalWrite(LED1, state);

    break;
  }
  case MOTOR_WRITE:
  {
    if (motor_control_mode == MOTOR_CONTROL_MANUAL)
    {
      uint8_t motor_no = data[1];
      uint8_t speed = data[2];
      cmd_motor(motor_no, speed);
    }
    break;
  }
  case SERVO_WRITE:
  {
    uint8_t servo_no = data[1];
    uint8_t angle = data[2];
    uint8_t speed = data[3];
    cmd_servo(servo_no, angle, speed);
    break;
  }
  case MOTOR_CONTROL_MODE:
  {
    uint8_t state = data[0];
    if (data[1] == 0)
    {
      motor_control_mode = MOTOR_CONTROL_AUTO;
      Serial.print("MOTOR_CONTROL_AUTO");
    }
    else
    {
      motor_control_mode = MOTOR_CONTROL_MANUAL;
      Serial.print("MOTOR_CONTROL_MANUAL");
    }
    break;
  }
  }
}

// UUID Converter
void strUUID2Bytes(String strUUID, uint8_t binUUID[])
{
  String hexString = String(strUUID);
  hexString.replace("-", "");

  for (int i = 16; i != 0; i--)
  {
    binUUID[i - 1] = hex2c(hexString[(16 - i) * 2], hexString[((16 - i) * 2) + 1]);
  }
}

char hex2c(char c1, char c2)
{
  return (nibble2c(c1) << 4) + nibble2c(c2);
}

char nibble2c(char c)
{
  if ((c >= '0') && (c <= '9'))
    return c - '0';
  if ((c >= 'A') && (c <= 'F'))
    return c + 10 - 'A';
  if ((c >= 'a') && (c <= 'f'))
    return c + 10 - 'a';
  return 0;
}