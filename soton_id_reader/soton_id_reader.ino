/**
 * Based on the PN532 examples from Adafruit
 */

#include <Wire.h>
#include <Adafruit_PN532.h>

#define IRQ_PIN 2
#define RESET_PIN 3

Adafruit_PN532 nfc(IRQ_PIN, RESET_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero

  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for a Mifare Classic Card ...");
}

void loop() {
  bool success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0};  // Buffer to store the returned UID
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (!success && uidLength != 4) return;  // Return if timeout or not mifare classic

  Serial.println("Found a Mifare Classic card");
  Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
  Serial.print("  UID Value: ");
  nfc.PrintHex(uid, uidLength);
  Serial.println();
}
