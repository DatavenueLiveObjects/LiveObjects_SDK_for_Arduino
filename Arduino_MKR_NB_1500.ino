/******************************************************************************
   INCLUDES
 ******************************************************************************/

#include <MKRNB.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"
#include "LiveObjects.h"

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

/******************************************************************************
   USER VARIABLES
 ******************************************************************************/

uint32_t messageRate = DEFAULT_MESSAGE_RATE;   // stores the current data message rate in s
uint32_t messageRateInMs = messageRate * 1000;   // stores the current data message rate in ms
unsigned long uptimeInSec;   // stores the device uptime in seconds (sent as sensor data)
unsigned long lastMessageTime = 0;   // stores the time when last data message was sent


/******************************************************************************
   USER PROGRAM
 ******************************************************************************/

  // an example of simple command callback function
void blinkLED(const String arguments, String &response) {
  pinMode(LED_BUILTIN, OUTPUT);
  for (byte i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }
  response = "{\"blinked\":\"5 times\"}";
}

  // an example of command callback function with arguments and response
void playTone(const String arguments, String &response) {
  // 'arguments' is containing "{\"frequency\":440,\"duration\":2000}"
  StaticJsonDocument<128> incomingArguments;  // creation of a JSON document called "incomingArguments" that will holds the arguments
  deserializeJson(incomingArguments, arguments);  // extraction of JSON data
  int toneFrequency = incomingArguments["frequency"];  // data are now accessible using brackets
  int toneDuration = incomingArguments["duration"];
  // play tone
  StaticJsonDocument<128> outgoingResponse;  // creation of a JSON document that will holds the response
  outgoingResponse["I played"] = "the tone";  // adding reponse item (you can add several by repeating the line):
  outgoingResponse["time played"] = 1;
  serializeJson(outgoingResponse, response);  // exporting JSON in 'reponse' String
  // 'response' now holds "{\"I played\":\"the tone\", \"time played\": 1}"
}

  // an example of simple parameter callback function
  // this function is called after the messageRate parameter has been updated OTA
void processMsgRate() {
  messageRateInMs = messageRate * 1000;
}


void setup() {
  Serial.begin(115200);
  Serial.print("\n*** Live Objects on Arduino MKR 1500NB, revision ");
  Serial.print(SW_REVISION);
  Serial.println("***");

  addParameter("message rate (s)", messageRate, processMsgRate);   // declaring a parameter with a callback function
  addCommand("blink", blinkLED);                  // declaring 2 commands
  addCommand("play tone", playTone);

  LiveObjects_connect();                          // connects to the network + Live Objects
}

void loop() {
  if (millis() - lastMessageTime > messageRateInMs) {
    // collect data periodically
    Serial.println("Sampling data");
    uptimeInSec = millis() / 1000;
    addToPayload("uptime", uptimeInSec);          // adding 'uptime' value to the current payload

    Serial.println("Sending data to Live Objects");
    sendData();                                   // send the data to Live Objects
    lastMessageTime = millis();
  }

  LiveObjects_loop();                             // don't forget to keep this in your main loop
}
