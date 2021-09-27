/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */
#include "LiveObjectsMKR.h"
#ifdef LIVE_OBJECTS_IOT_SAFE
#include "IoTSAFE.h"
#endif
#if defined ARDUINO_SAMD_MKRNB1500
#define NBD
#include <MKRNB.h>
#elif defined ARDUINO_SAMD_MKRGSM1400
#define GSMD
#include <MKRGSM.h>
#endif

#if defined NBD || defined GSMD
class LiveObjectsCellular : public LiveObjectsMKR
{
  public:
    static LiveObjectsCellular& get()
    {
      static LiveObjectsCellular g; return g;
    }

  private:
    LiveObjectsCellular();
    ~LiveObjectsCellular();
    LiveObjectsCellular(const LiveObjectsCellular&)  = delete;
    LiveObjectsCellular& operator== (const LiveObjectsCellular&) =  delete;
  public:
    void begin(Protocol p=MQTT, Encoding s=TEXT, bool bDebug=true) override;
    void addNetworkInfo() override;
    void sendData();
  private:
    String parseCommand(String inputString);
    void connectNetwork() override;
    void checkNetwork() override;
    void disconnectNetwork() override;
    static void messageCallback(int msg);
    #ifdef LIVE_OBJECTS_IOT_SAFE
    unsigned long getTime();
    static unsigned long trampolineGetTime();
    #endif
  private:
  #ifdef NBD
  NB m_Acces;
  NBScanner m_Scanner;
  NB_SMS m_Sms;
  #endif
  #ifdef GSMD
  GSM m_Acces;
  GPRS m_GPRSAcces;
  GSMScanner m_Scanner;
  GSM_SMS m_Sms;
  #endif
  String m_sNumber;
  #ifdef LIVE_OBJECTS_IOT_SAFE
  IoTSAFE m_IoTSafe;
  #endif
};

typedef LiveObjectsCellular LiveObjects;

extern const String SECRET_SERVER_MSISDN;
extern const String SECRET_PINNUMBER;
extern const String SECRET_APN;
extern const String SECRET_APN_USER;
extern const String SECRET_APN_PASS;
#endif
