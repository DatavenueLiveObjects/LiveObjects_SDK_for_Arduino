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
#ifdef ARDUINO_ARCH_SAMD
    pinMode(PIN_USB_HOST_ENABLE, OUTPUT);
    digitalWrite(PIN_USB_HOST_ENABLE, LOW);
    #if defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500)
      pinMode(PMIC_IRQ_PIN, INPUT_PULLUP);
    #endif
#endif
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
String ToHex(int x)
{
  return ToHexT(x);
}
String ToHex(long x)
{
  return ToHexT(x);
}
String ToHex(float x)
{
  return ToHexT(x);
}
String ToHex(double x)
{
  return ToHexT(x);
}
String ToHex(long long x)
{
  return ToHexT(x);
}
String ToHex(int8_t x)
{
  return ToHexT(x);
}

String ToHex(unsigned int x)
{
  return ToHexTU(x);
}
String ToHex(unsigned long x)
{
  return ToHexTU(x);
}
String ToHex(unsigned long long x)
{
  return ToHexTU(x);
}
String ToHex(uint8_t x)
{
  return ToHexTU(x);
}