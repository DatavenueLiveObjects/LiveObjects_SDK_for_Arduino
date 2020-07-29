/******************************************************************************
   INCLUDES
 ******************************************************************************/
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

  // An example of a command callback function handling 3 arguments:
  // 'time ON' holds the LED ON time in milliseconds,
  // 'time OFF' holds the LED OFF time in milliseconds,
  // 'repetitions' holds the number of blinks.
  // The command will respond with the total duration of the animation.

void blinkLED(const String arguments, String &response) {
  // example of 'arguments' content: "{\"time ON\":250,\"time OFF\":750,\"repetitions\":5}"
  StaticJsonDocument<128> incomingArguments;  // creation of a JSON document called "incomingArguments" that will holds the arguments
  deserializeJson(incomingArguments, arguments);  // extraction of JSON data

  int timeOn = incomingArguments["time ON"];  // arguments are now accessible using their name
  int timeOff = incomingArguments["time OFF"];
  int reps = incomingArguments["repetitions"];
  unsigned long elaspedTime = millis();   // will keep track of time in order to compute the animation duration

  pinMode(LED_BUILTIN, OUTPUT);
  for (byte i = 0; i < reps; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(timeOn);
    digitalWrite(LED_BUILTIN, LOW);
    delay(timeOff);
  }

  elaspedTime = millis() - elaspedTime;

  StaticJsonDocument<128> outgoingResponse;  // creation of a JSON document that will hold the response
  outgoingResponse["animation duration (milliseconds)"] = elaspedTime;  // adding reponse item (you can add several by repeating the line):
  serializeJson(outgoingResponse, response);  // exporting JSON in 'reponse' String
  // example of 'response' content: "{\"animation duration (milliseconds)\":5000}"
}


void setup() {
  Serial.begin(115200);
  Serial.print("\n*** Live Objects on Arduino MKR NB 1500, revision ");
  Serial.print(SW_REVISION);
  Serial.println("***");

  // Declaring a simple commands hadled by the function 'blinkLED'.
  lo.addCommand("blink", blinkLED);
  lo.begin(MQTT, TLS, true);
  lo.connect();                          // connects to the network + Live Objects
}

void loop() {
  if (millis() - lastMessageTime > messageRate) {
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
