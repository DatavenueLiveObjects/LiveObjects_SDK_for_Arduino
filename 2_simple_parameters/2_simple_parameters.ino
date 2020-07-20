/******************************************************************************
   DEFAULT VALUES
 ******************************************************************************/

// Set MQTT security:
// comment the line to disable security (MQTT),
// uncomment to activate TLS security (MQTTS).
#define MQTT_TLS

/******************************************************************************
   INCLUDES
 ******************************************************************************/

#include <MKRNB.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"
#include "LiveObjects.h"

/******************************************************************************
   USER VARIABLES
 ******************************************************************************/

uint32_t messageRate = 60000;   // stores the current data message rate in Milliseconds
unsigned long uptime;   // stores the device uptime (sent as fake sensor data)
unsigned long lastMessageTime = 0;   // stores the time when last data message was sent


/******************************************************************************
   USER PROGRAM
 ******************************************************************************/

void setup() {
  Serial.begin(115200);
  Serial.print("\n*** Live Objects on Arduino MKR NB 1500, revision ");
  Serial.print(SW_REVISION);
  Serial.println("***");

  // Declaring a simple parameter stored in the variable 'messageRate'.
  // This parameter will become available for modification over the air from Live Objects
  // upon the first connection: go to Devices > your device > Parameters
  // Note that parameters are reset upon restart.
  addParameter("message rate (milliseconds)", messageRate);

  LiveObjects_connect();                          // connects to the network + Live Objects
}

void loop() {
  if (millis() - lastMessageTime > messageRate) {
    // collect data periodically
    Serial.println("Sampling data");
    uptime = millis();
    addToPayload("uptime", uptime);               // adding 'uptime' value to the current payload

    Serial.println("Sending data to Live Objects");
    sendData();                                   // send the data to Live Objects
    lastMessageTime = millis();
  }

  LiveObjects_loop();                             // don't forget to keep this in your main loop
}
