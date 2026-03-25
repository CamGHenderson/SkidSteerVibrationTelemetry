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

int8_t adxl375I2CAddresses[2] = { 0x53, 0x29 };
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
        //i2cWriteByteData(handle, BW_RATE, 10);
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

        printf("%s\n", "adxl375 in sleep mode.");
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

void printBits(uint8_t num)
{
   for(int bit = 0; bit < (sizeof(uint8_t) * 8); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
}

float printAccelerationData()
{
    uint8_t data[6];
    i2cReadI2CBlockData(adxl375I2CAddresses[0], DATAX0, data, 6);


    printf("|");
    printBits(data[1]);
    printf(" ");
    printBits(data[0]);
    printf(" | \n");

    printf("X0 %i, ", data[0]);
    printf("X1 %i, ", data[1]);


    //
    //printf("X16 %i\n", x16);

    //printf("Y0 %i, ", data[2]);
    //printf("Y1 %i, ", data[3]);


    //printf("Z0 %i, ", data[4]);
    //printf("Z1 %i\n", data[5]);

    return 0.0f;
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
                printAccelerationData();
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