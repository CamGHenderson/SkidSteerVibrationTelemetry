#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pigpio.h>
#include <unistd.h>

#define INDICATOR_LED 18

void printConnectedI2CDevices()
{
    printf("%s\n", "Commencing I2C Search...");
    for(int8_t i = 0; i < 127; i++)
    {
        if(i2cOpen(i, 0) >= 0)
        {
            printf("%s %i\n", "I2C Device at Address: ", i);
        }
    }

    printf("%s\n", "I2C Search Complete.");
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

    gpioSetMode(INDICATOR_LED, PI_OUTPUT);
    gpioWrite(INDICATOR_LED, 1);

    printf("%s\n", "Initialization Complete");
    printf("%s\n", "Telemetry Program on Standby.");
}

void terminate()
{
    printf("%s\n", "Terminating Telemetry Program...");
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
        scanf("%s", &command);

        if(!strcmp(command, "record"))
        {
            printf("%s\n", "recording...");
        }
        else if(!strcmp(command, "stop-recording"))
        {
            printf("%s\n", "stoped recording.");
        }
        else if(!strcmp(command, "exit"))
        {
            break;
        }
    }
    
    terminate();

    return 0;
}