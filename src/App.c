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

DataPoint* data = NULL;
uint32_t length = 0;

float initializationTime = 0.0f;

float getTime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return ((float)now.tv_sec) + (float)(1.0E-9*now.tv_nsec) - initializationTime;
}

void addData(DataPoint v)
{
    length++;
    data = realloc(data, sizeof(DataPoint) * length);
    data[length - 1] = v;
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
                Vec3f v = ADXL375_read();

                DataPoint dp;
                dp.accelerationValue = v;
                dp.time = getTime();

                float delta = 0.0f;
                if(length > 0)
                {
                    float delta = dp.time - data[length].time;
                    //printf("%f %f %f\n", delta, dp.time, data[length].time);
                }
                float rate = 1.0f / delta;

                //printf("Time: %.3fs, Delta: %.3fs, Rate: %.3fHz, X: %.3f, Y: %.3f, Z: %.3f\n", dp.time, delta, rate, v.x, v.y, v.z);
                
                addData(dp);
                usleep(100 * 1000);
            }

            for(uint16_t i = 0; i < length; i++)
            {
                printf("Time: %.3f, X: %.3f, Y: %.3f, Z: %.3f\n", data[i].time, ata[i].accelerationValue.x, data[i].accelerationValue.y, data[i].accelerationValue.z);
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