# LiveObjects device commands

Commands can be used to sent request of performing some action by device.

![diagram](img/command_diagram.png)

In this example we will use command to make Arduino onboard LED blink


## Running
first of all, be sure that you installed required libraries and generated api key mentioned in main README file, then:
1. Open "4_simple_command.ino" sketch using Arduino IDE
2. Replace ```const char SECRET_LIVEOBJECTS_API_KEY[]="...";``` in arduino_secrets.h with api key you generated .
3. Upload *4_simple_command.ino* sketch to your Arduino MKR1500 board

## Verify
**Is device is online:**<br>
If all went fine under **devices** tab on Live LiveObjects portal you should see online device identified by modem IMEI

## Send command
Now navigate to:<br>
**Devices->urn:lo:nsid:mqtt:[your_device_imei]->Commands** <br>
And click **"Add command"**<br>

Finally define command like this:
![diagram](img/command_send.png)
And after clicking "Verify" your command should be sent to device and as effect you should see Arduino onboard LED blinking 5-times

![blink](img/blinkCommand.gif)
