#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/Vector.h"

#define DOWNLOADED_FILE_DIRECTORY "../downloaded/ConcreatePlateSidePlateAndCaseMountTest1.csv"
#define OUTPUT_FILE_DIRECTORY "FilteredOutput/ConcreatePlateSidePlateAndCaseMountTest1.csv"
#define TIME_INCREMENT 0.1f

typedef struct
{
    float time;
    Vec3f accelerationValue[2];
} DataPoint;

Vec3f addVec3f(Vec3f v1, Vec3f v2)
{
    Vec3f v3;
    v3.x = v1.x + v2.x;
    v3.y = v1.y + v2.y;
    v3.z = v1.z + v2.z;
    return v3;
}

Vec3f divVec3f(Vec3f v, float n)
{  
    Vec3f v2;
    v2.x = v.x / n;
    v2.y = v.y / n;
    v2.z = v.z / n;
    return v2;
}

Vec3f vec3f()
{
    Vec3f v;
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 0.0f;
    return v;
}

DataPoint resetDP()
{
    DataPoint dp;
    dp.time = 0.0f;
    dp.accelerationValue[0] = vec3f();
    dp.accelerationValue[1] = vec3f();
    return dp;
}

Vec3f maxVec3f(Vec3f v1, Vec3f v2)
{
    Vec3f v3;
    v3.x = (abs(v1.x) > abs(v2.x)) ? v1.x : v2.x;  
    v3.y = (abs(v1.y) > abs(v2.y)) ? v1.y : v2.y;  
    v3.z = (abs(v1.z) > abs(v2.z)) ? v1.z : v2.z;
    return v3;
}

DataPoint maxDataPoint(DataPoint d1, DataPoint d2)
{
    DataPoint d3;
    d3.accelerationValue[0] = maxVec3f(d1.accelerationValue[0], d2.accelerationValue[0]);
    d3.accelerationValue[1] = maxVec3f(d1.accelerationValue[1], d2.accelerationValue[1]);
    return d3;
}

int32_t main()
{
    printf("Kill yourself\n");

    FILE* file = fopen(DOWNLOADED_FILE_DIRECTORY, "r");

    float time = 0.0f;
    float lastTime = 0.0f;
    char fileBuffer[200];
    int16_t pointIndex = 0;

    DataPoint points[10000];
    DataPoint dpAve;
    DataPoint dpMax = resetDP();
    uint16_t i = 0;
    if(file != NULL) {
        while(fgets(fileBuffer, 200, file))
        {
            DataPoint dp;
            char* value = strtok(fileBuffer, ",");
            dp.time = (float)atof(value);
            
            value = strtok(NULL, ",");
            dp.accelerationValue[0].x = (float)atof(value);
            
            value = strtok(NULL, ",");
            dp.accelerationValue[0].y = (float)atof(value);
            
            value = strtok(NULL, ",");
            dp.accelerationValue[0].z = (float)atof(value);
            
            value = strtok(NULL, ",");
            dp.accelerationValue[1].x = (float)atof(value);
            
            value = strtok(NULL, ",");
            dp.accelerationValue[1].y = (float)atof(value);

            value = strtok(NULL, ",");
            dp.accelerationValue[1].z = (float)atof(value);

            if(dp.time < points[pointIndex].time && pointIndex > 0)
            {
                points[pointIndex] = dp;
                pointIndex++;

                break;
            }

            /*if(pointIndex >= 30000)
            {
                break;
            }*/

            points[pointIndex] = dp;
            pointIndex++;

            /*
            i++;

            time = dp.time;
            dpAve.accelerationValue[0] = addVec3f(dpAve.accelerationValue[0], dp.accelerationValue[0]);
            dpAve.accelerationValue[1] = addVec3f(dpAve.accelerationValue[1], dp.accelerationValue[1]);

            dpMax = maxDataPoint(dp, dpMax);

            if(time - lastTime >= TIME_INCREMENT)
            {
                dpAve.accelerationValue[0] = divVec3f(dpAve.accelerationValue[0], (float)i);
                dpAve.accelerationValue[1] = divVec3f(dpAve.accelerationValue[1], (float)i);                
                i = 0;
                
                dpAve.time = time;
                dpMax.time = time;
                lastTime = time;
                
                pointIndex++;

                dpAve = resetDP();
                dpMax = resetDP();
            }*/
        }

        fclose(file);

        file = fopen(OUTPUT_FILE_DIRECTORY, "w");
        for(uint16_t i = 0; i < pointIndex; i++)
        {
            DataPoint dp = points[i];
            fprintf(file, "%.8f, %.8f, %.8f, %.8f, %.8f, %.8f, %.8f,\n", dp.time, dp.accelerationValue[0].x, dp.accelerationValue[0].y, dp.accelerationValue[0].z,
                                                                           dp.accelerationValue[1].x, dp.accelerationValue[1].y, dp.accelerationValue[1].z);
        }

        fclose(file);
    } 
    else
    {
        printf("%s\n", "Unable to open file");
    }

    printf("Kill yourself\n");

    fclose(file);
    return 0;
}