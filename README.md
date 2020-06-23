# Prototype LTE-M with Orange using Live Objects and Arduino MKR NB 1500

### Discover the LTE-M cellular network, dedicated to IoT, using [**Live Objects**](https://liveobjects.orange-business.com) and [**Arduino MKR NB 1500**](https://store.arduino.cc/mkr-nb-1500).

This example of code wraps all the functions necessary to make your object work with Live Objects. You can declare parameters, which you can later update OTA from Live objects. You can also create commands to trigger actions remotely.
The code will manage the MQTT exchanges with Live objects under the hood, and keep your parameters up to date or execute the commands received without you having to take care of them (apart from writing the code of these commands, of course).

## Installation ##
This code needs 3 external libraries to run, that you can install using the built-in [Library Manager](https://www.arduino.cc/en/guide/libraries) of the Arduino IDE.

#### Libraries provided by Arduino
- [MKRNB](https://www.arduino.cc/en/Reference/MKRNB) in order to handle the LTE-M module on **Arduino MKR NB 1500**
- [ArduinoMqttClient](https://github.com/arduino-libraries/ArduinoMqttClient) that implements a MQTT client for Arduino

#### Library developed by Benoît Blanchon
- [ArduinoJson](https://arduinojson.org/), a powerful library used to parse, store and handle JSON easily

#### SAMD21 Arduino core
- You also need to install the Arduino core for Atmel SAMD21 processor, used on the boards of the MKR family. Open the [Boards Manager](https://www.arduino.cc/en/guide/cores) and install the package called "Arduino SAMD Boards (32-bit ARM Cortex-M0+)".

## How to use ##

1. Clone or download the directory from Github.
2. Log in to [Live Objects](https://liveobjects.orange-business.com) or request a [trial account](https://liveobjects.orange-business.com/#/request_account) (up to 10 devices for 1 year) if you don't have one.
3. Create an [API key](https://liveobjects.orange-business.com/#/config/apikeys) for your device. Give it a name, select the *Device access* role and validate. Copy the key.
4. In the **'arduino_secrets.h'** file:
   - Paste it as initialization value for the `SECRET_LIVEOBJECTS_API_KEY` variable in the 'arduino_secrets.h' file -keep the double quotes!
   - Fill in the cellular credentials if needed (pin code, APN information, etc). Most of the time, APN will set up automatically. Your SIM card may have a default pin code (like "0000"), unless you deactivated it using the [Pin management](https://github.com/arduino-libraries/MKRNB/blob/master/examples/Tools/PinManagement/PinManagement.ino) sketch, provided with the MKRNB library.
5. In the [Arduino_MKR1500NB.ino](./Arduino_MKR1500NB.ino) sketch, you can choose whether or not using TLS security with the MQTT protocol:
```c++
  // Set MQTT security:
  // comment the line to disable security (MQTT),
  // uncomment to activate TLS security (MQTTS).
#define MQTT_TLS
```
You can also adjust the default message rate of 60s:
```c++
  // Set the default message transmission rate (in seconds).
  // This parameter can be updated later over the air from Live Objects.
#define DEFAULT_MESSAGE_RATE 60
```

This rate can be later adjusted over the air by modifying the parameter directly on Live Objects (see [user guide](https://liveobjects.orange-business.com/cms/app/uploads/EN_User-guide-Live-Objects-7.pdf#%5B%7B%22num%22%3A115%2C%22gen%22%3A0%7D%2C%7B%22name%22%3A%22XYZ%22%7D%2C68%2C355%2C0%5D), §2.4.8.4).

## Developer guide ##

### Declare parameters ###
You can update over the air some parameters of your sketch using Live Objects's MQTT Parameters. Parameters and Commands must be declared _before_ your device connects to Live Objects.

You can declare parameters with the `addParameter()` instruction, which accepts the following arguments:
- the label of your parameter (_const char*_ expected) as it will be displayed on Live Objects;
- the variable your parameter is stored in (the variable can be of any type **except** _char*_, _char[]_ or any custom type);
- (optional) a callback function, if you need to perform some tasks after the parameter has been updated;
- (optional) the type of parameter you want to use on Live Objects, among _INTEGER_, _UNSIGNED_INTEGER_, _BINARY_, _STRING_ or _DECIMAL_ (for float values). If omitted, the type will be automatically chosen from the variable you use.
```c++
int myParam;
...
addParameter("my parameter", myParam);
addParameter("a second parameter", 2ndParam, myCallbackFunction);
addParameter("a third parameter", 3rdParam, STRING);
addParameter("another parameter", anotherParam, anotherCallbackFunction, UNSIGNED_INTEGER);
```

The callback function does not take any arguments. It is of form
```c++
void myCallbackFunction() {
  // do stuff
}
```

Further reading on Live Objects' [MQTT parameters](https://liveobjects.orange-business.com/doc/html/lo_manual.html#_commands).

### Declare commands ###
Commands lets you trigger specific actions on your device from Live Objects. Parameters and Commands must be declared _before_ your device connects to Live Objects.

Commands can be declared using the `addcommand()` instruction, which accepts the following arguments:
- the label of your command (_const char*_ expected);
- the callback function that will execute the command.
```c++
addParameter("a command", myCallback);
```

The callback function is of form
```c++
void myCallback(const String arguments, String &response) {
  // do stuff
}
```

Arguments and response are optional when using commands, but they can be useful if you want to pass parameters to your function. For instance, you could define a `play tone` command that will use some parameters like the frequency of the tone, or its duration.
- Any incoming arguments will be passed using the `arguments` String containing a JSON object;
- You can pass response arguments in the form of a JSON objet stored in the `response` String.
```c++
void playTone(const String arguments, String &response) {
  // arguments = "{\"duration\":2000,\"frequency\":440}"
  // play the tone accordingly to arguments
  response = "{\"I played\":\"the tone\"}";
}

void setup() {
  addParameter("play tone", playTone);
}
```

> :warning: **Command name and arguments are case-sensitive when creating the command on Live Objects.** On the opposite, there is no specific order for specifying the command arguments.
![Live Object screenshot](./LiveObjects_command.png)

You may use the ArduinoJSON library, or any other library to process the JSON objects more easily.

Further reading on Live Objects' [MQTT commands](https://liveobjects.orange-business.com/doc/html/lo_manual.html#MQTT_DEV_CMD).

### Send data ###
You can send data very easily to Live Objects.

#### Dead simple method ####
Compose your payload using the `addToPayload()` instruction. You will need to provide a label (_const char*_ expected) for your value, and the data itself. You data can be of any type.

Data is added on each call to `addToPayload()`, so repeat the instruction if you have multiple data to send. When your payload is ready, send it using `sendData()`. That simple.
```c++
int value;
double myOtherValue;

void loop() {
  // collect data
  addToPayload("my data", value);
  addToPayload("my other data", myOtherValue);
  sendData();
}
```

As soon the data is send, your payload is cleared and waiting for the next sending.

#### Advanced method ####
Advanced users may want to add more specific fields to their payload, like geolocation information or custom timestamps. In that case, you can compose your own JSON payload as a string (_char*_ or _String_) and pass it to send using `sendData()`:
```c++
void loop() {
  String myPayloadString = "{\"value\":{\"uptime\":0, \"conditions\":\"good\"}}"
  sendData(myPayloadString);
  
  // or
  char* myPayloadCharArray = "{\"value\":{\"uptime\":0, \"conditions\":\"good\"}}"
  sendData(myPayloadCharArray);
}
```

### Connect, disconnect and loop ###
You can control the connection and disconnection of your device using `LiveObjects_connect()` and `LiveObjects_disconnect()`.

In order to check for any incoming configuration update or command, you need to keep the `LiveObjects_loop()` instruction in your main loop.

## Toubleshooting ##
### My payload is truncated on Live Objects ###
This can happen with large payload, because of the fixed-size JSON storage allocated for processing your payload (512 bytes by default). You can allocate more room by modifying the value in the **'LiveObjects.h'** file at line 8:
```c++
#define PAYLOAD_DATA_SIZE 512
```

### My parameters are not registered on Live Objects ###
Same reason as above, it can happen if you have a large number of parameters. You need to allocate more room by modifying the value in the **'LiveObjects.h'** file at line 7:
```c++
#define PAYLOAD_DEVMGT_SIZE 256
```

### Only my first 10 parameters are registered on Live Objects ###
By default, a maximum of 10 parameters can be managerd by the code sample. You can enable more parameters by modifying the value in the **'LiveObjects.h'** file at line 5:
```c++
#define PARAMETERS_NB_MAX 10
```

### Only my first 10 commands are working ###
By default, a maximum of 10 commands can be managerd by the code sample. You can enable more commands by modifying the value in the **'LiveObjects.h'** file at line 6:
```c++
#define COMMANDS_NB_MAX 10
```

### One of my command is not working ###
If you have many arguments in your command, first check that you get all the arguments when entering the function:
```c++
// this is your callback function
void blinkLED(const String arguments, String &response) {
  // display arguments
  Serial.print("arguments: ");
  Serial.println(arguments);
  ...
}
```
If your arguments are incomplete, try allocating more room for JSON storage by modifying the value in the **'LiveObjects.h'** file at line 7:
```c++
#define PAYLOAD_DEVMGT_SIZE 256
```
