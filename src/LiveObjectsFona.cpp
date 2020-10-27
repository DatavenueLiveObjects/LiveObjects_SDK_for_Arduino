#include "LiveObjectsFona.h"
#ifdef FONAD
LiveObjectsFona::LiveObjectsFona() 
  :
   m_Fona(FONA_RST)
   ,m_FonaSerial(FONA_TX,FONA_RX)
   ,m_FonaMQTT(&m_Fona,MQTT_BROKER, 1883,"Fona", MQTT_USER, SECRET_LIVEOBJECTS_API_KEY)
   ,m_sClientID()
   ,m_nPort(1883)
   ,m_Security(NONE)
{
  clearPayload();
}

void LiveObjectsFona::begin(Protocol p, Encoding e, bool d) 
{
  if(m_Encoding == BINARY)
  {
    //Serial.println("[ERROR] This board support only Text mode");
  }
  m_Protocol=p;
  m_Encoding=e;
  m_FonaMQTT.setPort(m_nPort);
}

void LiveObjectsFona::loop() 
{
  
  unsigned long now = millis();
  if (now - lastKeepAliveNetwork > KEEP_ALIVE_NETWORK) {
    m_Fona.getNetworkStatus();
    if(m_Protocol == MQTT) m_FonaMQTT.ping();
    lastKeepAliveNetwork = now;
  }
}

void LiveObjectsFona::connect() 
{
  while (! FONAconnect()) {
    //Serial.print(".");
  }
  Serial.println("[INFO] Connected to Cellular!");
  delay(5000);  // wait a few seconds to stabilize connection
  if(m_Protocol==MQTT)
  {
    m_Fona.getIMEI(m_sClientID);
    connectMQTT();
  }
}

void LiveObjectsFona::sendData() 
{
  if(m_Protocol == MQTT)
  {
    if(m_Encoding==TEXT)
    {
      m_Payload[JSONMODEL] = JSONMODELNAME;
      publishMessage(MQTT_PUBDATA, m_Payload);
    }
    //else publishMessage(m_sTopic+m_sDecoder, m_sPayload);
  }
  else
  {
    //Serial.print("[INFO] Publishing message: ");
    //Serial.println(m_BufferPayload);
    if(strlen(m_BufferPayload) > 160)
    {
      //Serial.println("[ERROR] Payload to big, skipping sending...");
      return;
    }
    else if (strlen(m_BufferPayload) < 1)
    {
      ////Serial.println("[ERROR] Payload is empty, skipping sending...");
      return;
    }
    if(m_Encoding==TEXT)
    {
      Serial.print("[INFO] Publishing message: ");
      Serial.println(m_BufferPayload);
      m_Fona.sendSMS(SECRET_SERVER_MSISDN.c_str(),m_BufferPayload);
    }
  }
  clearPayload();
}

void LiveObjectsFona::addCommand(char* name, onCommandCallback cb) 
{
  //Serial.println("[ERROR] Command are not supported on this board");
}

void LiveObjectsFona::changeConfiguration(Protocol p,Security s, Encoding mode) 
{
  //Serial.print("[ERROR] This board doesn't support this function");
}

void LiveObjectsFona::setSecurity(Security s) 
{
  //Serial.println("This board doesn't support TLS"); m_Security=NONE;
}

void LiveObjectsFona::enableDebug(bool b) 
{
  //Serial.print("[ERROR] This board doesn't support this function");
}

// void LiveObjectsFona::setClientID(const char* id) 
// {
//   m_sClientID=id;
// }

void LiveObjectsFona::setDecoder(char* s) 
{
  //Serial.print("[ERROR] This board doesn't support this function");
}

void LiveObjectsFona::addTimestamp(time_t timestamp) 
{
  //Serial.print("[ERROR] This board doesn't support this function");
}

void LiveObjectsFona::addLocation(double lat, double lon, double alt) 
{
  //Serial.print("[ERROR] This board doesn't support this function");
}

void LiveObjectsFona::addPowerStatus() 
{
  //Serial.print("[ERROR] This board doesn't support this function");
}

void LiveObjectsFona::addNetworkInfo() 
{
  //Serial.print("[ERROR] This board doesn't support this function");
}

bool LiveObjectsFona::FONAconnect() 
{

  //Serial.println("[INFO] Initializing FONA");

  m_FonaSerial.begin(4800); // if you're using software serial

  if (!m_Fona.begin(m_FonaSerial)) { // can also try fona.begin(Serial1)
    //Serial.println("[ERROR] error initializing FONA");
    return false;
  }
  m_FonaSerial.println("AT+CMEE=2");
  delay(100);
  //m_Fona//Serial.println("AT+CMGF=1");
  delay(100);
  //Serial.println("[INFO] Fona Initialized");
  //Serial.println("[INFO] Connecting to cellular...");
  while (m_Fona.getNetworkStatus() != 1) {
    delay(500);
  }

  delay(5000); // wait a few seconds to stabilize connection

  if(m_Protocol != SMS) 
  {
    m_Fona.setGPRSNetworkSettings(F("internet"), F(""), F(""));

    delay(5000); // wait a few seconds to stabilize connection

    //Serial.println("[INFO] Enabling GPRS");
    if (!m_Fona.enableGPRS(true)) 
    {
      //Serial.println("[ERROR] Failed to turn GPRS on");
      return false;
    }
    //Serial.println("[INFO] Enabled!");
  }
  delay(5000); // wait a few seconds to stabilize connection
  return true;
}

void LiveObjectsFona::connectMQTT() 
{
  m_FonaMQTT.setClientID(m_sClientID);
  int8_t ret;

  // Stop if already connected.
  if (m_FonaMQTT.connected()) {
    return;
  }
  delay(2000);
  Serial.println("[INFO] Connecting to MQTT ");

  while ((ret = m_FonaMQTT.connect()) != 0) { // connect will return 0 for connected
    Serial.println(m_FonaMQTT.connectErrorString(ret));
    //Serial.print(".");
    //m_FonaMQTT.disconnect();
    delay(5000);  // wait 5 seconds
  }
  //Serial.println();
  Serial.println("[INFO] MQTT Connected!");
}

void LiveObjectsFona::clearPayload() 
{
  m_Payload.clear();
  memset(m_BufferPayload, '\0', PAYLOAD_DATA_SIZE);
  m_Payload.createNestedObject(JSONVALUE);
}

void LiveObjectsFona::publishMessage(const char* topic, JsonDocument& payload) 
{
  if( measureJson(payload) >= PAYLOAD_DATA_SIZE )
  {
    //Serial.println("[ERROR] Message is to big");
    return;
  }
  serializeJson(payload,m_BufferPayload);
  Serial.println("Publishing message:");
  Serial.println(m_BufferPayload);
  m_FonaMQTT.publish(topic, m_BufferPayload);
}

#endif