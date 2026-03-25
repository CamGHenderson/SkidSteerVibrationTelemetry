#include "ADXL375.h"

#include <stdint.h>
#include <pigpio.h>
#include <stdio.h>

#include "I2C_Device.h"

#define ADXL375_MG2G_MULTIPLIER 0.049f

#define INT_ENABLE 0x2C
#define THRESH_SHOCK 0x1D
#define DUR 0x21
#define LATENT 0x22
#define WINDOW 0x23
#define SHOCK_AXES 0x2A

#define POWER_CTL 0x2D
#define DATA_FORMAT 0x31
#define BW_RATE 0x2C
#define DATAX0 0x32
#define DATAX1 0x33
#define DATAY0 0x34
#define DATAY1 0x35
#define DATAZ0 0x36
#define DATAZ1 0x37

int8_t adxl375I2CAddresses[2] = { 0x53, 0x29 };
int32_t adxlI2CHandles[2] = { 0, 0 };

void initializeADXL375()
{
    for(uint16_t i = 0; i < 1; i++)
    {
        int32_t handle = i2cOpen(I2C_BUS, adxl375I2CAddresses[i], 0);
        if(handle >= 0)
        {
            printf("%s %i\n", "i2c opened on address: ", adxl375I2CAddresses[i]);
        }

        adxl375I2CAddresses[i] = handle;

        // bullshit config from stolen adafruit library
        i2cWriteByteData(handle, INT_ENABLE, 0);
        i2cWriteByteData(handle, THRESH_SHOCK, 20);
        i2cWriteByteData(handle, DUR, 50);
        i2cWriteByteData(handle, LATENT, 0);
        i2cWriteByteData(handle, WINDOW, 0);
        i2cWriteByteData(handle, SHOCK_AXES, 0x7);
        i2cWriteByteData(handle, DATA_FORMAT, 0b00001011);

        // turn off sleep mode for adxl
        i2cWriteByteData(handle, POWER_CTL, 0x08);

        // set sample rate to 25hz
        i2cWriteByteData(handle, BW_RATE, 8);

        uint8_t bwRateData = i2cReadByteData(handle, BW_RATE);
        printf("%i -> ", bwRateData);
        
        printBits(bwRateData);
        printf("\n");
    }
}

void terminateADXL375()
{
    for(uint16_t i = 0; i < 1; i++)
    {
        // put adxl into sleep mode
        i2cWriteByteData(adxlI2CHandles[i], POWER_CTL, 0x04);
        i2cClose(adxlI2CHandles[i]);

        printf("%s\n", "adxl375 in sleep mode.");
    }
}

void printBits(uint8_t num)
{
    uint16_t maxPow = 1 << (8 - 1);
    for(int bit = 0; bit < 8; bit++)
    {
        printf("%u ", num & maxPow ? 1 : 0);
        num = num << 1;
    }
}

float printAccelerationData()
{
    uint8_t data[6];
    i2cReadI2CBlockData(adxl375I2CAddresses[0], DATAX0, data, 6);
    float xa = ((float)(data[0] | data[1] << 8)) * ADXL375_MG2G_MULTIPLIER;
    printf("X acceleration(g's) %.3f\n", xa);

    //printf("Y0 %i, ", data[2]);
    //printf("Y1 %i, ", data[3]);


    //printf("Z0 %i, ", data[4]);
    //printf("Z1 %i\n", data[5]);

    return 0.0f;
}