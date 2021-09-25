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
  Serial.println();
  delay(500);

  bool success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0};  // Buffer to store the returned UID
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (!success && uidLength != 4) return;  // Return if timeout or not mifare classic

  // Print UID
  Serial.println("Found a Mifare Classic card");
  Serial.print("  UID: ");
  printUID(uid);

  // Print student ID
  Serial.println("Authenticating block 24 (sector 6) with key A");
  uint8_t keya[6] = {0x51, 0xA0, 0x12, 0x10, 0xC7, 0x0A};
  uint8_t blockData[16];
  success = readBlock(24, 0, keya, uid, blockData);
  
  if (!success) {
    Serial.println("Reading student ID block failed.");
    return;
  }
  // ID block starts with "U4", then 8-digit ID, then nulls until the end
  Serial.print("  Student ID: ");
  printBlock(blockData, 2, 10);
  Serial.println();

  // Print name
  Serial.println("Authenticating name blocks with key A");

  uint8_t nameKeys[3][6] = {
    {0x4E, 0x06, 0xC8, 0x81, 0x43, 0xBC},  // Sector 1
    {0x0D, 0xCE, 0x02, 0x41, 0xA3, 0x0E},  // Sector 2
    {0xC5, 0x04, 0x92, 0xA0, 0x11, 0x4C}   // Sector 3
  };

  Serial.print("  Name: ");
  for (uint8_t block = 4; block <= 14; block++) {
    if ((block+1) % 4 == 0) continue;  // Skip sector trailers
    success = readBlock(block, 0, nameKeys[block / 4 - 1], uid, blockData);

    if (!success) {
      Serial.print("Reading block failed: ");
      Serial.println(block);
      return;
    }

    // 9 bytes of something, then 135 bytes of names separated by asterisks
    if (block == 4) printBlock(blockData, 9, 16);
    else printBlock(blockData, 0, 16);
  }
  Serial.println();
}

/*!
 * Tries to read a block from the card into a string buffer.
 * 
 * @param blockNumber   The block number to authenticate. 0-255 for 4KB Soton ID cards
 * @param keyNumber     Which key to use during authentication. 0 = A, 1 = B
 * @param keyData       Pointer to a byte array containing the 6 byte key
 * @param uid           Pointer to a byte array containing the card's UID
 * @param blockData     Pointer to a byte array to contain the data from the block
 */
bool readBlock(
  uint8_t blockNumber,
  uint8_t keyNumber,
  uint8_t *keyData,
  uint8_t *uid,
  uint8_t *blockData) {
  
  bool success;
  success = nfc.mifareclassic_AuthenticateBlock(uid, 4, blockNumber, keyNumber, keyData);
  
  if (!success) {
    Serial.println("Authentication failed.");
    return success;
  }
  
  success = nfc.mifareclassic_ReadDataBlock(blockNumber, blockData);
  if (!success) {
    Serial.println("Reading block failed.");
  }
  return success;
}

/*!
 * Writes an array of integers to the console.
 * 
 * @param block      The block to print
 * @param startByte  The byte to print from, inclusive
 * @param endByte    The byte ot print to, exclusive
 */
void printBlock(const uint8_t *block, uint8_t startByte, uint8_t endByte) {
  for (uint8_t i = startByte; i < endByte; i++) Serial.write(block[i]);
}

void printUID(const byte *uid) {
  for (uint8_t i = 0; i < 4; i++) {
    if (uid[i] <= 0xf) Serial.print("0");
    Serial.print(uid[i] & 0xff, HEX);
  }
  Serial.println();
}
