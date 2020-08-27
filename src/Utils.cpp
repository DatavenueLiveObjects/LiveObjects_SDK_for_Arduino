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

String ToHex(String x)
{
  String hex;
  for(int i=0;i<x.length();++i)
    hex+=ToHexT(x[i]);
  return hex;
}

uint8_t hexBinary(char msb, char lsb)
{
  auto hexToDec = [](char x)
  {
    if(x>='a' && x<='f') return 10 + (x-'a');
    else return x-'0';
  };

  return (hexToDec(msb)<<4)|hexToDec(lsb); 
}