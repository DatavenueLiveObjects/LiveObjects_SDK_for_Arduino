/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */
#pragma once
#if defined ESP8266 || defined ESP32
#include "LiveObjectsBase.h"
  #ifdef ESP8266 
    #include <ESP8266WiFi.h>
    #define ESP8266D
  #else
    #include <WiFi.h>
    #include <WiFiClientSecure.h>
    #define ESP32D
  #endif
#include <PubSubClient.h>
#endif

#if defined ESP32D || defined ESP8266D
class LiveObjectsESP : public LiveObjectsBase
{
  LiveObjectsESP();
  ~LiveObjectsESP();
  LiveObjectsESP(const LiveObjectsESP&)  = delete;
  LiveObjectsESP& operator== (const LiveObjectsESP&) =  delete;

  public:
    static LiveObjectsESP& get()
    {
      static LiveObjectsESP e; return e;
    }

  private:
    void connectNetwork() override;
    void checkNetwork() override;
    void disconnectNetwork() override;
    static void messageCallback(char* topic, uint8_t* payload, unsigned int length);

  protected:
    void checkMQTT() override;
    void connectMQTT() override;
    void disconnectMQTT() override;
    void stopMQTT()override;
    void sendMQTT(String& topic, JsonDocument& doc)override;
    void sendMQTT(String& topic, String& doc)override;
    void deserializeMessage(JsonDocument& doc)override;
    void onMQTTmessage(char* topic, uint8_t* payload, unsigned int length);

  public:
    void begin(Protocol p=MQTT, Encoding s=TEXT, bool bDebug=true) override;
    void addPowerStatus()override;
    void addNetworkInfo() override;

  private:
    Client* m_pClient;
    PubSubClient* m_pMqttclient;
    String m_sMac;
    String m_sIP;
    String m_sRecvBuffer;
};

extern const String SECRET_SSID;
extern const String SECRET_WIFI_PASS;

typedef LiveObjectsESP LiveObjects;
#endif