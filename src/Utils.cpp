#include "Utils.h"
#include <Arduino.h>

byte readRegister(byte address) {
    Wire.beginTransmission(PMIC_ADDRESS);
    Wire.write(address);

    if (Wire.endTransmission(true) != 0) {
      return -1;
    }

    if (Wire.requestFrom(PMIC_ADDRESS, 1, true) != 1) {
      return -1;
    }

    return Wire.read();
}


void batteryBegin()
{
  Wire.begin();

   //check PMIC version
  if (readRegister(PMIC_VERSION_REGISTER) != 0x23) {
      Serial.println("[ERROR] PMIC NOT COMPATIBLE STOPPING");
      while(true);
  }
}
