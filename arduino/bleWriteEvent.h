#ifndef BLE_WRITE_EVENT_H_
#define BLE_WRITE_EVENT_H_

#include <bluefruit.h>

extern volatile int g_flag_update_advertiseuuid;
extern volatile int g_flag_error_advertiseuuid;

void bleWriteEvent(uint16_t conn_handle, BLECharacteristic *chr, uint8_t *data, uint16_t len);
void ble_write_loop();
void board_user_write();

#endif