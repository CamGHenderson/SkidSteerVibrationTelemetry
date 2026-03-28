#pragma once
#include "Vector.h"

#define ADXL375_COUNT 3

void ADXL375_initialize();
void ADXL375_terminate();
Vec3f ADXL375_read();