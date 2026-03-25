#!/bin/bash

gcc src/App.c src/ADXL375.c src/I2C_Device.c -o bin/App.out -lpigpio