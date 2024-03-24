#ifndef __I2C_H
#define __I2C_H
#include "main.h"
#define MAX5980GTJ_DEFAULT_ADDRESS (uint8_t)0x5E
#define MAX5980GTJ_STATUS_REGISTER (uint8_t)0x0C
#define I2C_REQUEST_WRITE (uint8_t)0x00
#define I2C_REQUEST_READ (uint8_t)0x01
void Handle_I2C_Master(uint8_t register_address, uint8_t* data, uint8_t data_size);
#endif
