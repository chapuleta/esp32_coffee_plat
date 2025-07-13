# QR Code Invertido - Preto no Branco

## 🎨 Alterações Realizadas

Invertido as cores do QR Code para exibir **QR Code preto em fundo branco** em vez do padrão anterior (QR Code branco em fundo preto).

## 🔧 Mudanças no Código

### ✅ Fundo Branco
```cpp
// ANTES: Tela preta (padrão)
display.clearDisplay();

// DEPOIS: Tela branca
display.clearDisplay();
display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
```

### ✅ QR Code Preto
```cpp
// ANTES: Módulos pretos = pixels brancos
display.drawPixel(screenX, screenY, SSD1306_WHITE);

// DEPOIS: Módulos pretos = pixels pretos
display.drawPixel(screenX, screenY, SSD1306_BLACK);
```

## 📱 Resultado Visual

### Antes:
- **Fundo**: Preto
- **QR Code**: Branco
- **Estilo**: Negativo

### Depois:
- **Fundo**: Branco ⭐
- **QR Code**: Preto ⭐
- **Estilo**: Positivo (padrão)

## 🎯 Benefícios

1. **Padrão Universal**: QR Codes tradicionalmente são pretos em fundo branco
2. **Melhor Legibilidade**: Contraste mais familiar para apps de leitura
3. **Compatibilidade**: Melhor reconhecimento por diferentes leitores
4. **Visual Profissional**: Aparência mais padrão e comercial

## ✅ Status

- **Compilação**: ✅ Sucesso
- **Tamanho**: 1.207.401 bytes (92.1% da Flash)
- **RAM**: 52.564 bytes (16.0%)
- **Funcionalidade**: QR Code preto em fundo branco

## 🚀 Como Testar

1. **Upload do firmware**:
   ```bash
   pio run --target upload
   ```

2. **Abrir Serial Monitor** (115200 baud)

3. **Criar pagamento**:
   - Digite `9` para teste de R$ 0,01
   - Ou digite `1` para doação normal

4. **Visualizar QR Code**:
   - Digite `R` ou `S` para exibir QR Code
   - Agora aparecerá **preto no branco**!

## 📋 Comandos Disponíveis

- `R` - QR Code REAL (biblioteca QRCodeGen) - **PRETO no BRANCO**
- `S` - QR Code REAL (tela completa) - **PRETO no BRANCO**  
- `T` - QR Code PNG (se disponível)

---

**✅ QR Code agora é PRETO em FUNDO BRANCO como solicitado!** 🎉
