ABOUT THIS PROJECT

This project runs on ESP8266 and ESP32 and connnects to WiFi and Viessmann Heaters using KM BUS (https://github.com/openv/openv/wiki/KM-Bus) over TTL<->M-BUS adapter (https://www.aliexpress.com/item/32751482255.html?spm=a2g0o.productlist.0.0.43744c91mj7fO8&algo_pvid=92261257-59d5-45e2-b1d3-575c4333a3b1&algo_expid=92261257-59d5-45e2-b1d3-575c4333a3b1-0&btsid=0bb0622a16021520273081710ef490&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)


# INSTALL INSTRUCTIONS

1. Check out this repository
2. Download and install PlatformIO
	https://docs.platformio.org/en/latest/core/installation/index.html
3. Go to the software folder of this repository
    
    $ cd software

4. copy configuration.h.example to configuration.h and change the credentials to your desire

5. Compile

    $ pio run

6. Upload to device

    $ pio run -t upload

7. Check log output

    $ pio device monitor

8. Open an issue with the error and hope for help, because things never work out the way you expect it

    https://github.com/dumpfheimer/WiFiVitotrol/issues
