#pragma once
#include <stdint.h>

#define I2C_BUS 1

typedef struct
{
    int8_t address;
    int32_t handle;

} I2C_Device;

void printConnectedI2CDevices();