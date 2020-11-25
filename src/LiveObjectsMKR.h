/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */
#pragma once
/******************************************************************************
   MKR BOARDS BASE CLASS
 ******************************************************************************/
#include "LiveObjectsBase.h"
#include "MqttClient.h"
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
