#ifndef RASPI_COM_H_
#define RASPI_COM_H_

#include <bluefruit.h>

void ExecUartCmd();

void ExecUartCmdCallback(uint8_t motor, int speed);

#endif