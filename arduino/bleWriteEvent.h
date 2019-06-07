#ifndef BLE_WRITE_EVENT_H_
#define BLE_WRITE_EVENT_H_

#include <bluefruit.h>

extern volatile int g_flag_update_advertiseuuid;
extern volatile int g_flag_error_advertiseuuid;

void writeEvent(uint8_t *data);
void ble_write_loop();

#endif