# Live Objects parameters

Parameters give you ability to configure your device over the air from Live Objects. In this example we will use a parameter to adjust time rate on which messages from device are sent.

![diagram](img/parameter_diagram.png)

## Running
First of all, be sure that you installed the required libraries and generated an API key mentioned in the main README file, then:
1. Open "1_send_data.ino" sketch using Arduino IDE
2. Replace ```const char SECRET_LIVEOBJECTS_API_KEY[]="...";``` in arduino_secrets.h with API key you generated
3. Upload *2_simple_parameters.ino* sketch to your Arduino MKR NB 1500 board


## Verify
**Is device online:**<br>
If all went fine under **devices** tab on Live Live Objects portal you should see online your device identified by its modem IMEI:

**Is device sending data:**<br>
Under data tab on Live Objects portal you should see messages sent by your device, along with values *{ "uptime": xxxxx }*

## Update parameter
Now you can adjust the rate on which messages are sent from Live Objects (default is 60000ms). To do this navigate to:<br>
**Devices->urn:lo:nsid:mqtt:[your_device_imei]->Parameters** <br>
You should see a parameter named "message rate (miliseconds)" there. Feel free to modify the parameter: click on the parameter's name, modify the value and hit **Update**. Don't forget to click on the **Send changes** button to send the new value to your device.

![parameter](img/parameter.png)
