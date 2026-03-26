#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <pigpio.h>
#include <unistd.h>

#include "ADXL375.h"
#include "I2C_Device.h"
#include "Vector.h"

#define INDICATOR_LED 18

Vec3f* data = NULL;
uint32_t length = 0;

void addData(Vec3f v)
{
    length++;
    data = realloc(data, sizeof(Vec3f) * length);
    data[length - 1] = v;
}

void initializeMPU6050()
{

}

void initializeDevices()
{
    ADXL375_initialize();
    //initializeMPU6050();
}

void terminateDevices()
{
    ADXL375_terminate();
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
    initializeDevices();

    gpioSetMode(INDICATOR_LED, PI_OUTPUT);
    gpioWrite(INDICATOR_LED, 1);

    printf("%s\n", "Initialization Complete");
    printf("%s\n", "Telemetry Program on Standby.");
}

void terminate()
{
    printf("%s\n", "Terminating Telemetry Program...");
    
    terminateDevices();
    
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
                addData(ADXL375_read());
                usleep(100 * 1000);
            }

            for(uint16_t i = 0; i < length; i++)
            {
                printf("X: %.3f, Y: %.3f, Z: %.3f\n", data[i].x, data[i].y, data[i].z);
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
        else
        {
            printf("%s\n", "unknown command try again.");
        }
    }
    
    terminate();

    return 0;
}