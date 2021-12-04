# Dynamic Tree Lights
Custom tree lights that react to changes in the environment lighting. Once a change is detected on an individual tree light it emits a white flash after a short delay followed by three more of random duration in random colors. These can also trigger nearby tree lights, so the flash propagates through the Christmas tree.

This project was inspired by CONSTELLACTION, an installation created by panGenerator for Copernicus Science Centre / Przemiany Festiwal 2013 (https://vimeo.com/76479685).

The PCBs are built around an Attiny84 microcontroller and require an ICSP to be programmed. In my case I used an Arduino Nano with the sketch ArduinoISP.ino that can be found on the examples.
