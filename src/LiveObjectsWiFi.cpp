/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */
#include "LiveObjectsWiFi.h"
#ifdef WIFID
LiveObjectsWiFi::LiveObjectsWiFi()
  :
   LiveObjectsMKR()
   ,m_sMac()
   ,m_sIP()
{
}
LiveObjectsWiFi::~LiveObjectsWiFi()
{}



void LiveObjectsWiFi::begin(Protocol p, Encoding s, bool bDebug)
{
  LiveObjectsBase::begin(p,s,bDebug);
  if(p != MQTT)
  {
    outputDebug(ERR,"Wrong protocol! This board support only MQTT! Stopping....");
    while(true);
  }
  switch(m_Security)
  {
    case TLS:
    #ifdef ARDUINO_SAMD_MKR1000
      outputDebug(ERR,"TLS NOT COMPATIBLE, STOPPING...");
      while(true);
    #endif
    m_pClient = new WiFiSSLClient();
    m_pMqttclient = new MqttClient(m_pClient);
    m_nPort = 8883;
    break;
    case NONE:
    m_pClient = new WiFiClient();
    m_pMqttclient = new MqttClient(m_pClient);
    m_nPort = 1883;
    break;
    default:
    outputDebug(ERR,"Wrong mode! Stopping...");
    while(true);
  }
  m_bInitialized = true;
  m_pMqttclient->onMessage(messageCallback);
}

void LiveObjectsWiFi::connectNetwork()
{
  #ifdef ADAFRUIT_FEATHER_M0
    WiFi.setPins(8,7,4,2);
  #endif
  if(!m_bInitialized)
  {
    outputDebug(WARN,"missing begin() call, calling with default protcol=MQTT, security protcol=TLS, debug=true");
    begin();
  }
  if (WiFi.status() == WL_NO_SHIELD) {
    outputDebug(ERR,"Communication with WiFi module failed! Stopping here...");
    // don't continue
    while (true);
  }

  if(String(SECRET_SSID).length()<1)
  {
    outputDebug(ERR,"SECRET_SSID is empty, check arduino_secrets.h, Stopping here...");
    while(true);
  }
  String s = WiFi.firmwareVersion();
  outputDebug(INFO,"WiFi firmware version ",s);

  outputDebug(INFO,"Attempting to connect to SSID: ", SECRET_SSID);

  while (WiFi.begin(SECRET_SSID.c_str(), SECRET_WIFI_PASS.c_str()) != WL_CONNECTED) {
    // failed, retry
    outputDebug(TXT,".");
    delay(1000);
  }
   outputDebug();
  IPAddress ip = WiFi.localIP();
  for(int i=0;i<4;++i)
  {
    m_sIP+=ip[i];
    if(i!=3) m_sIP+='.';
  }

   uint8_t mac[6];
  char buff[10];
  WiFi.macAddress(mac);

  for(int i=5;i>=0;--i)
  {
    memset(buff,'\0',10);
    itoa(mac[i],buff,16);
    if(mac[i]<17)
    {
      m_sMac+="0";
      m_sMqttid+="0";
    }
    for(int j=0;j<strlen(buff);++j)
    {
      m_sMac += (char)toupper(buff[j]);
      m_sMqttid += (char)toupper(buff[j]);
    }
  if(i!=0) m_sMac += ':';
  }
  m_sModel = m_sMqttid;
}
void LiveObjectsWiFi::checkNetwork()
{
  if(WiFi.status()== WL_DISCONNECTED)
    connectNetwork();
}
void LiveObjectsWiFi::disconnectNetwork()
{
  outputDebug(INFO,"Disconnecting WiFi");
  WiFi.disconnect();
}
void LiveObjectsWiFi::messageCallback(int msg)
{
  LiveObjects::get().onMQTTmessage(msg);
}







void LiveObjectsWiFi::addNetworkInfo()
{
  String tmp;
  tmp = WiFi.RSSI();
  tmp += " dbm";
  if(m_Protocol == MQTT && m_Encoding==TEXT)  addToPayload(easyDataPayload[JSONVALUE].createNestedObject("networkInfo"),"mac",m_sMac,"ssid",SECRET_SSID,"ip",m_sIP,"strength",tmp);
  else addToPayload(m_sMac,SECRET_SSID,m_sIP,tmp);
}
#endif