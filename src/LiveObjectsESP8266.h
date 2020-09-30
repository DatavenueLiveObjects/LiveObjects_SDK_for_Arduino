#pragma once
#ifdef ESP8266
#include "LiveObjectsBase.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
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
    WiFiClient* m_pClient;
    PubSubClient* m_pMqttclient;
    String m_sMac;
    String m_sIP;
    String m_sRecvBuffer;
};

extern const String SECRET_SSID;
extern const String SECRET_WIFI_PASS;

typedef LiveObjectsESP LiveObjects;
#endif