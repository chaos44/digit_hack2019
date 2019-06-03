#ifndef UUID_CONFIG_H_
#define UUID_CONFIG_H_

#include <bluefruit.h>

void configFileInit();
void configFileWrite(uint8_t binUuid[]);
void configFileRead(uint8_t binUuid[]);
int configFileExist();
int compareUuid(uint8_t uuid1[], uint8_t uuid2[]);

#endif