/******************************************************************************
   INCLUDES
 ******************************************************************************/
#include "arduino_secrets.h"
#include <LiveObjects.h>
/******************************************************************************
   USER VARIABLES
 ******************************************************************************/
uint32_t messageRate = 5000;   // stores the current data message rate in Milliseconds
unsigned long uptime;   // stores the device uptime (sent as fake sensor data)
unsigned long lastMessageTime = 0;   // stores the time when last data message was sent

/******************************************************************************
   USER PROGRAM
 ******************************************************************************/
void setup() {
  delay(2000);
  Serial.begin(9600);
  Serial.print("\n*** Live Objects SDK for Arduino, revision ");
  Serial.print(SW_REVISION);
  Serial.println(" ***\n");
  lo.begin(SMS, TEXT, true);
  lo.connect(); // connects to the network + Live Objects
}

void loop() {
  if (millis() - lastMessageTime > messageRate) {
    // collect data periodically
    Serial.println("Sampling data");
    uptime = millis();
    lo.addToPayload("uptime", uptime);               // adding 'uptime' value to the current payload
    lo.addTimestamp(1596283200);                     // adding timestamp to the current payload
    Serial.println("Sending data to Live Objects");
    lo.sendData();                                   // send the data to Live Objects
    lastMessageTime = millis();
  }
  lo.loop();                             // don't forget to keep this in your main loop
}