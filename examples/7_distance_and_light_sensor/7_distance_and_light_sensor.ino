/******************************************************************************
   INCLUDES
 ******************************************************************************/
#include "arduino_secrets.h"
#include <LiveObjects.h>
#include "ExtDevices.h"

/******************************************************************************
   USER VARIABLES
 ******************************************************************************/
uint32_t messageRate = 5000;   // stores the current data message rate in Milliseconds
unsigned long lastMessageTime = 0;   // stores the time when last data message was sent

VL6180x sensor(VL6180X_ADDRESS);
char ambient_light[20];     // ambient light level [lx]
u_int8_t distance;          // distance from obstruction [mm]

/******************************************************************************
   USER PROGRAM
 ******************************************************************************/
void setup() {
  Serial.begin(115200);

  VL6180XInit();    // initialization of VL6180X sensor

  Serial.print("\n*** Live Objects for Arduino MKR boards, revision ");
  Serial.print(SW_REVISION);
  Serial.println(" ***");
  lo.setSecurity(TLS);
  lo.begin(MQTT, TEXT, true);
  lo.connect(); // connects to the network + Live Objects
}

void loop() {
  if (millis() - lastMessageTime > messageRate) {
    // collect data periodically
    Serial.println("Sampling data");

    distance = sensor.getDistance();
    lo.addToPayload("distance", distance);               // adding 'distance' value to the current payload
    sprintf(ambient_light, "%.2f", sensor.getAmbientLight(GAIN_1));
    lo.addToPayload("ambient_light", ambient_light);     // adding 'ambient_light' value to the current payload
    
    Serial.println("Sending data to Live Objects");
    lo.sendData();                                   // send the data to Live Objects
    lastMessageTime = millis();
  }
  lo.loop();                             // don't forget to keep this in your main loop
}
