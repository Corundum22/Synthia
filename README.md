# Synthia
This is a synthesizer made as a third-year electrical engineering project.

## Building
Set up espressif's [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).

Building is performed with ```idf.py build```.

## Flashing
The ESP32 DevKitC Wroom V1 board can be flashed with ```idf.py -p _PORTNAME_ -b 115200 flash```.

Replace _PORTNAME_ with the appropriate device serial port for your system.

## Monitoring Output
Monitor the serial output, which will be used for debugging, with ```idf.py -p _PORTNAME_ -b 115200 monitor```.

Again, replace _PORTNAME_ with the appropriate device serial port for your system.
