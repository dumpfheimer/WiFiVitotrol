# ABOUT THIS PROJECT

This project runs on ESP8266 (and untested ESP32) and connects to WiFi and Viessmann Heaters using Optolink (https://github.com/openv/openv/wiki/Die-Optolink-Schnittstelle) with a self-made adapter (https://github.com/openv/openv/wiki/Bauanleitung)
If you have a 3D printer check out OptolinkConnector.blend and OptolinkConnector.stl located in the hardware folder.

# INSTALL INSTRUCTIONS

1. Check out this repository
2. Download and install PlatformIO
   https://docs.platformio.org/en/latest/core/installation/index.html
3. Go to the software folder of this repository

   $ cd software

4. Compile and Upload

   $ pio run -t upload

5. (optional) Check log output

   $ pio device monitor

6. Configure the device (if not done with configuration.h)
   The device, if unconfigured, will open a Hotspot (Portal-XXXXXXXXXX)
   Connect to the hotspot and navigate to http://192.168.4.1/wifiMgr/configure
   Enter SSID, Password, Hostname, MQTT Host Username and Password (optional)
   Click submit

7. Open an issue with the error and hope for help, because things never work out the way you expect it

   https://github.com/dumpfheimer/OptoProxy/issues

# HOW TO USE OptoProxy
## General information
OptopLink communicates via addresses. To read or write information you need to know two things:
1. Which address the information is stored in
2. In which format the data is stored in (see Data types)

Refer to openv Wiki for more information: https://github.com/openv/openv/wiki/Adressen

### Data types

There are several data for converting the datapoint into useable format:

- raw
- temp
- temps
- stat
- count
- counts
- mode
- hours
- cop

## 1. Reading a datapoint

Example URL for reading Address 0x2000 (Room Temperature Setpoint)

http://<IP_OF_ESP8266>/read?addr=0x2000&conv=temp

## 2. Writing a datapoint

Example URL for writing Address 0x2000 (Room Temperature Setpoint)

http://<IP_OF_ESP8266>/write?addr=0x2000&conv=temp&val=21

# WIRING
## ESP8266
default config: D4 (RX), D5 (TX)

## ESP32
# WARNING: ESP32 is not tested by me
default config: 17 (RX), 16 (TX)