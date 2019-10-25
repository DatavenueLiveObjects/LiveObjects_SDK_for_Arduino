# Using Arduino LTE-M with Live Objects

### Code sample for using [**Arduino MKR1500 NB**](https://store.arduino.cc/mkr-nb-1500) with [**Live Objects**](https://liveobjects.orange-business.com), based on MQTT over LTE-M connection

## External libraries ##
This code sample uses several external libraries: don't forget to download/install them before using the code.
They can be found using the Library manager of the Arduino IDE.

#### Libraries provided by Arduino
- [MKRNB.h](https://www.arduino.cc/en/Reference/MKRNB) in order to handle the LTE-M module on the **Arduino MKR1500 NB**
- [ArduinoMqttClient.h](https://github.com/arduino-libraries/ArduinoMqttClient) that implements a MQTT client for Arduino (beta stage)

#### Library developed by Benoît Blanchon
- [ArduinoJson](https://arduinojson.org/), a powerful library used to store, handle and parse JSON

## How to use ##

1. Log in to [Live Objects](https://liveobjects.orange-business.com) or request a [trial account](https://liveobjects.orange-business.com/#/request_account) (up to 10 devices for 1 year) if you don't have one.
2. Create an [API key](https://liveobjects.orange-business.com/#/config/apikeys) for your device (*Device access* role), and paste it in the [arduino_secrets.h](./arduino_secrets.h) file.
3. Make sure you installed the tools for the MKR 1500 NB (using the board manager of the Arduino IDE), plus all of the above libraries.
4. Compile and upload the code!

The code sends the uptime at the default rate of 60s.
This rate can be adjusted by modifying the parameter directly on Live Objects (see [user guide](https://liveobjects.orange-business.com/cms/app/uploads/EN_User-guide-Live-Objects-7.pdf#%5B%7B%22num%22%3A115%2C%22gen%22%3A0%7D%2C%7B%22name%22%3A%22XYZ%22%7D%2C68%2C355%2C0%5D), §2.4.8.4).

Feel free to adapt the code, like adding sensors, more parameters, commands, etc.
