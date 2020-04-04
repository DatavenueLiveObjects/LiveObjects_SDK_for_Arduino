/******************************************************************************
   DEFAULT VALUES
 ******************************************************************************/

  // Set MQTT security:
  // comment the line to disable security (MQTT),
  // uncomment to activate TLS security (MQTTS).
#define MQTT_TLS

  // Set the default message transmission rate (in seconds).
  // This parameter can be updated later over the air from Live Objects.
#define DEFAULT_MESSAGE_RATE 60

#define SW_REVISION "1.5.0"

/******************************************************************************
   INCLUDES
 ******************************************************************************/

#include "arduino_secrets.h"
#if defined MQTT_TLS
  #include "LiveObjectsCert.h"
#endif
#include <MKRNB.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>

/******************************************************************************
   TYPEDEFS
 ******************************************************************************/

enum NetworkConnectionAction {
  CONNECT,
  DISCONNECT
};

/******************************************************************************
   CONSTANTS
 ******************************************************************************/

  // Live Objects information
const char mqtt_id[] = "MKRNB1500";
const char mqtt_broker[] = "liveobjects.orange-business.com";
const char mqtt_user[] = "json+device";     // MQTT username for 'device' role
#if defined MQTT_TLS
  const uint16_t mqtt_port = 8883;          // TLS MQTT port
#else
  const uint16_t mqtt_port = 1883;          // plain MQTT port
#endif
const char mqtt_pubdata[] = "dev/data";     // topic for publishing data
const char mqtt_pubcfg[] = "dev/cfg";       // subscribed topic for configuration updates
const char mqtt_subcfg[] = "dev/cfg/upd";   // topic for publishing configuration

  // Buffers holding the serialized default payloads for data and configuration
const char* JSONdata = "{\"model\":\"github_sample_MKR\",\"value\":{\"uptime\":0}}";
const char* JSONcfg = "{\"cfg\":{\"message rate (s)\":{\"t\":\"u32\",\"v\":0}}}";

/******************************************************************************
   VARIABLES
 ******************************************************************************/

uint32_t messageRate = DEFAULT_MESSAGE_RATE * 1000;   // stores the current message rate
uint32_t lastMessageTime = DEFAULT_MESSAGE_RATE * 1000;   // stores the time when last data message was sent
uint32_t uptimeInSec;   // stores the device uptime in seconds (sent in the data message)
bool DigiCert_rootCA_loaded = false;  // controls if the root CA certificate has already been stored in the SARA module

void connectionManager(NetworkConnectionAction _action);

/******************************************************************************
   OBJECTS CREATION
 ******************************************************************************/

NB nbAccess;
#if defined MQTT_TLS
  NBSSLClient nbClient;   // for TLS MQTT connection to Live Objects
#else
  NBClient nbClient;    // for plain MQTT connection to Live Objects
#endif
MqttClient mqttClient(nbClient);
StaticJsonDocument<350> payload;


void setup() {
  Serial.begin(9600);
  while (!Serial);  // uncomment this line if you want to run the sketch without monitoring the Serial line.
  Serial.print("\n***\nLive Objects on Arduino MKR 1500NB\nrevision ");
  Serial.println(SW_REVISION);
  Serial.println("***");

  if (sizeof(SECRET_LIVEOBJECTS_API_KEY) != 33) {
    Serial.print("Please check your Live Objects API key (arduino_secrets.h file).\nStopping here.");
    while (1) ;
  }

  mqttClient.setId(mqtt_id);
  mqttClient.setUsernamePassword(mqtt_user, SECRET_LIVEOBJECTS_API_KEY);
  mqttClient.onMessage(onMessageReceived);

  connectionManager(CONNECT);
  updateConfig();
}

void loop() {
  if (millis() - lastMessageTime > messageRate) {
    // get data from sensors
    Serial.println("Sampling data");
    sampleData();

    // connect and send data to Live Objects
    Serial.println("Sending data to Live Objects");
    if (nbAccess.status() != NB_READY || !mqttClient.connected())
      connectionManager(CONNECT);

    sendData();
  }

    delay (1000);
    mqttClient.poll();
}

void connectionManager(NetworkConnectionAction _action = CONNECT) {
  switch (_action) {
    case CONNECT:
      Serial.print("Connecting to cellular network");
      while (nbAccess.begin(SECRET_PINNUMBER, SECRET_APN, SECRET_APN_USER, SECRET_APN_PASS) != NB_READY)
      	Serial.print(".");
      Serial.println("\nYou're connected to the network");

      #if defined MQTT_TLS    // only performed when using TLS, in order to load the needed root CA into the SARA module
        if (!DigiCert_rootCA_loaded) {
          Serial.println("Loading DigiCert Root CA certificate");
          MODEM.sendf("AT+USECMNG=0,0,\"%s\",%d", LO_ROOT_CERT.name, LO_ROOT_CERT.size);
          if (MODEM.waitForPrompt() != 1) {
            Serial.print("Problem loading certificate!\nStopping here.");
            while (1) ;
          }
          else {
            MODEM.write(LO_ROOT_CERT.data, LO_ROOT_CERT.size);
            int ready;
            while (!MODEM.ready()) ;
            DigiCert_rootCA_loaded = true;
            Serial.println("Certificate loaded");
          }
        }
      #endif

      Serial.print("Connecting to MQTT broker '");
      Serial.print(mqtt_broker);
      Serial.print("'");
      
      while (!mqttClient.connect(mqtt_broker, mqtt_port))
        Serial.print(".");
      Serial.println("\nYou're connected to the MQTT broker");
      Serial.println();

      mqttClient.subscribe(mqtt_subcfg);
      mqttClient.poll();

      break;

    case DISCONNECT:
      Serial.println("\nClosing MQTT connection...");
      mqttClient.stop();
      Serial.println("Disconnecting from cellular network...");
      nbAccess.shutdown();
      Serial.println("Offline.\n");
      break;
  }
}

void publishMessage(const char* topic, const char* _buffer) {
  Serial.print("Publishing message on topic '");
  mqttClient.beginMessage(topic);
  mqttClient.print(_buffer);
  mqttClient.endMessage();
  Serial.print(topic);
  Serial.println("':");
  Serial.println(_buffer);
}
  
void onMessageReceived(int messageSize) {
  String topic = mqttClient.messageTopic();
  Serial.print("Received a message with topic '");
  Serial.print(topic);
  Serial.println("':");

  char _buffer[300];
  
  byte i=0;
  while (mqttClient.available())
    _buffer[i++] = (char)mqttClient.read();
  _buffer[i]=0;
  Serial.println(_buffer);
  
  payload.clear();
  deserializeJson(payload, _buffer);

  if (topic == F("dev/cfg/upd"))
    updateConfig();
  else if (topic == F("dev/cmd"))
    //command();
    ;
}

void updateConfig() {
  if (payload.containsKey(F("cid"))) {
    if (payload[F("cfg")].containsKey(F("transmit frequency (s)"))) {
      messageRate = payload[F("cfg")][F("transmit frequency (s)")][F("v")].as<int>() * 1000;
      payload[F("cfg")][F("transmit frequency (s)")][F("v")] = messageRate / 1000;
    }
  }
  else {
    payload.clear();
    deserializeJson(payload, JSONcfg);
    payload[F("cfg")][F("transmit frequency (s)")][F("v")] = messageRate / 1000;
  }

  char _buffer[300];
  serializeJson(payload, _buffer);
  publishMessage(mqtt_pubcfg, _buffer);
}

void sampleData() {
  uptimeInSec = millis()/1000;
}

void sendData() {
  payload.clear();
  deserializeJson(payload, JSONdata);
 
  payload[F("value")][F("uptime")] = uptimeInSec;
  
  char _buffer[300];
  serializeJson(payload, _buffer);
  publishMessage(mqtt_pubdata, _buffer);
  lastMessageTime = millis();
}
