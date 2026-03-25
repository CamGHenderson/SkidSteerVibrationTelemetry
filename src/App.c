#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <pigpio.h>
#include <unistd.h>

#include "ADXL375.h"

typedef struct
{
    int8_t address;
    int32_t handle;

} I2C_Device;

void printBits(uint8_t num)
{
    uint16_t maxPow = 1 << (8 - 1);
    for(int bit = 0; bit < 8; bit++)
    {
        printf("%u ", num & maxPow ? 1 : 0);
        num = num << 1;
    }
}

void printConnectedI2CDevices()
{
    printf("%s\n", "Commencing I2C Search...");
    system("i2cdetect -y 1");
    printf("%s\n", "I2C Search Complete.");
}

void initializeMPU6050()
{

}

void initializeI2CDevices()
{
    initializeADXL375();
    //initializeMPU6050();
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

int32_t main()
{
    initialize();
    
    while(true)
    {
        char command[50];
        printf("\n>> ");
        scanf("%s", &command);

        if(!strcmp(command, "record"))
        {
            printf("%s\n", "recording...");

            for(uint16_t i = 0; i < 100; i++)
            {
                //printf("X acceleration: %.3f\n", getAccelerationX());
                //printAccelerationData();
                usleep(100 * 1000);
            }
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