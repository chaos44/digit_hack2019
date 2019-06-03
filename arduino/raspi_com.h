#ifndef RASPI_COM_H_
#define RASPI_COM_H_

#include <bluefruit.h>

extern char Buffer[100];

boolean readSerial();
int strToInt(char *str);

#endif