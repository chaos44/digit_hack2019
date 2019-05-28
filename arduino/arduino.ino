#include <bluefruit.h>
#include <Wire.h>

#define LED1 7

/**
   2WD BLE R/C Car for LINE Things development board with TI DRV8830 motor driver
*/

// Device Name: Maximum 30 bytes
#define DEVICE_NAME "4WD R/C Car DRV8830 - nRF52"

// User service UUID: Change this to your generated service UUID
#define USER_SERVICE_UUID "3fd1e37b-83a3-4691-8a70-dc42cd486ef7"            //置き換えること

#define WRITE_CHARACTERISTIC_UUID "E9062E71-9E62-4BC6-B0D3-35CDCD9B027B"    //置き換えること
#define NOTIFY_CHARACTERISTIC_UUID "62FBD229-6EDD-4D1A-B554-5C4E1BB29169"   //置き換えること

// PSDI Service UUID: Fixed value for Developer Trial
#define PSDI_SERVICE_UUID "E625601E-9E55-4597-A598-76018A0D293D"
#define PSDI_CHARACTERISTIC_UUID "26E2B12B-85F0-4F3F-9FDD-91D114270E6E"

uint8_t userServiceUUID[16];
uint8_t writeCharacteristicUUID[16];
uint8_t notifyCharacteristicUUID[16];

uint8_t psdiServiceUUID[16];
uint8_t psdiCharacteristicUUID[16];

BLEService userService;
BLECharacteristic writeCharacteristic;
BLECharacteristic notifyCharacteristic;

BLEService psdiService;
BLECharacteristic psdiCharacteristic;

static void cmd_servo(uint8_t servo_no, uint8_t angle, uint8_t speed);
static void cmd_motor(uint8_t motor_no, int8_t speed);

void setup() {
  Serial.begin(115200);
  device_init();

  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);


  setupServices();
  startAdvertising();
  Serial.println("Ready to Connect");
}

void loop() {
  //  i2c_scanner();

  //  cmd_motor(0, 100);
  //  delay(2000);
  //  cmd_motor(0, -100);
  //  delay(2000);
  //  cmd_motor(0, 0);
  //  delay(2000);
  //  cmd_motor(1, 100);
  //  delay(2000);
  //  cmd_motor(1, -100);
  //  delay(2000);
  //  cmd_motor(1, 0);
  //  delay(2000);
  //
  //  cmd_servo(1, 30, 60);
  //  delay(2000);
  //  cmd_servo(1, 150, 120);
  //  delay(2000);

  delay(500);
}


#define BOX1_LOCK     0x00
#define BOX2_LOCK     0x01
#define WARNING_LIGHT 0x02
#define TEST_LED      0xff

#define LOCK    1
#define UNLOCK  0

#define LOCKED_ANGLE      20
#define UNLOCKED_ANGLE    90

void WriteCallback(uint16_t conn_handle, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  switch (data[0]) {
    case BOX1_LOCK: {
        char lock_state = (char)data[1];
        if (lock_state == UNLOCK) {
          cmd_servo(1, UNLOCKED_ANGLE, 90);
        } else {
          cmd_servo(1, LOCKED_ANGLE, 90);
        }
        break;
      }
    case BOX2_LOCK: {
        char lock_state = (char)data[1];
        if (lock_state == UNLOCK) {
          cmd_servo(2, UNLOCKED_ANGLE, 90);
        } else {
          cmd_servo(2, LOCKED_ANGLE, 90);
        }
        break;
      }
    case WARNING_LIGHT: {
        char duty = (char)data[1];
        if (duty == 0) {
          //              cmd_kaiten_lamp(0, 0);
        } else {
          //              cmd_kaiten_lamp(duty, 1);
        }
        break;
      }
    case TEST_LED: {
        char led_state = (char)data[1];
        if (led_state == 0) {
          digitalWrite(LED1, LOW);
        } else {
          digitalWrite(LED1, HIGH);
        }
        break;
      }
  }
}

void setupServices(void) {
  // Convert String UUID to raw UUID bytes
  strUUID2Bytes(USER_SERVICE_UUID, userServiceUUID);
  strUUID2Bytes(WRITE_CHARACTERISTIC_UUID, writeCharacteristicUUID);
  strUUID2Bytes(NOTIFY_CHARACTERISTIC_UUID, notifyCharacteristicUUID);
  strUUID2Bytes(PSDI_SERVICE_UUID, psdiServiceUUID);
  strUUID2Bytes(PSDI_CHARACTERISTIC_UUID, psdiCharacteristicUUID);

  // Setup User Service
  userService = BLEService(userServiceUUID);
  userService.begin();

  writeCharacteristic = BLECharacteristic(writeCharacteristicUUID);
  writeCharacteristic.setProperties(CHR_PROPS_WRITE);
  writeCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  writeCharacteristic.setWriteCallback(WriteCallback);
  writeCharacteristic.setFixedLen(3);
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
  uint32_t deviceAddr[] = { NRF_FICR->DEVICEADDR[0], NRF_FICR->DEVICEADDR[1] };
  psdiCharacteristic.write(deviceAddr, sizeof(deviceAddr));
}

void startAdvertising(void) {
  // Start Advertising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(userService);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);
}

// UUID Converter
void strUUID2Bytes(String strUUID, uint8_t binUUID[]) {
  String hexString = String(strUUID);
  hexString.replace("-", "");

  for (int i = 16; i != 0 ; i--) {
    binUUID[i - 1] = hex2c(hexString[(16 - i) * 2], hexString[((16 - i) * 2) + 1]);
  }
}

char hex2c(char c1, char c2) {
  return (nibble2c(c1) << 4) + nibble2c(c2);
}

char nibble2c(char c) {
  if ((c >= '0') && (c <= '9'))
    return c - '0';
  if ((c >= 'A') && (c <= 'F'))
    return c + 10 - 'A';
  if ((c >= 'a') && (c <= 'f'))
    return c + 10 - 'a';
  return 0;
}



void device_init() {
  Wire.begin();

  pinMode(LED1, OUTPUT);
}



static const int motor_addr = 0x26;
static void cmd_motor(uint8_t motor_no, int8_t speed) {
  uint8_t motor_cmd[3];
  Serial.println("cmd_motor");

  motor_cmd[0] = 0x01;
  motor_cmd[1] = motor_no;
  motor_cmd[2] = *(uint8_t *)(&speed);

  Wire.beginTransmission(motor_addr);
  for (int i = 0; i < sizeof(motor_cmd) / sizeof(motor_cmd[0]); i++) {
    Serial.print(i);
    Serial.print(":");
    Serial.println(motor_cmd[i], HEX);

    Wire.write(motor_cmd[i]);
  }
  Wire.endTransmission();
}

static void cmd_servo(uint8_t servo_no, uint8_t angle, uint8_t speed) {
  char servo_cmd[4];
  servo_cmd[0] = 0x02;
  servo_cmd[1] = servo_no;
  servo_cmd[2] = angle;
  servo_cmd[3] = speed;
  Wire.beginTransmission(motor_addr);
  for (int i = 0; i < sizeof(servo_cmd) / sizeof(servo_cmd[0]); i++) {
    Wire.write(servo_cmd[i]);
  }
  Wire.endTransmission();
}

static void i2c_scanner()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}
