#include "LiveObjectsFONA.h"
#ifdef FONAD
LiveObjectsFONA::LiveObjectsFONA() 
    :
     LiveObjectsBase()
    ,m_FonaSerial(FONA_TX, FONA_RX)
    ,m_Fona(FONA_RST)
    ,m_FonaMQTT(nullptr)
    ,m_subCommands(nullptr)
    ,m_subParameters(nullptr)
{

    String num = SECRET_SERVER_MSISDN;
  for (int i = 0; i < num.length(); i+=2)
    {
        if ((i + 1) == num.length() && num.length() % 2 == 1)
        {
            num += num[i];
            num[i] = 'F';
        }
        else
        {
        char tmp = num[i + 1];
        num[i + 1] = num[i];
        num[i] = tmp;
        }
    }
    m_sNumber += ToHexT((char)(num.length() - 1));
    m_sNumber += "A1";
    m_sNumber += num;
}

LiveObjectsFONA::~LiveObjectsFONA()
{
    if(m_subCommands!=nullptr)delete m_subCommands;
    if(m_subParameters!=nullptr) delete m_subParameters;
    if(m_FonaMQTT!=nullptr) delete m_FonaMQTT;
}


bool LiveObjectsFONA::connectFONA()
{
    Watchdog.reset();

  Serial.println(F("Initializing FONA....(May take 3 seconds)"));

  m_FonaSerial.begin(4800); // if you're using software serial

  if (!m_Fona.begin(m_FonaSerial)) { // can also try fona.begin(Serial1)
    Serial.println(F("Couldn't find FONA"));
    return false;
  }
  m_FonaSerial.println("AT+CMEE=2");
  Serial.println(F("FONA is OK"));
  Watchdog.reset();
  Serial.println(F("Checking for network..."));
  while (m_Fona.getNetworkStatus() != 1) {
    delay(500);
  }

  Watchdog.reset();
  delay(5000); // wait a few seconds to stabilize connection
  Watchdog.reset();

  //m_Fona.setGPRSNetworkSettings(SECRET_APN.c_str(), SECRET_APN_USER.c_str(), SECRET_APN_PASS.c_str());

  Serial.println(F("Disabling GPRS"));
  m_Fona.enableGPRS(false);

  Watchdog.reset();
  delay(5000); // wait a few seconds to stabilize connection
  Watchdog.reset();

  Serial.println(F("Enabling GPRS"));
  if (!m_Fona.enableGPRS(true)) {
    Serial.println(F("Failed to turn GPRS on"));
    return false;
  }
  Watchdog.reset();

  return true;
}

void LiveObjectsFONA::connectNetwork() 
{

  if (!m_bInitialized)
  {
    outputDebug(WARN,"missing begin() call, calling with default protcol=MQTT, security protcol=TLS, debug=true");
    begin();
  }

  while (! connectFONA())
      outputDebug(TXT,".");


  char imei[16];
  uint8_t len = m_Fona.getIMEI(imei);
  if(len>0) m_sMqttid=imei;
  else
  {
      outputDebug(ERR, "Cannot obtain sim card IMEI, stopping here...");
      while(true);
  }
  
  if(m_nPort==8883)
  {
      if (!m_bCertLoaded) 
      {
      outputDebug(INFO,"Loading DigiCert Root CA certificate");
      char send[128];
      sprintf(send,"AT+USECMNG=0,0,\"%s\",%d", LO_ROOT_CERT.name, LO_ROOT_CERT.size);
      //error check needed
      m_Fona.println(send);
      m_FonaSerial.write(LO_ROOT_CERT.data, LO_ROOT_CERT.size);
      m_bCertLoaded = true;
      outputDebug(INFO,"Certificate loaded");
      }
  }
}

void LiveObjectsFONA::checkNetwork() 
{
    if(m_Fona.getNetworkStatus()!=1 || m_Fona.getNetworkStatus()!=5)
        connectNetwork();

    Adafruit_MQTT_Subscribe *tmp=nullptr;
    while(tmp = m_FonaMQTT->readSubscription(1000))
    {
        if(tmp == &m_subCommands)
        {
            m_subLast = m_subCommands;
            commandManager(MQTT_SUBCMD);
            m_subLast = nullptr;
        }
        else if(tmp==&m_subParameters)
        {
            m_subLast = m_subParameters;
            configurationManager(MQTT_SUBCFG,1);
            m_subLast = nullptr;
        }
    }
    //ToDo parsing received vals from sms
}

void LiveObjectsFONA::disconnectNetwork() 
{
    //m_Fona.disconnect()
}

void LiveObjectsFONA::checkMQTT() 
{
    if(!m_FonaMQTT->connected())
        connectMQTT();
}

void LiveObjectsFONA::connectMQTT() 
{
    if (SECRET_LIVEOBJECTS_API_KEY.length() != 32) // check if API key seems correct
    {
        outputDebug(ERR,"Please check your Live Objects API key (arduino_secrets.h file).\nStopping here.");
        while (1) ;
    }

    outputDebug(INFO,"Connecting to mqtt broker ", MQTT_BROKER);
    while(m_FonaMQTT->connect()!=0)
    {
       outputDebug(TXT,".");
       m_FonaMQTT->disconnect();
    }
    outputDebug();

    if(commands.size()>0) m_subCommands = new Adafruit_MQTT_Subscribe(m_FonaMQTT,MQTT_USER,MQTT_SUBCMD);
    if(parameters.size()>0) m_subParameters = new Adafruit_MQTT_Subscribe(m_FonaMQTT,MQTT_USER,MQTT_SUBCFG);

    if (!m_bInitialMqttConfig) 
    {
    configurationManager("");
    m_bInitialMqttConfig = true;
    }

}

void LiveObjectsFONA::disconnectMQTT() 
{
    outputDebug(INFO,"Closing MQTT connection...");
    m_FonaMQTT->disconnect();
}

void LiveObjectsFONA::stopMQTT() 
{
    if(m_FonaMQTT!=nullptr)
    {
        disconnectMQTT();
        delete m_FonaMQTT;
    }
}

void LiveObjectsFONA::sendMQTT(String& topic, JsonDocument& doc) 
{
    String payload;
    deserializeJson(doc,payload);
    m_FonaMQTT->publish(MQTT_PUBDATA, payload.c_str());
}

void LiveObjectsFONA::sendMQTT(String& topic, String& doc) 
{
    m_FonaMQTT->publish(MQTT_PUBDATA, doc.c_str());
}

void LiveObjectsFONA::deserializeMessage(JsonDocument& doc) 
{
    if(m_subLast!=nullptr)
    {
        String xd;
        for(int i=0;i<100;++i)
        {
            xd += (char)m_subLast->lastread[i];
        }
        serializeJson(doc,xd);
    }
    else
    {
        outputDebug(ERR, "Trying to read empty message!, stopping...");
        while(true);
    }
}

void LiveObjectsFONA::begin(Protocol p, Encoding s, bool bDebug) 
{
  LiveObjectsBase::begin(p,s,bDebug);
  if(p==MQTT)
  {
    switch(m_Security)
    {
      case TLS:
      m_nPort = 8883;
      break;
      case NONE:
      m_nPort = 1883;
      break;
      default:
      outputDebug(ERR,"Wrong mode! stopping...");
      while(true);
    }
    m_FonaMQTT = new Adafruit_MQTT_FONA(&m_Fona, MQTT_BROKER, m_nPort, MQTT_USER, SECRET_LIVEOBJECTS_API_KEY.c_str());
  }
  else
  {
    if(SECRET_SERVER_MSISDN.length()<3)
    {
      outputDebug(ERR,"SERVER MSISDN is empty! Check arduino_secrets.h! Stopping here....");
      while(true);
    }
  }
  m_bInitialized = true;
}


void LiveObjectsFONA::sendData()
{
  if(m_Protocol == MQTT) LiveObjectsBase::sendData();
  else
  {
    if(m_sPayload.length() > 160)
    {
      outputDebug(ERR,"Payload to big, skipping sending...");
      return;
    }
    else if (m_sPayload.length() < 1)
    {
      outputDebug(WARN,"Payload is empty, skipping...");
      return;
    }
    if(m_Encoding==TEXT)
    {
      outputDebug(INFO,"Publishing message: ", m_sPayload);
      m_Fona.sendSMS(SECRET_SERVER_MSISDN.c_str(), m_sPayload.c_str());
    }
    else
    {
      outputDebug(INFO,"Payload before conversion: ",m_sPayload);
      String msg;
      static String fixedStart = "001100";
      static String fixedInfo = "0000FF";
      msg+=fixedStart;
      msg+=m_sNumber;
      msg+= fixedInfo;
      msg+=ToHex((char)m_sPayload.length());
      msg+=to7bit(m_sPayload);
      int msgSize = (msg.length()-2)/2;
      msg+='\x1A';
      outputDebug(INFO,"Message len: ",msgSize);
      outputDebug(INFO,"Publishing message: ", msg);
      char cmd[30];
      sprintf(cmd,"AT+CMGS=%d\r",msgSize);
      m_Fona.print(cmd);
      delay(100);
      m_Fona.print(msg);

    }
    m_sPayload="";
  }
}

void LiveObjectsFONA::addPowerStatus() 
{
    
}

void LiveObjectsFONA::addNetworkInfo() 
{
    
}
#endif