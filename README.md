# ESP_Onboarding

This is an onboarding library for the ESP8266 Arduino environment.

# Overview

The aim of this library is to avoid hard coding Wifi credentials in my ESP projects.

# Usage

* Wipe the flash of your ESP8266.

```
esptool.py --port /dev/tty.usbserial-A60409VU erase_flash
```

* Open up the `ESP_OnboardingBasic.ino` example and write that onto the ESP device using the Arduino IDE.

* Connect to the console of the device using the serial monitor in the Arduino IDE and grab the value of the Access Token printed to the serial terminal. Note this uses `115200` as the baud speed.

* Connect your laptop to the AP of the ESP device

* Use curl to configure the wifi credentials for your network replacing xxxx with the value of the access token, and configuring the ssid and password for your network.

```
curl -v -H "Authorization: Token xxxx" 'http://192.168.4.1:8000/wifiSetup?ssid=xxx&pass=xxx'
```

Once configured these are stored in a config.json file in the ESP device.

# Links

* [ESP8266 Arduino environment](https://github.com/esp8266/Arduino)
* [esptool.py](https://github.com/themadinventor/esptool)

# TODO

* Random generated tokens
* Challenge
* SSL

# License

This code is released under the LGPL license see the LICENSE.md file for more details.
