# Prototype IoT with Live Objects and Arduino MKR1500 NB

### Discover the LTE-M cellular network, dedicated to IoT, using [**Live Objects**](https://liveobjects.orange-business.com) and the [**Arduino MKR1500 NB**](https://store.arduino.cc/mkr-nb-1500).

This code sample provides the minimal basis allowing you to create a LTE-M object, send data to Live Objects and configure it over the air.

## Installation ##
This code needs 3 external libraries to run, that you can install using the built-in [Library Manager](https://www.arduino.cc/en/guide/libraries) of the Arduino IDE.

#### Libraries provided by Arduino
- [MKRNB](https://www.arduino.cc/en/Reference/MKRNB) drives the LTE-M module of the Arduino MKR1500
- [ArduinoMqttClient](https://github.com/arduino-libraries/ArduinoMqttClient) implements a MQTT client for Arduino (beta stage)

#### Library developed by Benoît Blanchon
- [ArduinoJson](https://arduinojson.org/), a powerful library used to handle, store and parse JSON

#### SAMD21 Arduino core
- You also need to install the Arduino Core for Atmel's SAMD21 processor, used on the MKR family boards. Use the [Boards Manager](https://www.arduino.cc/en/guide/cores) to install the package called *Arduino SAMD Boards (32-bit ARM Cortex-M0+)*.

## How to use ##

1. Log in to [Live Objects](https://liveobjects.orange-business.com) or request a [trial account](https://liveobjects.orange-business.com/#/request_account) (up to 10 devices for 1 year, for free!) if you don't have one.
2. Create an [API key](https://liveobjects.orange-business.com/#/config/apikeys) for your device. Give it a name, select the *Device access* role and validate. Copy the key.
3. In the [arduino_secrets.h](./arduino_secrets.h) file:
   - Paste your API key as initialization value for the `SECRET_LIVEOBJECTS_API_KEY` variable -keep the double quotes!
   - Fill in the cellular credentials if needed (pin code, APN information, etc). Most of the time, APN will set up automatically. Your SIM card may have a "0000" pin code, unless you deactivated it using the [Pin management](https://github.com/arduino-libraries/MKRNB/blob/master/examples/Tools/PinManagement/PinManagement.ino) sketch, provided with the MKRNB library.
4. In the [Arduino_MKR1500NB.ino](./Arduino_MKR1500NB.ino) sketch, you can choose wheter or not using TLS security with the MQTT protocol:
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

Feel free to adapt the code, like adding sensors, more parameters, commands, etc.
