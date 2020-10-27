#include "LiveObjectsMKR.h"
LiveObjectsMKR::LiveObjectsMKR()
  :
   LiveObjectsBase()
  ,m_pClient(nullptr)
  ,m_pMqttclient(nullptr)
{}

LiveObjectsMKR::~LiveObjectsMKR()
{
  if(m_pMqttclient != nullptr)delete m_pMqttclient;
  if(m_pClient != nullptr)delete m_pClient;
}

void LiveObjectsMKR::deserializeMessage(JsonDocument& doc)
{
  deserializeJson(doc,*m_pMqttclient);
}

void LiveObjectsMKR::sendMQTT(String& topic, JsonDocument& doc)
{
  m_pMqttclient->beginMessage(topic);
  serializeJson(doc, *m_pMqttclient);
  m_pMqttclient->endMessage();
}

void LiveObjectsMKR::sendMQTT(String& topic, String& doc)
{
  m_pMqttclient->beginMessage(topic);
  for(int i=0;i<m_sPayload.length();i+=2)
    m_pMqttclient->write(hexBinary(m_sPayload[i],m_sPayload[i+1]));
  m_pMqttclient->endMessage();
}

void LiveObjectsMKR::checkMQTT()
{
  m_pMqttclient->poll();
  if(!m_bSubCMD) if(commands.size()>0) m_pMqttclient->subscribe(MQTT_SUBCMD);
  if(!m_pMqttclient->connected())
    connectMQTT();
}
void LiveObjectsMKR::connectMQTT()
{
  if (SECRET_LIVEOBJECTS_API_KEY.length() != 32) // check if API key seems correct
  {
    outputDebug(ERR,"Please check your Live Objects API key (arduino_secrets.h file).\nStopping here.");
    while (1) ;
  }

  if (!m_bInitialMqttConfig) {
    m_pMqttclient->setTxPayloadSize(PAYLOAD_DATA_SIZE);
    m_pMqttclient->setId(m_sMqttid.c_str());
    m_pMqttclient->setUsernamePassword(MQTT_USER, SECRET_LIVEOBJECTS_API_KEY);
  }

  outputDebug(INFO,"Connecting to MQTT broker ",MQTT_BROKER,":",m_nPort);


  while (!m_pMqttclient->connect(MQTT_BROKER, m_nPort)) outputDebug(TXT,".");
  outputDebug(INFO,"You're connected to the MQTT broker");

  if(parameters.size()>0)m_pMqttclient->subscribe(MQTT_SUBCFG);
  if(!m_bSubCMD && commands.size()>0) m_pMqttclient->subscribe(MQTT_SUBCMD);

  m_pMqttclient->poll();

  if (!m_bInitialMqttConfig) 
  {
    configurationManager("");
    m_bInitialMqttConfig = true;
  }
}

void LiveObjectsMKR::onMQTTmessage(int messageSize) {
  String topic = m_pMqttclient->messageTopic();
  if (topic == MQTT_SUBCFG)
    configurationManager(topic, messageSize);
  else if (topic == MQTT_SUBCMD)
    commandManager(topic);
}
void LiveObjectsMKR::disconnectMQTT()
{
  outputDebug(INFO,"Closing MQTT connection...");
  m_pMqttclient->stop();
}

void LiveObjectsMKR::stopMQTT()
{
  if(m_pMqttclient!= nullptr)
    {
      disconnectMQTT();
      delete m_pMqttclient;
      m_pMqttclient=nullptr;
      delete m_pClient;
      m_pClient=nullptr;
    }
}

void LiveObjectsMKR::addPowerStatus()
{
  #ifdef PMIC_PRESENT
  byte DATA = readRegister(SYSTEM_STATUS_REGISTER);
  bool charging = (DATA & ((1<<5)|(1<<4)))!=0;
  bool bat = (DATA & (1<<0)) == 0 || charging;
  bool power=(DATA & (1<<2));
  double voltage=0.;
  // outputDebug(INFO,  DATA&0b11000000u);
  // outputDebug(INFO,  DATA);
  // outputDebug(INFO,  power);
  if(bat) voltage = analogRead(ADC_BATTERY) * (4.3 / 1023.0);
  if(m_Protocol == MQTT && m_Encoding==TEXT)
  {
    addToPayload(easyDataPayload[JSONVALUE].createNestedObject("powerStatus"),
                 "external_power",(charging? true : ((!bat && power)?true:power))
                ,"battery_connected",bat 
                ,"battery_charging", charging
                ,"battery_voltage",voltage);
  }
  else addToStringPayload((charging ? true : (!bat && power ? true : power)), bat ,charging, voltage);
  #endif
}

