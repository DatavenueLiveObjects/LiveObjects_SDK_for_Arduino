# Send data to Live Objects thanks to IoT SAFE

This example shows how to send some sample data (device uptime) to Live Objects using Arduino MKR NB 1500.
![diagram](img/send_data_diagram.png)
This example uses the IoT SAFE APDU library to establish a MQTTS connection with a client certificate and a private key securely stored in the (e)SIM.
[2-way authentication](https://liveobjects.orange-business.com/doc/html/lo_manual_v2.html#_go_further_and_use_2_way_authentication) must have been enabled for your API key.
The client certificate embedded in the IoT SAFE applet inside the (e)SIM must have been signed with one of the Certificate Authority associated with your API key.

## Running
First of all, be sure that you installed the required libraries and generated an API key mentioned in the main README file, then:
1. Open "7_send_data.ino" sketch using Arduino IDE
2. Replace ```const char SECRET_LIVEOBJECTS_API_KEY[]="...";``` in arduino_secrets.h with API key you generated
3. Replace ```const char IOT_SAFE_CUSTOM_AID[]="...";``` in arduino_secrets.h with the Application ID of the IoT SAFE applet
4. Replace ```const char IOT_SAFE_CLIENT_CERTIFICATE_FILE_ID[]="...";``` in arduino_secrets.h with the ID of the client certificate file you saved in the IoT SAFE applet.
5. Replace ```const char IOT_SAFE_PRIVATE_KEY_ID[]="...";``` in arduino_secrets.h with the ID of the private key you saved in the IoT SAFE applet.
6. Upload *7_send_data.ino* sketch to your Arduino MKR NB 1500 board


## Verify
**Is device online:**<br>
If all went fine under **devices** tab on Live Live Objects portal you should see online your device identified by its modem IMEI:

![device_online](img/device_online.png)

**Is device sending data:**<br>
Under data tab on Live Objects portal you should see messages sent by your device, along with values *{ "uptime": xxxxx }*

![data_portal](img/data_portal.png)
