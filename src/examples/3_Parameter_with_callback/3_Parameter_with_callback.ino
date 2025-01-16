/******************************************************************************
   INCLUDES
 ******************************************************************************/
#include "arduino_secrets.h"
#include <LiveObjects.h>

/******************************************************************************
   USER VARIABLES
 ******************************************************************************/

uint32_t messageRate = 60;   // stores the current data message rate in Seconds
uint32_t messageRateInMs = messageRate * 1000;   // stores the current data message rate in Milliseconds
unsigned long uptime;   // stores the device uptime (sent as fake sensor data)
unsigned long lastMessageTime = 0;   // stores the time when last data message was sent


/******************************************************************************
   USER PROGRAM
 ******************************************************************************/

  // an example of simple parameter callback function:
  // this function is called after the messageRate parameter has been updated over the air
  // and transforms the message rate from Seconds to Milliseconds, which is the native time measurement on Arduino
void processMsgRate() {
  messageRateInMs = messageRate * 1000;
}


void setup() {
  delay(2000);
  Serial.begin(9600);
  Serial.print("\n*** Live Objects SDK for Arduino, revision ");
  Serial.print(SW_REVISION);
  Serial.println(" ***\n");

  // Declaring a parameter with a callback function 'processMsgRate'.
  // This function will be called after the update of the variable 'messageRate'.
  // This parameter will become available for modification over the air from Live Objects
  // upon the first connection: go to Devices > your device > Parameters
  // Note that parameters are reset upon restart.
  lo.addParameter("message rate (seconds)", messageRate, processMsgRate); // connects to the network + Live Objects
  lo.setSecurity(TLS);
  lo.begin(MQTT, TEXT, true);
  lo.connect();
}

void loop() {
  if (millis() - lastMessageTime > messageRateInMs) {
    // collect data periodically
    Serial.println("Sampling data");
    uptime = millis();
    lo.addToPayload("uptime", uptime);               // adding 'uptime' value to the current payload
    Serial.println("Sending data to Live Objects");
    lo.sendData();                                   // send the data to Live Objects
    lastMessageTime = millis();
  }

  lo.loop();                             // don't forget to keep this in your main loop
}
