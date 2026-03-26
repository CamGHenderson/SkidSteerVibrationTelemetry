#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
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

pthread_t recordingThread;

DataPoint* data;
uint32_t length = 0;
bool recording = false;
char workingFileName[50];

float initializationTime = 0.0f;
float recordStartTime;

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
    free(data);
    data = malloc(sizeof(DataPoint) * 2E6);
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

    data = malloc(sizeof(DataPoint) * 2E6);
}

void terminate()
{
    printf("%s\n", "Terminating Telemetry Program...");
    
    terminateDevices();
    
    gpioWrite(INDICATOR_LED, 0);
    gpioTerminate();
    printf("%s\n", "Telemetry Program Terminated.");
}

void writeDataToFile()
{
    FILE* file = fopen(workingFileName, "w");
    for(uint16_t i = 0; i < length; i++)
    {
        printf("%.3f, %.3f, %.3f, %.3f\n", data[i].time, data[i].accelerationValue.x, data[i].accelerationValue.y, data[i].accelerationValue.z);
    }

    fclose(file);
    printf("%s %s\n", "Saved file: ", workingFileName);

    clearData();
}

void* record()
{
    recordStartTime = getTime();
    recording = true;
    while(recording)
    {
        Vec3f v = ADXL375_read();

        DataPoint dp;
        dp.accelerationValue = v;
        dp.time = getTime();

        addData(dp);
    } 

    pthread_exit(NULL);
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
            if(!recording)
            {
                printf("%s", "Enter file name: ");
                scanf("%s", &workingFileName);
                printf("%s\n", "recording...");
                
                pthread_create(&recordingThread, NULL, record, NULL);
            }
            else
            {
                printf("%s\n", "already busy recording!");
            }
        }
        else if(!strcmp(command, "stop-recording"))
        {
            recording = false;
            printf("%s\n", "stopped recording.");
        }
        else if(!strcmp(command, "exit"))
        {
            if(recording)
            {
                printf("busy recording! cannot exit\n");
            }
            else
            {
                break;
            }
        }
        else
        {
            printf("%s\n", "unknown command try again.");
        }
    }
    
    terminate();

    return 0;
}