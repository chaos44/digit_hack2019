#include <bluefruit.h>
#include <Wire.h>

static const int motor_addr = 0x26;

void device_init() {
  Wire.begin();
}
 
void cmd_motor(uint8_t motor_no, int8_t speed) {
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

void cmd_servo(uint8_t servo_no, uint8_t angle, uint8_t speed) {
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

void i2c_scanner()
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
