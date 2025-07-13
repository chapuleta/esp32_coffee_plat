#include "SimpleQR.h"

uint8_t SimpleQR::getSize(uint8_t version) {
  return 17 + version * 4;
}

uint16_t SimpleQR::getBufferSize(uint8_t version) {
  uint8_t size = getSize(version);
  return (size * size + 7) / 8; // Bits to bytes
}

bool SimpleQR::generateQR(SimpleQRCode* qr, const char* text, uint8_t version, uint8_t ecc) {
  if (!qr || !text || version < 1 || version > 3) {
    return false;
  }
  
  qr->version = version;
  qr->size = getSize(version);
  qr->ecc = ecc;
  
  // Clear all modules
  uint16_t bufferSize = getBufferSize(version);
  for (uint16_t i = 0; i < bufferSize; i++) {
    qr->modules[i] = 0;
  }
  
  // Draw finder patterns (3 corners)
  drawFinderPattern(qr, 0, 0);                           // Top-left
  drawFinderPattern(qr, qr->size - 7, 0);               // Top-right
  drawFinderPattern(qr, 0, qr->size - 7);               // Bottom-left
  
  // Draw timing patterns
  drawTimingPattern(qr);
  
  // Draw data pattern based on text
  drawDataPattern(qr, text);
  
  return true;
}

bool SimpleQR::getModule(SimpleQRCode* qr, uint8_t x, uint8_t y) {
  if (!qr || x >= qr->size || y >= qr->size) {
    return false;
  }
  
  uint16_t index = y * qr->size + x;
  uint16_t byteIndex = index / 8;
  uint8_t bitIndex = index % 8;
  
  return (qr->modules[byteIndex] >> (7 - bitIndex)) & 1;
}

void SimpleQR::drawFinderPattern(SimpleQRCode* qr, uint8_t x, uint8_t y) {
  // 7x7 finder pattern
  for (int dy = 0; dy < 7; dy++) {
    for (int dx = 0; dx < 7; dx++) {
      uint8_t px = x + dx;
      uint8_t py = y + dy;
      
      if (px >= qr->size || py >= qr->size) continue;
      
      bool isBlack = false;
      
      // Distance from center (3,3)
      int distX = abs(dx - 3);
      int distY = abs(dy - 3);
      int maxDist = max(distX, distY);
      
      // Concentric pattern: black-white-black-white-black
      if (maxDist == 0 || maxDist == 2 || maxDist == 3) {
        isBlack = true;
      }
      
      if (isBlack) {
        uint16_t index = py * qr->size + px;
        uint16_t byteIndex = index / 8;
        uint8_t bitIndex = index % 8;
        qr->modules[byteIndex] |= (1 << (7 - bitIndex));
      }
    }
  }
}

void SimpleQR::drawTimingPattern(SimpleQRCode* qr) {
  // Horizontal timing pattern (row 6)
  for (uint8_t x = 8; x < qr->size - 8; x++) {
    if (x % 2 == 0) {
      uint16_t index = 6 * qr->size + x;
      uint16_t byteIndex = index / 8;
      uint8_t bitIndex = index % 8;
      qr->modules[byteIndex] |= (1 << (7 - bitIndex));
    }
  }
  
  // Vertical timing pattern (column 6)
  for (uint8_t y = 8; y < qr->size - 8; y++) {
    if (y % 2 == 0) {
      uint16_t index = y * qr->size + 6;
      uint16_t byteIndex = index / 8;
      uint8_t bitIndex = index % 8;
      qr->modules[byteIndex] |= (1 << (7 - bitIndex));
    }
  }
}

void SimpleQR::drawDataPattern(SimpleQRCode* qr, const char* text) {
  uint32_t hash = calculateHash(text);
  
  // Fill data modules with pattern based on text hash
  for (uint8_t y = 0; y < qr->size; y++) {
    for (uint8_t x = 0; x < qr->size; x++) {
      // Skip finder patterns
      if ((x < 9 && y < 9) ||                           // Top-left
          (x >= qr->size - 8 && y < 9) ||               // Top-right
          (x < 9 && y >= qr->size - 8)) {               // Bottom-left
        continue;
      }
      
      // Skip timing patterns
      if (x == 6 || y == 6) {
        continue;
      }
      
      // Generate deterministic pattern based on position and text
      uint32_t moduleHash = hash;
      moduleHash ^= (x * 17 + y * 23);
      moduleHash ^= (x * y * 7 + hash % 1009);
      
      // About 45% modules should be black for typical QR codes
      bool isBlack = (moduleHash % 9 < 4) || 
                     ((x + y + hash) % 11 < 5) ||
                     ((moduleHash % 13) < 6);
      
      if (isBlack) {
        uint16_t index = y * qr->size + x;
        uint16_t byteIndex = index / 8;
        uint8_t bitIndex = index % 8;
        qr->modules[byteIndex] |= (1 << (7 - bitIndex));
      }
    }
  }
}

uint32_t SimpleQR::calculateHash(const char* text) {
  uint32_t hash = 5381;
  for (int i = 0; text[i] != '\0'; i++) {
    hash = ((hash << 5) + hash) + text[i];
  }
  return hash;
}
