#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pigpio.h>

#include "ADXL375.h"
#include "I2C_Device.h"
#include "Vector.h"

#define INDICATOR_LED 18

typedef struct
{
    Vec3f accelerationValue;
    float time;
} DataPoint;

DataPoint* data;
uint32_t length = 0;

float initializationTime = 0.0f;

float getTime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return ((float)now.tv_sec) + (float)(1.0E-9 * now.tv_nsec) - initializationTime;
}

void addData(DataPoint v)
{
    length++;
    //data = realloc(data, sizeof(DataPoint) * length);
    data[length - 1] = v;
}

bool duplicateDataPoint(DataPoint dp1, DataPoint dp2)
{
    if(dp1.accelerationValue.x == dp2.accelerationValue.x &&
       dp1.accelerationValue.y == dp2.accelerationValue.y &&
       dp1.accelerationValue.z == dp2.accelerationValue.z)
       return true;

    return false;
}

void clearData()
{

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

    initializationTime = getTime();

    printf("%s\n", "Initialization Complete");
    printf("%s\n", "Telemetry Program on Standby.");

    data = malloc(sizeof(Vec3f) * 2E6);
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

            for(uint16_t i = 0; i < 1000; i++)
            {
                //printf("X acceleration: %.3f\n", getAccelerationX());
                //printAccelerationData();
                Vec3f v = ADXL375_read();

                DataPoint dp;
                //dp.accelerationValue = v;
                dp.time = getTime();

                /*float delta = 0.0f;
                if(length > 0)
                {
                    delta = dp.time - data[length - 1].time;
                }
                float rate = 1.0f / delta;*/
                
                addData(dp);
            }

            for(uint16_t i = 0; i < length; i++)
            {
                printf("Time: %.3fs, Rate %.3f Hz, X: %.3f, Y: %.3f, Z: %.3f\n", data[i].time, 1.0f / (data[i].time - data[i - 1].time), data[i].accelerationValue.x, data[i].accelerationValue.y, data[i].accelerationValue.z);
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