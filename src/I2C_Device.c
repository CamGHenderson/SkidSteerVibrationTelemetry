#include "I2C_Device.h"

#include <stdlib.h>
#include <stdio.h>

void printConnectedI2CDevices()
{
    printf("%s\n", "Commencing I2C Search...");
    system("i2cdetect -y 1");
    printf("%s\n", "I2C Search Complete.");
}