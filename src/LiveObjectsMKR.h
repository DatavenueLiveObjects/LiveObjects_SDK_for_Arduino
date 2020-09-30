#pragma once
/******************************************************************************
   MKR BOARDS BASE CLASS
 ******************************************************************************/
#include "LiveObjectsBase.h"
#include "ArduinoMqttClient.h"
class LiveObjectsMKR : public LiveObjectsBase
{
protected:
    LiveObjectsMKR();
    ~LiveObjectsMKR();
    LiveObjectsMKR(const LiveObjectsMKR&)  = delete;
    LiveObjectsMKR& operator== (const LiveObjectsMKR&) =  delete;

protected:
  void checkMQTT() override;
  void connectMQTT() override;
  void disconnectMQTT() override;
  void stopMQTT()override;
  void sendMQTT(String& topic, JsonDocument& doc)override;
  void sendMQTT(String& topic, String& doc)override;
  void deserializeMessage(JsonDocument& doc)override;
  void onMQTTmessage(int messageSize);

public:
  void addPowerStatus()override;
protected:
  Client* m_pClient;
  MqttClient *m_pMqttclient;
};
