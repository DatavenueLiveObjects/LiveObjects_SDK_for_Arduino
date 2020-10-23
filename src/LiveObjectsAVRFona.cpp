#include "LiveObjectsAVRFona.h"

LiveObjectsFona::LiveObjectsFona() 
  :
   m_Fona(FONA_RST)
   ,m_FonaSerial(FONA_TX,FONA_RX)
   ,m_FonaMQTT(&m_Fona,MQTT_BROKER, 1883,"Fona", MQTT_USER, SECRET_LIVEOBJECTS_API_KEY)
   ,m_sClientID()
   ,m_nPort(1883)
   ,m_nSubs(0)
{
  clearPayload();
}

void LiveObjectsFona::begin(Protocol p, Encoding e, bool d) 
{
  m_FonaMQTT.setPort(m_nPort);
  m_Protocol=p;
  m_Encoding=e;
}

void LiveObjectsFona::loop() 
{
  char buffer[100];
  if(m_FonaMQTT.readMessage(buffer))
  {
    StaticJsonDocument<100> doc;
    deserializeJson(doc,buffer);
    Serial.println(doc["req"].as<char*>());
    Serial.println(doc["res"].as<char*>());
  }
}

void LiveObjectsFona::connect() 
{
  while (! FONAconnect()) {
    Serial.println("Retrying FONA");
  }
  m_Fona.getIMEI(m_sClientID);
  Serial.println(F("Connected to Cellular!"));
  delay(5000);  // wait a few seconds to stabilize connection
  connectMQTT();
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
  //else publishMessage(m_sTopic+m_sDecoder,m_sPayload);

  clearPayload();
  //m_FonaMQTT.publish("dev/data","{ \"value\": { \"hello\" : \"world\" }}");
}

void LiveObjectsFona::addCommand(char* name, onCommandCallback cb) 
{
  m_Commands[m_nSubs] = new LiveObjects_command(name, cb);
  m_nSubs++;
}

bool LiveObjectsFona::FONAconnect() 
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

  m_Fona.setGPRSNetworkSettings(F("internet"), F(""), F(""));

  //Serial.println(F("Disabling GPRS"));
  //fona.enableGPRS(false);

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

void LiveObjectsFona::connectMQTT() 
{
  //m_FonaMQTT = new Adafruit_MQTT_FONA(&m_Fona,MQTT_BROKER, m_nPort,m_sClientID.c_str(), MQTT_USER,SECRET_LIVEOBJECTS_API_KEY.c_str());
  m_FonaMQTT.setClientID(m_sClientID);
  int8_t ret;

  // Stop if already connected.
  if (m_FonaMQTT.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = m_FonaMQTT.connect()) != 0) { // connect will return 0 for connected
    Serial.println(m_FonaMQTT.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    m_FonaMQTT.disconnect();
    delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
  if(m_nSubs>0) m_FonaMQTT.subscribeTopic(MQTT_SUBCMD);
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
    //outputDebug(ERR,"Message size is to big, aborting send command.");
    Serial.println("[ERROR] Message is to big");
    return;
  }
  serializeJson(payload,m_BufferPayload);
  Serial.println("Publishing message:");
  Serial.println(m_BufferPayload);
  m_FonaMQTT.publish(topic, m_BufferPayload);
}