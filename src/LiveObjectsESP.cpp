/*
 * Copyright (C) Orange
 *
 * This software is distributed under the terms and conditions of the 'MIT'
 * license which can be found in the file 'LICENSE.md' in this package distribution
 */
#include "LiveObjectsESP.h"

#if defined ESP8266D || defined ESP32D
LiveObjectsESP::LiveObjectsESP()
  :
   LiveObjectsBase()
  ,m_pClient(nullptr)
  ,m_pMqttclient(nullptr)
{}

LiveObjectsESP::~LiveObjectsESP()
{
  if(m_pClient!=nullptr)delete m_pClient;
  if(m_pMqttclient!=nullptr) delete m_pMqttclient;
}
void LiveObjectsESP::begin(Protocol p, Encoding s, bool bDebug) 
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
    m_pClient = new WiFiClientSecure();
    m_pMqttclient = new PubSubClient(*m_pClient);
    m_nPort = 8883;
    #ifdef ESP32D
    ((WiFiClientSecure*)m_pClient)->setCACert(SERVER_CERT);
    #else
    ((WiFiClientSecure*)m_pClient)->setCACert(LO_ROOT_CERT.data, LO_ROOT_CERT.size);
    #endif
    break;
    case NONE:
    m_pClient = new WiFiClient();
    m_pMqttclient = new PubSubClient(*m_pClient);
    m_nPort = 1883;
    break;
    default:
    outputDebug(ERR,"Wrong mode! Stopping...");
    while(true);
  }
  m_bInitialized = true;
  m_pMqttclient->setCallback(messageCallback);
}

void LiveObjectsESP::connectNetwork() 
{
  outputDebug(INFO, "Connectong to ", SECRET_SSID);
  #ifdef ESP8266D
    WiFi.begin(SECRET_SSID, SECRET_WIFI_PASS);
  #else
    WiFi.begin(SECRET_SSID.c_str(), SECRET_WIFI_PASS.c_str());
  #endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    outputDebug(TXT,".");
  }
  IPAddress ip = WiFi.localIP();
  for(int i=0;i<4;++i)
  {
    m_sIP+=ip[i];
    if(i!=3) m_sIP+='.';
  }

  outputDebug(INFO,"Connected, IP addres: ", m_sIP);
   uint8_t mac[6];
  char buff[10];
  WiFi.macAddress(mac);

  for(int i=0;i<6;++i)
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
  if(i!=5) m_sMac += ':';
  }

  // while(true)
  // {
  //   outputDebug(INFO, m_sIP," ",  m_sMqttid, " ",  m_sMac);
  //   delay(500);
  // }
  m_sModel = m_sMac;
}

void LiveObjectsESP::checkNetwork() 
{
  if(WiFi.status() != WL_CONNECTED)
    connectNetwork();
}

void LiveObjectsESP::disconnectNetwork() 
{
  WiFi.disconnect();
}

void LiveObjectsESP::checkMQTT() 
{
  m_pMqttclient->loop();
  if(!m_pMqttclient->connected())
    connectMQTT();
}

void LiveObjectsESP::connectMQTT() 
{
  if (SECRET_LIVEOBJECTS_API_KEY.length() != 32) // check if API key seems correct
  {
    outputDebug(ERR,"Please check your Live Objects API key (arduino_secrets.h file).\nStopping here.");
    while (1) ;
  }

  if (!m_bInitialMqttConfig) {
    m_pMqttclient->setServer(MQTT_BROKER, m_nPort);
  }

  outputDebug(INFO,"Connecting to MQTT broker ",MQTT_BROKER,":",m_nPort);
  
  while (!m_pMqttclient->connect(m_sMqttid.c_str(),MQTT_USER, SECRET_LIVEOBJECTS_API_KEY.c_str())) outputDebug(TXT,".");
  outputDebug(INFO,"You're connected to the MQTT broker");

  #ifdef ESP8266D
  if(parameters.size()>0) outputDebug(ERR,"Parameters not supported!");
  #else
  if(parameters.size()>0) m_pMqttclient->subscribe(MQTT_SUBCFG);
  #endif
  if(!m_bSubCMD && commands.size()>0) m_pMqttclient->subscribe(MQTT_SUBCMD);

  m_pMqttclient->loop();

  if (!m_bInitialMqttConfig) 
  {
    #ifndef ESP8266D
    configurationManager("");
    #endif
    m_bInitialMqttConfig = true;
  }
}

void LiveObjectsESP::disconnectMQTT() 
{
  m_pMqttclient->disconnect();
}

void LiveObjectsESP::stopMQTT() 
{
  if(m_pMqttclient!=nullptr)
  {
    disconnectMQTT();
    delete m_pMqttclient;
    m_pMqttclient=nullptr;
    delete m_pClient;
    m_pClient=nullptr;
  }
}

void LiveObjectsESP::sendMQTT(String& topic, JsonDocument& doc) 
{
  m_pMqttclient->beginPublish(topic.c_str(), measureJson(doc), false);
  serializeJson(doc,*m_pMqttclient);
  m_pMqttclient->endPublish();
}

void LiveObjectsESP::sendMQTT(String& topic, String& doc) 
{
  m_pMqttclient->beginPublish(topic.c_str(), doc.length()/2, false);
  for(int i=0;i<m_sPayload.length();i+=2)
    m_pMqttclient->write(hexBinary(m_sPayload[i],m_sPayload[i+1]));
  m_pMqttclient->endPublish();
}

void LiveObjectsESP::deserializeMessage(JsonDocument& doc) 
{
  outputDebug(INFO,"DESERIALIZING");
  deserializeJson(doc,m_sRecvBuffer);
}


void LiveObjectsESP::addPowerStatus() 
{
  
}

void LiveObjectsESP::addNetworkInfo() 
{
  String tmp;
  tmp = String(WiFi.RSSI(),10);
  tmp += " dbm";
  if(m_Protocol == MQTT && m_Encoding==TEXT)  addToPayload(easyDataPayload[JSONVALUE].createNestedObject("networkInfo"),"mac",m_sMac,"ssid",SECRET_SSID,"ip",m_sIP,"strength",tmp);
  else addToPayload(m_sMac,SECRET_SSID,m_sIP,tmp);
}

void LiveObjectsESP::messageCallback(char* topic, uint8_t* payload, unsigned int length)
{
  LiveObjects::get().onMQTTmessage(topic,payload,length);
}

void LiveObjectsESP::onMQTTmessage(char* topic, uint8_t* payload, unsigned int length) 
{
  String stopic(topic);
  m_sRecvBuffer="";
  for(int i=0;i<length;++i) m_sRecvBuffer+=(char)payload[i];
  if(stopic==MQTT_SUBCFG)
  {
    #ifdef ESP8266D
    outputDebug(ERR, "Parameters not supported on this board!");
    return;
    #endif
    configurationManager(topic,1);
  }
  else if(stopic==MQTT_SUBCMD)
    commandManager(topic);
}
#endif