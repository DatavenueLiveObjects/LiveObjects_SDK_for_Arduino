#define DEFAULT_TX_FREQUENCY 60 // message rate by default
                                // (can be updated over the air from Live Objects platform)

#include "arduino_secrets.h"
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <MKRNB.h>

#define sw_revision "1.0.1"

NB nbAccess;
//GPRS gprs;
NBClient nbClient;
MqttClient mqttClient(nbClient);

const char mqtt_user[] = "json+device";
const char mqtt_pass[] = SECRET_LIVEOBJECTS_API_KEY;
const char mqtt_id[] = "MKRNB1500";
const char mqtt_broker[] = "liveobjects.orange-business.com";
const char mqtt_pubdata[] = "dev/data";
const char mqtt_pubcfg[] = "dev/cfg";
const char mqtt_subcfg[] = "dev/cfg/upd";

const char pin[] = SECRET_PINNUMBER;
const char apn[] = SECRET_APN;
const char apn_user[] = SECRET_APN_USER;
const char apn_pass[] = SECRET_APN_PASS;

const char* JSONdata = "{\"model\":\"github_sample_MKR\",\"value\":{\"uptime\":0}}";
const char* JSONcfg= "{\"cfg\":{\"transmit frequency (s)\":{\"t\":\"u32\",\"v\":0}}}";

uint32_t transmissionFrequency = DEFAULT_TX_FREQUENCY * 1000;
uint32_t lastTransmission = DEFAULT_TX_FREQUENCY * 1000;

uint32_t uptimeInSec = 0;

StaticJsonDocument<350> payload;

void connectionManager(bool _way);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.print("\n\n***\nUsing Arduino LTE-M with Live Objects\nversion ");
  Serial.println(sw_revision);
  Serial.println("***\n");

  mqttClient.setId(mqtt_id);
  mqttClient.setUsernamePassword(mqtt_user, mqtt_pass);
  mqttClient.onMessage(onMessageReceived);

  connectionManager(1);
  updateConfig();
}

void loop() {
  if (millis() - lastTransmission > transmissionFrequency) {
    // get data from sensors
    Serial.println("Sampling data");
    sampleData();

    // connect and send data to Live Objects
    Serial.println("Sending data to Live Objects");
    //if (nbAccess.status() != NB_READY || gprs.status() != GPRS_READY || !mqttClient.connected())
    if (nbAccess.status() != NB_READY || !mqttClient.connected())
      connectionManager(1);

    sendData();
  }

    delay (1000);
    mqttClient.poll();
}

void connectionManager(bool _way = 1) {
  switch (_way) {
    case 1:
      Serial.print("Connecting to cellular network");
      //while ((nbAccess.begin() != NB_READY) || (gprs.attachGPRS() != GPRS_READY))
      while (nbAccess.begin(pin, apn, apn_user, apn_pass) != NB_READY)
      	Serial.print(".");
      
      Serial.println("\nYou're connected to the network");
      
      Serial.print("Connecting to MQTT broker '");
      Serial.print(mqtt_broker);
      Serial.print("'");
      
      while (!mqttClient.connect(mqtt_broker))
        Serial.print(".");
      
      Serial.println("\nYou're connected to the MQTT broker");
      Serial.println();

      mqttClient.subscribe(mqtt_subcfg);
      mqttClient.poll();

      break;

    case 0:
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
      transmissionFrequency = payload[F("cfg")][F("transmit frequency (s)")][F("v")].as<int>() * 1000;
      payload[F("cfg")][F("transmit frequency (s)")][F("v")] = transmissionFrequency / 1000;
    }
  }
  else {
    payload.clear();
    deserializeJson(payload, JSONcfg);
    payload[F("cfg")][F("transmit frequency (s)")][F("v")] = transmissionFrequency / 1000;
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
  lastTransmission = millis();
}
