/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */

#include "ExtDevices.h"

VL6180xIdentification identification;


void VL6180XInit()
{
  VL6180x sensor(VL6180X_ADDRESS);
  Wire.begin();         // Start I2C library
  delay(1500);          // delay 1.5s

  sensor.getIdentification(&identification); // Retrieve manufacture info from device memory
  printVL6180xIdentification(&identification);      // Helper function to print all the Module information

  if (sensor.VL6180xInit() != 0)
  {
    Serial.println("Failed to initialize VL6180X sensor. Freezing..."); // Initialize device and check for errors
    while (1)
      ;
  }
  sensor.VL6180xDefautSettings(); // Load default settings to get started.
  delay(1000); // delay 1s
}

void printVL6180xIdentification(struct VL6180xIdentification *dev)
{
  Serial.println();
  Serial.println("VL6180X  sensor");

  Serial.print("Model ID = ");
  Serial.println(dev->idModel);

  Serial.print("Model Rev = ");
  Serial.print(dev->idModelRevMajor);
  
  Serial.println(dev->idModelRevMinor);

  Serial.print("Module Rev = ");
  Serial.print(dev->idModuleRevMajor);
  
  Serial.println(dev->idModuleRevMinor);

  Serial.print("Manufacture Date = ");
  Serial.print((dev->idDate >> 3) & 0x001F);
  Serial.print("/");
  Serial.print((dev->idDate >> 8) & 0x000F);
  Serial.print("/1");
  Serial.print((dev->idDate >> 12) & 0x000F);
  Serial.print(" Phase: ");
  Serial.println(dev->idDate & 0x0007);
  Serial.println();
}
