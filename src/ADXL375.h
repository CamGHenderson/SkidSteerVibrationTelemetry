#pragma once
#include "Vector.h"
#include <stdint.h>

#define ADXL375_COUNT 3

void ADXL375_initialize();
void ADXL375_terminate();
void ADXL375_calibrate();
Vec3f ADXL375_read(uint8_t sensorIndex);