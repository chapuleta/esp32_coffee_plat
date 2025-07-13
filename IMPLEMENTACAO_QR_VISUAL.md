# Implementação de QR Code Visual - Plano Detalhado

## Objetivo
Implementar exibição de QR Code como imagem bitmap na tela OLED 128x64 do ESP32, mantendo estabilidade e evitando crashes.

## Estratégia Recomendada: Conversão Offline

### Passo 1: Geração do QR Code como PNG
```cpp
// No ESP32: gerar QR Code apenas como string (como já implementado)
String qrCodeData = response.substring(startPos, endPos);

// Enviar dados via Serial para processamento externo
Serial.println("QR_DATA:" + qrCodeData);
```

### Passo 2: Script Python para Conversão
```python
#!/usr/bin/env python3
# Arquivo: tools/qr_to_bitmap.py
import qrcode
import sys
from PIL import Image

def generate_qr_bitmap(data, size=64):
    # Gerar QR Code
    qr = qrcode.QRCode(
        version=1,
        error_correction=qrcode.constants.ERROR_CORRECT_L,
        box_size=1,
        border=1,
    )
    qr.add_data(data)
    qr.make(fit=True)
    
    # Criar imagem
    img = qr.make_image(fill_color="black", back_color="white")
    img = img.resize((size, size), Image.NEAREST)
    
    # Converter para bitmap C++
    print("#define QR_WIDTH", size)
    print("#define QR_HEIGHT", size)
    print("const uint8_t PROGMEM qr_bitmap[] = {")
    
    for y in range(size):
        print("  ", end='')
        for x in range(0, size, 8):
            byte_val = 0
            for bit in range(8):
                if x + bit < size:
                    pixel = img.getpixel((x + bit, y))
                    if pixel == 0:  # Preto
                        byte_val |= (1 << (7 - bit))
            print(f"0x{byte_val:02X},", end=' ')
        print()
    print("};")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        generate_qr_bitmap(sys.argv[1])
```

### Passo 3: Integração no Código ESP32
```cpp
// Arquivo: include/qr_bitmaps.h
#ifndef QR_BITMAPS_H
#define QR_BITMAPS_H

#include <pgmspace.h>

// QR Code para valores comuns (pré-gerados)
extern const uint8_t PROGMEM qr_bitmap_5[];
extern const uint8_t PROGMEM qr_bitmap_10[];
extern const uint8_t PROGMEM qr_bitmap_20[];
// ... outros valores

#define QR_WIDTH 64
#define QR_HEIGHT 64

// Função para obter bitmap baseado no valor
const uint8_t* getQRBitmap(float valor);

#endif
```

### Passo 4: Atualização da Função mostrarQRCode()
```cpp
void mostrarQRCode() {
  display.clearDisplay();
  
  // Tentar obter bitmap pré-gerado
  const uint8_t* qr_bitmap = getQRBitmap(valorDoacao);
  
  if (qr_bitmap != nullptr) {
    // Exibir QR Code visual
    display.drawBitmap(32, 0, qr_bitmap, QR_WIDTH, QR_HEIGHT, SSD1306_WHITE);
    display.display();
  } else {
    // Fallback para interface textual
    display.setCursor(0,0);
    display.println("=== PIX GERADO ===");
    display.print("Valor: R$ ");
    display.println(valorDoacao, 2);
    display.println("Escaneie com app banco");
    display.display();
  }
}
```

## Vantagens desta Abordagem

✅ **Estabilidade**: Sem processamento pesado em runtime
✅ **Memória Controlada**: Bitmaps pré-calculados em PROGMEM
✅ **Performance**: Exibição instantânea
✅ **Compatibilidade**: Funciona com hardware existente
✅ **Fallback**: Interface textual como backup

## Limitações

⚠️ **QR Codes Fixos**: Apenas para valores pré-definidos
⚠️ **Memória Flash**: Cada bitmap ocupa ~512 bytes
⚠️ **Flexibilidade**: QR dinâmicos requerem conversão externa

## Próximos Passos

1. Criar script Python de conversão
2. Gerar bitmaps para valores comuns (R$ 5, 10, 20, 50, 100)
3. Implementar seleção de bitmap no código
4. Testar estabilidade
5. Documentar processo para valores personalizados

## Alternativa Futura: QR Dinâmico

Para QR Codes totalmente dinâmicos, considerar:
- ESP32 com mais RAM (ESP32-S3)
- Biblioteca `qrcode` otimizada para ESP32
- Implementação em chunks menores
- Watchdog timer configurado adequadamente

---
**Data**: $(Get-Date -Format "yyyy-MM-dd HH:mm")
**Status**: Plano Aprovado - Pronto para Implementação
