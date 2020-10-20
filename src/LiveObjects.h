#pragma once
 /******************************************************************************
   Arduino MKR BOARDS
 ******************************************************************************/
#ifdef ARDUINO_ARCH_SAMD
  #if defined ARDUINO_SAMD_MKRGSM1400 || defined ARDUINO_SAMD_MKRNB1500
    #define Cellular
    #include "LiveObjectsCellular.h"
  #elif defined ARDUINO_SAMD_MKRWIFI1010 || defined ARDUINO_SAMD_NANO_33_IOT || defined ARDUINO_SAMD_MKRVIDOR4000 || defined ARDUINO_SAMD_MKR1000 || ADAFRUIT_FEATHER_M0
    #define WIFI_
    #include "LiveObjectsWiFi.h"
  #endif

  /* Power status for supported MKR boards*/
  #if defined ARDUINO_SAMD_MKRWIFI1010 || defined ARDUINO_SAMD_MKRNB1500 || defined ARDUINO_SAMD_MKRGSM1400
    #define PMIC_PRESENT
  #endif
#endif

 /******************************************************************************
   ESP8266 and ESP32 BOARDS 
 ******************************************************************************/
#if defined ESP8266 || defined ESP32
  #include "LiveObjectsESP.h"
#endif

 /******************************************************************************
   Adafruit FONA
 ******************************************************************************/
#ifdef ARDUINO_AVR_FEATHER32U4
  #include "LiveObjectsFONA.h"
#endif


/******************************************************************************
  CLASS DRAFT BOARD
******************************************************************************/
/*
#ifdef COMPILER_SYMBOL_FOR_BOARD

#include <necessaryLibrary.h>

class className : public LiveObjectsBase
{
public:
  static className& get(){
    static className g;
    return g;
  }


  private:
    void connectNetwork() override;
    void checkNetwork() override;
    void disconnectNetwork() override;

  protected:
    void checkMQTT() override;
    void connectMQTT() override;
    void disconnectMQTT() override;
    void stopMQTT()override;
    void sendMQTT(String& topic, JsonDocument& doc)override;
    void sendMQTT(String& topic, String& doc)override;
    void deserializeMessage(JsonDocument& doc)override;

  public:
    void begin(Protocol p=MQTT, Encoding s=TEXT, bool bDebug=true) override;
    void addPowerStatus()override;
    void addNetworkInfo() override;

  private:
  //Necessary variables
};

typedef className LiveObjects;
#endif

*/
extern LiveObjects& lo;
