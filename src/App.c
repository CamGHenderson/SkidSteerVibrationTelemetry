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

#define STANDBY_LED 18
#define RECORDING_LED 17

typedef struct
{
    Vec3f accelerationValue[ADXL375_COUNT];
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
    bool test = true;
    for(uint16_t i = 0; i < ADXL375_COUNT; i++)
    {
        if(dp1.accelerationValue[i].x != dp2.accelerationValue[i].x ||
            dp1.accelerationValue[i].y != dp2.accelerationValue[i].y ||
            dp1.accelerationValue[i].z != dp2.accelerationValue[i].z)
        {
            test = false;
            break;
        }
    }

    return test;
}

void clearData()
{
    free(data);
    data = malloc(sizeof(DataPoint) * 2E6);
    length = 0;
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

    gpioSetMode(STANDBY_LED, PI_OUTPUT);
    gpioSetMode(RECORDING_LED, PI_OUTPUT);

    gpioWrite(STANDBY_LED, 1);

    initializationTime = getTime();

    printf("%s\n", "Initialization Complete");
    printf("%s\n", "Telemetry Program on Standby.");

    data = malloc(sizeof(DataPoint) * 2E6);

    sleep(1);

    printf("%s\n", "ready to calibrate...");
    printf("%s\n", "type \"calibrate\" when ready");
}

void terminate()
{
    printf("%s\n", "Terminating Telemetry Program...");
    
    terminateDevices();
    
    gpioWrite(STANDBY_LED, 0);
    gpioTerminate();
    printf("%s\n", "Telemetry Program Terminated.");
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void writeDataToFile()
{
    FILE* file = fopen(concat("data/", workingFileName), "w");
    for(uint16_t i = 0; i < length; i++)
    {
        fprintf(file, "%.5f, ", data[i].time);

        for(uint16_t s = 0; s < ADXL375_COUNT; s++)
            fprintf(file, "%.5f, %.5f, %.5f, ", data[i].accelerationValue[s].x, data[i].accelerationValue[s].y, data[i].accelerationValue[s].z);

        fprintf(file, "\n");
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
        DataPoint dp;
        dp.time = getTime() - recordStartTime;
        for(uint16_t i = 0; i < ADXL375_COUNT; i++)
        {
            Vec3f v = ADXL375_read(i);
            dp.accelerationValue[i] = v;
        }

        addData(dp);
    } 

    writeDataToFile();
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
        else if(!strcmp(command, "print"))
        {
            while(true)
            {
                for(uint16_t s = 0; s < ADXL375_COUNT; s++)
                    Vec3f v = ADXL375_read(s);
                    printf("| %.5f, %.5f, %.5f |", v.x, v.y, v.z);
                
                printf("\n");
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
        else if(!strcmp(command, "calibrate"))
        {
            if(!recording)
                ADXL375_calibrate();
            else
                printf("busy recording! cannot calibrate\n");
        }
        else
        {
            printf("%s\n", "unknown command try again.");
        }

        if(recording)
        {
            gpioWrite(RECORDING_LED, 1);
        }
        else
        {
            gpioWrite(RECORDING_LED, 0);
        }
    }
    
    terminate();

    return 0;
}