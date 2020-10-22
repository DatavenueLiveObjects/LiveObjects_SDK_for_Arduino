#include "LiveObjectsAVRFona.h"

LiveObjectsFona::LiveObjectsFona() 
  :
   m_Fona(FONA_RST)
   ,m_FonaSerial(FONA_TX,FONA_RX)
   ,m_FonaMQTT(nullptr)
   ,m_sClientID("Fona")
   ,m_nPort(1883)
{
  
}

void LiveObjectsFona::begin(Protocol p, Encoding e, bool d) 
{
  
}

void LiveObjectsFona::loop() 
{
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = m_FonaMQTT->readSubscription(5000))) {
    //if (subscription == &onoffbutton) {
    //  Serial.print(F("Got: "));
    //  Serial.println((char *)onoffbutton.lastread);
    //}
  }
}

void LiveObjectsFona::connect() 
{
  while (! FONAconnect()) {
    Serial.println("Retrying FONA");
  }
  Serial.println(F("Connected to Cellular!"));
  delay(5000);  // wait a few seconds to stabilize connection
  connectMQTT();
}

void LiveObjectsFona::addToPayload(String x, int y) 
{
  
}

void LiveObjectsFona::sendData() 
{
  m_FonaMQTT->publish("dev/data","{ \"value\": { \"hello\" : \"world\" }}");
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
  m_FonaMQTT = new Adafruit_MQTT_FONA(&m_Fona,MQTT_BROKER, m_nPort,m_sClientID.c_str(), MQTT_USER,SECRET_LIVEOBJECTS_API_KEY.c_str());
  int8_t ret;

  // Stop if already connected.
  if (m_FonaMQTT->connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = m_FonaMQTT->connect()) != 0) { // connect will return 0 for connected
    Serial.println(m_FonaMQTT->connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    m_FonaMQTT->disconnect();
    delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}