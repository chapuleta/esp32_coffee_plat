# Sistema de Controle via Serial Monitor - ESP32 Coffee Project

## 🎮 Controle 100% Via Teclado do Computador

O projeto está configurado para controle **exclusivo via Serial Monitor** - não há botões físicos conectados ao ESP32. Todo controle é feito através do teclado do computador usando o monitor serial do Arduino IDE ou PlatformIO.

## 📋 Comandos Disponíveis

### 🏠 Menu Principal
- **`1`** - Fazer doação (vai para seleção de valor)
- **`9`** - TESTE: Criar pagamento de R$ 0,01 para testar API
- **`0`** - Voltar ao menu inicial

### 🎯 Comandos de Teste (após criar um pagamento)
- **`T`** - TESTE: Exibir QR PNG real atual
- **`S`** - TESTE: Exibir QR Code REAL (tela completa 128x64)
- **`R`** - TESTE: QR Code REAL (biblioteca QRCodeGen)
- **`8`** - TESTE: Decodificar PNG atual

### 💰 Seleção de Valor
- **`↑`** ou **`w`** - Opção anterior
- **`↓`** ou **`s`** - Próxima opção  
- **`ENTER`** ou **`e`** - Selecionar valor
- **`0`** - Voltar ao menu inicial

### 💵 Valor Personalizado
- **Digite o valor** e pressione **`ENTER`**
- **`ESC`** - Cancelar entrada

## 🔧 Configuração Técnica

### Pinos Utilizados (Apenas OLED I2C)
```cpp
#define OLED_SDA 21    // Pino SDA do display OLED
#define OLED_SCL 22    // Pino SCL do display OLED  
#define OLED_ADDRESS 0x3C  // Endereço I2C do display
```

### Não Há Pinos de Botões
✅ **Sem botões físicos** - controle 100% via Serial  
✅ **Sem pinMode(), digitalRead(), digitalWrite()**  
✅ **Sem interrupções ou polling de pinos**

## 🖥️ Como Usar

1. **Conectar ESP32** via USB ao computador
2. **Abrir Serial Monitor** (115200 baud)
3. **Digitar comandos** no teclado conforme menu
4. **Usar setas do teclado** para navegação (↑↓)
5. **Apps de pagamento** fazem leitura do QR Code na tela OLED

## 🎯 Funcionalidades

### ✅ Menu Interativo
- Interface completa no Serial Monitor
- Navegação com setas do teclado  
- Feedback visual na tela OLED

### ✅ QR Code Visual Real
- Biblioteca QRCodeGen (Nayuki) profissional
- Usa toda a tela 128x64 (sem "área azul")
- Máxima qualidade e legibilidade

### ✅ Integração Mercado Pago
- API oficial do Mercado Pago
- Pagamentos PIX reais
- Verificação automática de status

### ✅ Testes e Debug
- Múltiplos comandos de teste
- Debug via Serial Monitor
- Fallback entre métodos de QR Code

## 📱 Fluxo de Uso

```
1. ESP32 inicia → Menu Principal no Serial
2. Usuário digita "1" → Seleção de Valor
3. Usuário navega com ↑↓ → Seleciona valor
4. Sistema cria pagamento → QR Code na tela OLED
5. Cliente escaneia com app → Pagamento confirmado
6. Sistema volta ao menu → Pronto para próximo
```

## 🔍 Monitoramento

Toda atividade é logada no Serial Monitor:
- Status das conexões WiFi
- Criação de pagamentos
- Geração de QR Codes
- Status dos pagamentos
- Erros e debugging

---

**✅ Sistema 100% Funcional via Serial Monitor**  
**🚫 Nenhum botão físico necessário**  
**📱 Controle completo via teclado do computador**
