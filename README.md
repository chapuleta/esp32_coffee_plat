# ESP32 - Caixa de Doações com Mercado Pago

Sistema de doações com QR Code do Mercado Pago para ESP32 com tela OLED 128x64.

## Características

- Menu interativo na tela OLED
- Integração com API do Mercado Pago
- Geração de QR Code para pagamento PIX
- Controle via monitor serial
- Acompanhamento de doações em tempo real
- Histórico do último e maior contribuidor

## Hardware Necessário

- ESP32 Dev Board
- Tela OLED 128x64 (I2C)
- Conexões:
  - OLED SDA → GPIO 21
  - OLED SCL → GPIO 22
  - OLED VCC → 3.3V
  - OLED GND → GND

## Configuração

1. **Edite o arquivo `include/config.h`:**
   ```cpp
   #define WIFI_SSID "SEU_WIFI_SSID"
   #define WIFI_PASSWORD "SUA_SENHA_WIFI"
   ```

2. **O token do Mercado Pago já está configurado no arquivo.**

3. **Compile e faça upload do código para o ESP32.**

## Como Usar

### Comandos do Monitor Serial (115200 baud):

- `1` - Iniciar processo de doação
- `0` - Voltar ao menu inicial

### Durante seleção de valor:

- `w` - Opção anterior
- `s` - Próxima opção  
- `e` - Selecionar opção atual

### Valores Pré-definidos:

- R$ 5,00
- R$ 10,00
- R$ 20,00
- R$ 50,00
- Valor personalizado

## Funcionamento

1. **Menu Inicial**: Mostra último contribuidor, maior contribuidor e saldo total
2. **Seleção de Valor**: Escolha entre valores pré-definidos ou digite um valor personalizado
3. **QR Code**: Gera e exibe QR Code para pagamento PIX
4. **Confirmação**: Aguarda confirmação do pagamento e atualiza os dados
5. **Retorno**: Volta automaticamente ao menu inicial

## Tela OLED - Informações Exibidas

### Menu Principal:
```
=== CAIXA DE DOACOES ===

Ultimo: [Nome]
R$ [Valor]

Maior: [Nome]  
R$ [Valor]

Saldo: R$ [Total]
```

### Seleção de Valor:
```
Selecione o valor:

> R$ 5.00
  R$ 10.00
  R$ 20.00
  R$ 50.00
  Valor personalizado
```

### QR Code:
```
R$ [Valor]
[QR Code PIX]
Escaneie o QR Code
```

## Bibliotecas Utilizadas

- Adafruit SSD1306 (Controle da tela OLED)
- Adafruit GFX Library (Gráficos)
- ArduinoJson (Manipulação JSON para API)
- QRCode (Geração de QR Codes)

## API Mercado Pago

O sistema utiliza a API v1 do Mercado Pago para:
- Criar pagamentos PIX
- Gerar QR Codes
- Verificar status dos pagamentos

## Próximas Melhorias

- [ ] Adicionar botões físicos para navegação
- [ ] Salvar dados na EEPROM/SPIFFS
- [ ] Interface web para configuração
- [ ] Suporte a múltiplos métodos de pagamento
- [ ] Relatórios detalhados de doações
- [ ] Notificações sonoras
- [ ] Backup dos dados em servidor

## Troubleshooting

### Display não funciona:
- Verifique as conexões I2C
- Confirme o endereço I2C (padrão: 0x3C)

### WiFi não conecta:
- Verifique SSID e senha no config.h
- Certifique-se que o ESP32 está no alcance do WiFi

### Erro na API do Mercado Pago:
- Verifique se o token está correto
- Confirme conexão com internet
- Veja logs no monitor serial

### QR Code não aparece:
- O QR Code pode ser muito grande para a tela
- Verifique se a resposta da API contém os dados do QR

## Licença

MIT License
