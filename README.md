ABOUT THIS PROJECT

This project runs on ESP8266 and ESP32 and connnects to WiFi and Viessmann Heaters using KM BUS (https://github.com/openv/openv/wiki/KM-Bus) over TTL<->M-BUS adapter (https://www.aliexpress.com/item/32751482255.html?spm=a2g0o.productlist.0.0.43744c91mj7fO8&algo_pvid=92261257-59d5-45e2-b1d3-575c4333a3b1&algo_expid=92261257-59d5-45e2-b1d3-575c4333a3b1-0&btsid=0bb0622a16021520273081710ef490&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)


# FIRMWARE INSTALL INSTRUCTIONS

1. Check out this repository
2. Download and install PlatformIO
   https://docs.platformio.org/en/latest/core/installation/index.html
3. Go to the software folder of this repository

   $ cd software

4. Compile and Upload
   a. ESP8266 (default Wemos d1 mini lite)
   $ pio run -e ESP8266 -t upload
   b. ESP32 (default Wemos d1 mini 32)
   $ pio run -e ESP32 -t upload
   other devices can be configured in platformio.ini
   more information @ https://docs.platformio.org/en/latest/projectconf/index.html

5. (optional) Check log output

   $ pio device monitor

6. Configure the device (if not done with configuration.h)
   The device, if unconfigured, will open a Hotspot (WiFiVitotrol-XXXXXXXXXX)
   Connect to the hotspot and navigate to http://192.168.4.1/wifiMgr/configure
   Enter SSID, Password, Hostname, MQTT Host Username and Password (optional)
   Click submit

7. Open an issue with the error and hope for help, because things never work out the way you expect it

    https://github.com/dumpfheimer/WiFiVitotrol/issues

# PHYSICAL SETUP (WIP)
I will not be responsible for any damage or other consequences

DANGER OF LIFE
Opening up your heater is DANGEROUS. Always COMPLETELY POWER OFF the device before opening up the heater.
If you are not 110% sure you know what you're doing get help by a professional.

1. Wire up the ESP to the TTL following the schema in WIRING
2. Connect MB_A and MB_D to the Terminals 145 of you Heater (Double and Triple check that this is the right Terminal on your Heater too)
3. Configure your Heater (Details will follow)


# WIRING
## ESP8266
WARNING: ESP8266 is not tested by me

TTL <-> ESP
---
VIN <-> 3.3V
GND <-> GND
RXD <-> D4
TXD <-> D5
default config: D4 (RX), D5 (TX)

## ESP32
TTL <-> ESP
---
VIN <-> 3.3V
GND <-> GND
RXD <-> 17
TXD <-> 16