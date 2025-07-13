#ifndef SIMPLE_QR_H
#define SIMPLE_QR_H

#include <Arduino.h>

// Versões de QR Code simplificadas
#define QRCODE_VERSION_1    1  // 21x21 modules
#define QRCODE_VERSION_2    2  // 25x25 modules  
#define QRCODE_VERSION_3    3  // 29x29 modules

// Error correction levels
#define QRCODE_ECC_LOW      0
#define QRCODE_ECC_MEDIUM   1
#define QRCODE_ECC_QUARTILE 2
#define QRCODE_ECC_HIGH     3

// Estrutura do QR Code
struct SimpleQRCode {
  uint8_t version;
  uint8_t size;
  uint8_t ecc;
  uint8_t *modules;
};

class SimpleQR {
public:
  static bool generateQR(SimpleQRCode* qr, const char* text, uint8_t version, uint8_t ecc);
  static bool getModule(SimpleQRCode* qr, uint8_t x, uint8_t y);
  static uint8_t getSize(uint8_t version);
  static uint16_t getBufferSize(uint8_t version);
  
private:
  static void drawFinderPattern(SimpleQRCode* qr, uint8_t x, uint8_t y);
  static void drawTimingPattern(SimpleQRCode* qr);
  static void drawDataPattern(SimpleQRCode* qr, const char* text);
  static uint32_t calculateHash(const char* text);
};

#endif
