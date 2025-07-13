#ifndef CONFIG_H
#define CONFIG_H

// Configurações WiFi - ALTERE AQUI
#define WIFI_SSID "ILZAMAGDA"
#define WIFI_PASSWORD "inventaumaai"

// Token Mercado Pago
#define MERCADO_PAGO_TOKEN "APP_USR-5426692687926122-070811-eab51cec3eee674658dab7096fb3ba16-113476843"

// Pinos da tela OLED (padrão I2C)
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_ADDRESS 0x3C

// Valores pré-definidos para doação
#define NUM_VALORES_PREDEFINIDOS 4

// Configurações de tempo
#define VERIFICACAO_PAGAMENTO_INTERVALO 5000  // 5 segundos
#define TEMPO_CONFIRMACAO_TELA 5000           // 5 segundos

#endif
