#pragma once
#include "Vector.h"

void ADXL375_initialize();
void ADXL375_terminate();
Vec3f ADXL375_read();