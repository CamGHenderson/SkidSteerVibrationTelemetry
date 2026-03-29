#include "ADXL375.h"

#include <stdint.h>
#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>

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

#define OFFSETX 0x1E
#define OFFSETY 0x1F
#define OFFSETZ 0x20

int8_t adxl375I2CAddresses[ADXL375_COUNT] = { 0x53, 0x1D, 0x70 };
int32_t adxlI2CHandles[ADXL375_COUNT] = { 0, 0, 0 };
Vec3f adxl375Offsets[ADXL375_COUNT];

void printBits(uint8_t num)
{
    uint16_t maxPow = 1 << (8 - 1);
    for(int bit = 0; bit < 8; bit++)
    {
        printf("%u ", num & maxPow ? 1 : 0);
        num = num << 1;
    }
}

void ADXL375_calibrate()
{
    printf("calibrating all adxl375...\n");
    
    for(uint16_t s = 0; s < ADXL375_COUNT; s++)
    {
        Vec3f ave;
        for(uint16_t i = 0; i < 2000; i++)
        {
            Vec3f v = ADXL375_read(s);
            ave.x += v.x;
            ave.y += v.y;
            ave.z += v.z;

            usleep(1000);
        }

        ave.x = ave.x / 2000.0f;
        ave.y = ave.y / 2000.0f;
        ave.z = ave.z / 2000.0f;

        adxl375Offsets[s] = ave;
    }

    printf("calibrated all adxl375!");
}

void ADXL375_initialize()
{
    for(uint16_t i = 0; i < ADXL375_COUNT; i++)
    {
        int32_t handle = i2cOpen(I2C_BUS, adxl375I2CAddresses[i], 0);
        if(handle >= 0)
        {
            printf("%s %i\n", "i2c opened on address: ", adxl375I2CAddresses[i]);
        }

        printf("Handle: %i, ", handle);

        adxlI2CHandles[i] = handle;

        // bullshit config from stolen adafruit library
        i2cWriteByteData(handle, INT_ENABLE, 0);
        i2cWriteByteData(handle, THRESH_SHOCK, 20);
        i2cWriteByteData(handle, DUR, 50);
        i2cWriteByteData(handle, LATENT, 0);
        i2cWriteByteData(handle, WINDOW, 0);
        i2cWriteByteData(handle, SHOCK_AXES, 0x7);
        i2cWriteByteData(handle, DATA_FORMAT, 0b00001011);

        // turn off sleep mode for adxl
        i2cWriteByteData(handle, POWER_CTL, 15);

        // set sample rate to 800hz
        i2cWriteByteData(handle, BW_RATE, 14);

        adxl375Offsets[i].x = 0.0f;
        adxl375Offsets[i].y = 0.0f;
        adxl375Offsets[i].z = 0.0f;

        uint8_t bwRateData = i2cReadByteData(handle, BW_RATE);
        printf("%i -> ", bwRateData);
        
        printBits(bwRateData);
        printf("\n");
    }
}

void ADXL375_terminate()
{
    for(uint16_t i = 0; i < ADXL375_COUNT; i++)
    {
        // put adxl into sleep mode
        i2cWriteByteData(adxlI2CHandles[i], POWER_CTL, 0x04);
        i2cClose(adxlI2CHandles[i]);

        printf("%s\n", "adxl375 in sleep mode.");
    }
}

Vec3f ADXL375_read(uint8_t sensorIndex)
{
    uint8_t data[6];
    i2cReadI2CBlockData(adxlI2CHandles[sensorIndex], DATAX0, data, 6);

    int16_t x16 = (data[0] | data[1] << 8);
    int16_t y16 = (data[2] | data[3] << 8);
    int16_t z16 = (data[4] | data[5] << 8);

    Vec3f v;
    v.x = (((float)x16) * ADXL375_MG2G_MULTIPLIER) - adxl375Offsets[sensorIndex].x;
    v.y = (((float)y16) * ADXL375_MG2G_MULTIPLIER) - adxl375Offsets[sensorIndex].y;
    v.z = (((float)z16)) * ADXL375_MG2G_MULTIPLIER - adxl375Offsets[sensorIndex].z;
 
    return v;
}