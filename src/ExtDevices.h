/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */

#include <Wire.h>
#include <SparkFun_VL6180X.h>

#define VL6180X_ADDRESS 0x29

void VL6180XInit();
void printVL6180xIdentification(struct VL6180xIdentification *temp);
