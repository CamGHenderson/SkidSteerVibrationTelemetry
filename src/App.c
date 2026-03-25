#include <stdio.h>
#include <stdint.h>
#include <pigpio.h>
#include <unistd.h>

int32_t main()
{
    printf("%s\n", "Initializing Telemetry Program");
    if(gpioInitialise() < 0) 
    {                                                                                                            
        printf("%s\n", "gpio failed to initialize");                                                         
        exit(-1);
    }                                                                                    
    
    gpioSetMode(18, PI_OUTPUT);
    gpioWrite(18, 1);
    sleep(3);
    gpioWrite(18, 0);

    printf("%s\n", "Terminating Telemetry Program");

    return 0;
}