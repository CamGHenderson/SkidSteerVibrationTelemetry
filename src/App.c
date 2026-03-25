#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <pigpio.h>
#include <unistd.h>

#define INDICATOR_LED 18
#define I2C_BUS 1

#define POWER_CTL 0x2D
#define DATA_FORMAT 0x31
#define BW_RATE 0x2C
#define DATAX0 0x32
#define DATAX1 0x33
#define DATAY0 0x34
#define DATAY1 0x35
#define DATAZ0 0x36
#define DATAZ1 0x37

int8_t adxl375I2CAddresses[2] = { 53, 29 };
int32_t adxlI2CHandles[2] = { 0, 0 };

typedef struct
{
    int8_t address;
    int32_t handle;

} I2C_Device;

void printConnectedI2CDevices()
{
    printf("%s\n", "Commencing I2C Search...");
    system("i2cdetect -y 1");
    printf("%s\n", "I2C Search Complete.");
}

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

        // turn off sleep mode for adxl
        i2cWriteByteData(handle, POWER_CTL, 0x08);

        // set sample rate to 3200hz
        i2cWriteByteData(handle, BW_RATE, 0x0F);
    }
}

void initializeMPU6050()
{

}

void initializeI2CDevices()
{
    initializeADXL375();
    //initializeMPU6050();
}

void terminateADXL375()
{
    for(uint16_t i = 0; i < 1; i++)
    {
        // put adxl into sleep mode
        i2cWriteByteData(adxlI2CHandles[i], POWER_CTL, 0x04);
        i2cClose(adxlI2CHandles[i]);
    }
}

void terminateI2CDevices()
{
    terminateADXL375();
}

void initialize()
{
    printf("%s\n", "Initializing Telemetry Program...");
    if(gpioInitialise() < 0) 
    {                                                                                                            
        printf("%s\n", "gpio failed to initialize!");                                                         
        exit(-1);
    }                                                                                    
    
    printConnectedI2CDevices();
    initializeADXL375();

    gpioSetMode(INDICATOR_LED, PI_OUTPUT);
    gpioWrite(INDICATOR_LED, 1);

    printf("%s\n", "Initialization Complete");
    printf("%s\n", "Telemetry Program on Standby.");
}

void terminate()
{
    printf("%s\n", "Terminating Telemetry Program...");
    
    terminateI2CDevices();
    
    gpioWrite(INDICATOR_LED, 0);
    gpioTerminate();
    printf("%s\n", "Telemetry Program Terminated.");
}

float getAccelerationX()
{
    int8_t x0 = i2cReadByteData(adxlI2CHandles[0], DATAX0);
    int8_t x1 = i2cReadByteData(adxlI2CHandles[0], DATAX1);

    printf("X0 %i\n", x0);
    printf("X1 %i\n", x1);

    int16_t x16 = (x1 << 8 | x0);
    if (x16 & (1 << 15))
    {
        x16 = x16 - (1 << 16);
    }

    return (float)(x16 / pow(2, 15)) * 16.0f * 9.81f;
}

int32_t main()
{
    initialize();
    
    while(true)
    {
        char command[50];
        scanf("%s", &command);

        if(!strcmp(command, "record"))
        {
            printf("%s\n", "recording...");

            printf("X acceleration: %.3f\n", getAccelerationX());
        }
        else if(!strcmp(command, "stop-recording"))
        {
            printf("%s\n", "stopped recording.");
        }
        else if(!strcmp(command, "exit"))
        {
            break;
        }
    }
    
    terminate();

    return 0;
}