# Soton ID reader

Read University of Southampton ID cards. This only works with the plaintext parts at the moment.

## Circuit

Connect the Arduino to a PN532 breakout board like so:

- 5V -> VCC
- GND -> GND
- A4 -> SDA
- A5 -> SCL
- 2 -> IRQ
- 3 -> RSTO

## Dependencies
- [Adafruit PN532 library](https://github.com/adafruit/Adafruit-PN532) - this is available in the Arduino IDE's library manager
