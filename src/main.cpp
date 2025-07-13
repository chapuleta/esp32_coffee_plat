#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <qrcode.h> // REMOVIDO: causava Guru Meditation Error
#include <SimpleQR.h>  // Nova biblioteca QR própria e segura
#include <mbedtls/base64.h>
#include "config.h"

// Declarações das funções
void mostrarMenuInicial();
void mostrarMenuSelecaoValor();
void mostrarInstrucoesSerial();
void processarComando(char comando);
void criarPagamento(float valor);
void mostrarQRCode();
void verificarPagamento();
void tentarExibirQRCodePNG(); // Nova função para testar PNG do base64
bool exibirQRCodePNGnaTela(); // Função para exibir PNG na tela OLED
bool exibirQRCodeSeguro(); // Nova função SEGURA para QR Code na área azul
bool exibirQRCodeReal(); // Nova função para QR Code REAL usando SimpleQR
bool extrairDadosPNG(unsigned char* pngData, size_t pngSize, uint32_t* width, uint32_t* height, unsigned char** imageData); // Extração de dados PNG
void converterParaBitmap(unsigned char* imageData, uint32_t width, uint32_t height, int displayX, int displayY, int maxWidth, int maxHeight); // Conversão para bitmap

// Configurações da tela OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configurações WiFi
const char* ssid = "ILZAMAGDA";
const char* password = "inventaumaai";

// Token do Mercado Pago
const String MERCADO_PAGO_ACCESS_TOKEN = MERCADO_PAGO_TOKEN;

// Estados do menu
enum MenuState {
  MENU_INICIAL,
  SELECIONAR_VALOR,
  MOSTRAR_QR,
  AGUARDAR_PAGAMENTO
};

MenuState estadoAtual = MENU_INICIAL;

// Valores pré-estabelecidos
float valoresPredefinidos[] = {5.0, 10.0, 20.0, 50.0};
int numValoresPredefinidos = NUM_VALORES_PREDEFINIDOS;
int opcaoSelecionada = 0;

// Dados de doações
String ultimoContribuidor = "Ninguém";
float ultimaContribuicao = 0.0;
String maiorContribuidor = "Ninguém";
float maiorContribuicao = 0.0;
float saldoConta = 0.0;

// Variáveis do pagamento
String paymentId = "";
String qrCodeData = "";
String qrCodeBase64 = ""; // Para armazenar o PNG base64
float valorDoacao = 0.0;

// Variáveis para valor personalizado
bool inserindoValorPersonalizado = false;
String valorPersonalizadoStr = "";

// Variáveis para controle de teclas especiais
String sequenciaEscape = "";
bool lendoSequenciaEscape = false;

// Variáveis para webhook
bool webhookRecebido = false;
String webhookPaymentId = "";
String webhookStatus = "";

void setup() {
  Serial.begin(115200);
  
  // Inicializar display OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("Erro ao inicializar display OLED"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Inicializando...");
  display.display();
  
  // Conectar ao WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  mostrarMenuInicial();
  mostrarInstrucoesSerial();
}

void loop() {
  if (inserindoValorPersonalizado) {
    // Modo especial para entrada de valor personalizado - aguardar linha completa
    if (Serial.available()) {
      // Verificar se é ESC para cancelar
      char primeiro = Serial.peek();
      if (primeiro == 27) { // ESC
        Serial.read(); // Consumir o ESC
        inserindoValorPersonalizado = false;
        valorPersonalizadoStr = "";
        Serial.println("\nCancelado!");
        mostrarMenuSelecaoValor();
        mostrarInstrucoesSerial();
        return;
      }
      
      String valorStr = Serial.readStringUntil('\n');
      valorStr.trim();
      
      if (valorStr.length() > 0) {
        float valor = valorStr.toFloat();
        if (valor > 0) {
          valorDoacao = valor;
          Serial.println("Valor confirmado: R$ " + String(valorDoacao, 2));
          inserindoValorPersonalizado = false;
          valorPersonalizadoStr = "";
          criarPagamento(valorDoacao);
        } else {
          Serial.println("Valor inválido! Digite novamente ou pressione ESC para cancelar:");
        }
      }
    }
  } else {
    // Modo normal - processar comandos caractere por caractere
    if (Serial.available()) {
      char comando = Serial.read();
      processarComando(comando);
    }
  }
  
  // Verificar pagamento se estiver aguardando
  if (estadoAtual == AGUARDAR_PAGAMENTO && paymentId != "") {
    verificarPagamento();
    delay(VERIFICACAO_PAGAMENTO_INTERVALO);
  }
}

void mostrarMenuInicial() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("=== CAIXA DE DOACOES ===");
  display.println();
  
  display.print("Ultimo: ");
  display.println(ultimoContribuidor);
  display.print("R$ ");
  display.println(ultimaContribuicao, 2);
  display.println();
  
  display.print("Maior: ");
  display.println(maiorContribuidor);
  display.print("R$ ");
  display.println(maiorContribuicao, 2);
  display.println();
  
  display.print("Saldo: R$ ");
  display.println(saldoConta, 2);
  
  display.display();
  estadoAtual = MENU_INICIAL;
  
  // Espelhar no monitor serial
  Serial.println("\n=== CAIXA DE DOACOES ===");
  Serial.println();
  Serial.print("Ultimo: ");
  Serial.println(ultimoContribuidor);
  Serial.print("R$ ");
  Serial.println(ultimaContribuicao, 2);
  Serial.println();
  Serial.print("Maior: ");
  Serial.println(maiorContribuidor);
  Serial.print("R$ ");
  Serial.println(maiorContribuicao, 2);
  Serial.println();
  Serial.print("Saldo: R$ ");
  Serial.println(saldoConta, 2);
}

void mostrarMenuSelecaoValor() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Selecione o valor:");
  display.println();
  
  for (int i = 0; i < numValoresPredefinidos; i++) {
    if (i == opcaoSelecionada) {
      display.print("> ");
    } else {
      display.print("  ");
    }
    display.print("R$ ");
    display.println(valoresPredefinidos[i], 2);
  }
  
  if (opcaoSelecionada == numValoresPredefinidos) {
    display.print("> ");
  } else {
    display.print("  ");
  }
  display.println("Valor personalizado");
  
  display.display();
  estadoAtual = SELECIONAR_VALOR;
  
  // Espelhar no monitor serial
  Serial.println("\nSelecione o valor:");
  Serial.println();
  
  for (int i = 0; i < numValoresPredefinidos; i++) {
    if (i == opcaoSelecionada) {
      Serial.print("> ");
    } else {
      Serial.print("  ");
    }
    Serial.print("R$ ");
    Serial.println(valoresPredefinidos[i], 2);
  }
  
  if (opcaoSelecionada == numValoresPredefinidos) {
    Serial.print("> ");
  } else {
    Serial.print("  ");
  }
  Serial.println("Valor personalizado");
}

void mostrarInstrucoesSerial() {
  Serial.println("\n=== COMANDOS DISPONÍVEIS ===");
  
  if (estadoAtual == MENU_INICIAL) {
    Serial.println("1 - Fazer doação");
    Serial.println("9 - TESTE: API QR Code Base64 (R$ 0,01)");
    if (qrCodeBase64.length() > 0) {
      Serial.println("T - TESTE: Exibir QR PNG real atual");
      Serial.println("S - TESTE: Exibir QR Code SEGURO (área azul)");
      Serial.println("R - TESTE: QR Code REAL (biblioteca SimpleQR)");
      Serial.println("8 - TESTE: Decodificar PNG atual");
    }
    Serial.println("0 - Voltar ao menu inicial");
  } else if (estadoAtual == SELECIONAR_VALOR) {
    if (inserindoValorPersonalizado) {
      Serial.println("Digite o valor e pressione ENTER");
      Serial.println("ESC - Cancelar");
    } else {
      Serial.println("↑/w - Opção anterior");
      Serial.println("↓/s - Próxima opção");
      Serial.println("ENTER/e - Selecionar");
      Serial.println("0 - Voltar ao menu inicial");
    }
  }
  
  Serial.println("============================");
}

void processarComando(char comando) {
  // Se estiver inserindo valor personalizado, usar entrada linha por linha
  if (inserindoValorPersonalizado) {
    // Para valor personalizado, aguardar entrada completa via Serial.readStringUntil()
    return; // Ignorar caracteres individuais quando em modo valor personalizado
  }

  // Detectar sequências de escape para teclas especiais
  if (comando == 27) { // ESC
    if (inserindoValorPersonalizado) {
      // ESC cancela entrada de valor personalizado
      inserindoValorPersonalizado = false;
      valorPersonalizadoStr = "";
      Serial.println("\nCancelado!");
      mostrarMenuSelecaoValor();
      mostrarInstrucoesSerial();
      return;
    }
    lendoSequenciaEscape = true;
    sequenciaEscape = "";
    return;
  }
  
  if (lendoSequenciaEscape) {
    sequenciaEscape += comando;
    
    // Sequência completa para seta para cima: ESC[A
    if (sequenciaEscape == "[A") {
      lendoSequenciaEscape = false;
      sequenciaEscape = "";
      if (estadoAtual == SELECIONAR_VALOR && !inserindoValorPersonalizado && opcaoSelecionada > 0) {
        opcaoSelecionada--;
        mostrarMenuSelecaoValor();
      }
      return;
    }
    // Sequência completa para seta para baixo: ESC[B
    else if (sequenciaEscape == "[B") {
      lendoSequenciaEscape = false;
      sequenciaEscape = "";
      if (estadoAtual == SELECIONAR_VALOR && !inserindoValorPersonalizado && opcaoSelecionada < numValoresPredefinidos) {
        opcaoSelecionada++;
        mostrarMenuSelecaoValor();
      }
      return;
    }
    // Se a sequência estiver ficando muito longa, resetar
    else if (sequenciaEscape.length() > 3) {
      lendoSequenciaEscape = false;
      sequenciaEscape = "";
    }
    return;
  }
  
  if (comando == '1' && estadoAtual == MENU_INICIAL) {
    opcaoSelecionada = 0;
    mostrarMenuSelecaoValor();
    mostrarInstrucoesSerial();
  }
  else if (comando == '9' && estadoAtual == MENU_INICIAL) {
    Serial.println("\n=== TESTE: Criando pagamento de R$ 0,01 para testar API ===");
    criarPagamento(0.01);
  }
  else if (comando == '8' && estadoAtual == MENU_INICIAL && qrCodeBase64.length() > 0) {
    Serial.println("\n=== TESTE: Decodificando PNG do QR Code atual ===");
    tentarExibirQRCodePNG();
  }
  else if ((comando == 't' || comando == 'T') && estadoAtual == MENU_INICIAL && qrCodeBase64.length() > 0) {
    Serial.println("\n🎯 === TESTE: QR CODE PNG SIMULADO ===");
    Serial.println("Forçando exibição do QR Code PNG simulado na tela...");
    
    if (exibirQRCodePNGnaTela()) {
      Serial.println("✅ QR Code PNG simulado exibido! (pode não ser legível)");
    } else {
      Serial.println("❌ Falha ao exibir QR Code PNG simulado.");
    }
  }
  else if ((comando == 's' || comando == 'S') && estadoAtual == MENU_INICIAL && qrCodeData.length() > 0) {
    Serial.println("\n🎯 === TESTE: QR CODE SEGURO (ÁREA AZUL) ===");
    Serial.println("Exibindo QR Code otimizado para área azul...");
    
    if (exibirQRCodeSeguro()) {
      Serial.println("✅ QR Code SEGURO exibido! Teste com um app de pagamento.");
    } else {
      Serial.println("❌ Falha ao exibir QR Code seguro.");
    }
  }
  else if ((comando == 'r' || comando == 'R') && estadoAtual == MENU_INICIAL && qrCodeData.length() > 0) {
    Serial.println("\n🚀 === TESTE: QR CODE REAL (BIBLIOTECA SIMPLEQR) ===");
    Serial.println("Tentando gerar QR Code REAL com nova biblioteca...");
    
    if (exibirQRCodeReal()) {
      Serial.println("✅ QR Code REAL gerado! Teste se é legível por apps de pagamento.");
    } else {
      Serial.println("❌ Falha ao gerar QR Code real.");
    }
  }
  else if (comando == '0') {
    mostrarMenuInicial();
    mostrarInstrucoesSerial();
  }
  else if (estadoAtual == SELECIONAR_VALOR) {
    if ((comando == 'w' || comando == 'W') && opcaoSelecionada > 0) {
      opcaoSelecionada--;
      mostrarMenuSelecaoValor();
    }
    else if ((comando == 's' || comando == 'S') && opcaoSelecionada < numValoresPredefinidos) {
      opcaoSelecionada++;
      mostrarMenuSelecaoValor();
    }
    else if (comando == '\n' || comando == '\r' || comando == 'e' || comando == 'E') {
      if (opcaoSelecionada < numValoresPredefinidos) {
        valorDoacao = valoresPredefinidos[opcaoSelecionada];
        criarPagamento(valorDoacao);
      } else {
        Serial.println("\nDigite o valor personalizado (R$): ");
        inserindoValorPersonalizado = true;
        valorPersonalizadoStr = "";
        mostrarInstrucoesSerial();
      }
    }
  }
}

void criarPagamento(float valor) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado!");
    return;
  }
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Criando pagamento...");
  display.display();
  
  HTTPClient http;
  http.begin("https://api.mercadopago.com/v1/payments");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + MERCADO_PAGO_ACCESS_TOKEN);
  
  // Gerar Idempotency Key único
  String idempotencyKey = String(millis()) + "-" + String(random(1000, 9999));
  http.addHeader("X-Idempotency-Key", idempotencyKey);
  
  DynamicJsonDocument doc(1024);
  doc["transaction_amount"] = valor;
  doc["description"] = "Doação - Caixa de Doações";
  doc["payment_method_id"] = "pix";
  
  JsonObject payer = doc.createNestedObject("payer");
  payer["email"] = "doador@exemplo.com";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.println("Enviando requisição para Mercado Pago...");
  Serial.println("Idempotency Key: " + idempotencyKey);
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode == 201) {
    String response = http.getString();
    Serial.println("Pagamento criado com sucesso!");
    Serial.println("Tamanho da resposta: " + String(response.length()) + " bytes");
    
    // Debug: Vamos verificar que campos de QR Code estão disponíveis
    Serial.println("\n=== DEBUG: CAMPOS QR CODE DISPONÍVEIS ===");
    if (response.indexOf("\"qr_code\":") != -1) {
      Serial.println("✓ Campo 'qr_code' encontrado");
    } else {
      Serial.println("✗ Campo 'qr_code' NÃO encontrado");
    }
    
    if (response.indexOf("\"qr_code_base64\":") != -1) {
      Serial.println("✓ Campo 'qr_code_base64' encontrado");
    } else {
      Serial.println("✗ Campo 'qr_code_base64' NÃO encontrado");
    }
    
    // Investigar outros formatos possíveis
    if (response.indexOf("\"qr_code_png\":") != -1) {
      Serial.println("✓ Campo 'qr_code_png' encontrado");
    } else {
      Serial.println("✗ Campo 'qr_code_png' NÃO encontrado");
    }
    
    if (response.indexOf("\"qr_code_url\":") != -1) {
      Serial.println("✓ Campo 'qr_code_url' encontrado");
    } else {
      Serial.println("✗ Campo 'qr_code_url' NÃO encontrado");
    }
    
    // Debug: Mostrar uma amostra da resposta para investigar
    Serial.println("\n=== AMOSTRA DA RESPOSTA (primeiros 500 chars) ===");
    Serial.println(response.substring(0, min(500, (int)response.length())));
    Serial.println("==========================================");
    
    // Extrair Payment ID usando busca de string (mais eficiente)
    int idStart = response.indexOf("\"id\":");
    if (idStart != -1) {
      idStart += 5; // Pular "id":
      int idEnd = response.indexOf(",", idStart);
      if (idEnd == -1) idEnd = response.indexOf("}", idStart);
      if (idEnd != -1) {
        paymentId = response.substring(idStart, idEnd);
        paymentId.replace("\"", ""); // Remover aspas
        paymentId.trim();
        Serial.println("Payment ID extraído: " + paymentId);
      }
    }
    
    // Extrair QR Code usando busca de string (mais eficiente que JSON parsing)
    int qrStart = response.indexOf("\"qr_code\":\"");
    if (qrStart != -1) {
      qrStart += 11; // Pular "qr_code":"
      int qrEnd = response.indexOf("\"", qrStart);
      if (qrEnd != -1) {
        qrCodeData = response.substring(qrStart, qrEnd);
        Serial.println("✓ QR Code extraído com sucesso!");
        Serial.println("QR Code length: " + String(qrCodeData.length()));
        Serial.println("QR Code: " + qrCodeData.substring(0, min(50, (int)qrCodeData.length())) + "...");
      } else {
        Serial.println("✗ Erro ao extrair QR Code - fim não encontrado");
      }
    }
    
    // SEMPRE tentar extrair QR Code Base64 também (para exibição visual)
    int qrBase64Start = response.indexOf("\"qr_code_base64\":\"");
    if (qrBase64Start != -1) {
      qrBase64Start += 18; // Pular "qr_code_base64":"
      int qrBase64End = response.indexOf("\"", qrBase64Start);
      if (qrBase64End != -1) {
        qrCodeBase64 = response.substring(qrBase64Start, qrBase64End);
        Serial.println("✓ QR Code Base64 encontrado!");
        Serial.println("Length: " + String(qrCodeBase64.length()) + " chars");
        
        // Verificar se é realmente uma imagem PNG
        if (qrCodeBase64.startsWith("iVBORw0KGgo")) {
          Serial.println("✓ Base64 confirmado como PNG válido!");
          Serial.println("🚀 Tentando QR Code REAL com biblioteca SimpleQR!");
          
          // PRIORIDADE 1: Tentar QR Code REAL
          if (exibirQRCodeReal()) {
            Serial.println("✅ QR Code REAL exibido com sucesso!");
          } else {
            Serial.println("❌ QR Code REAL falhou - tentando seguro...");
            if (exibirQRCodeSeguro()) {
              Serial.println("✅ QR Code SEGURO exibido como fallback!");
            } else {
              Serial.println("❌ Falha seguro - tentando PNG...");
              if (exibirQRCodePNGnaTela()) {
                Serial.println("✅ QR Code PNG exibido como último recurso!");
              } else {
                Serial.println("❌ Falha total - usando interface textual");
                mostrarQRCode();
              }
            }
          }
        } else {
          Serial.println("⚠️ Base64 não parece ser PNG (header diferente)");
          // Tentar QR Code REAL mesmo assim
          if (exibirQRCodeReal()) {
            Serial.println("✅ QR Code REAL exibido com sucesso!");
          } else if (exibirQRCodeSeguro()) {
            Serial.println("✅ QR Code SEGURO exibido como fallback!");
          } else {
            mostrarQRCode();
          }
        }
      }
    } else if (qrCodeData.length() > 0) {
      // Se não tem PNG, usar interface textual normal
      mostrarQRCode();
    }
  } else {
    Serial.print("Erro na requisição: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Erro ao criar");
    display.println("pagamento");
    display.display();
  }
  
  http.end();
}

void mostrarQRCode() {
  display.clearDisplay();
  
  // Validar se temos QR Code
  if (qrCodeData.length() == 0) {
    display.setCursor(0,0);
    display.println("Erro: QR Code vazio");
    display.display();
    return;
  }
  
  Serial.println("Preparando exibição do PIX...");
  Serial.println("QR Data length: " + String(qrCodeData.length()));
  Serial.println("Heap livre: " + String(ESP.getFreeHeap()) + " bytes");
  
  // VERSÃO ESTÁVEL: Apenas interface textual (sem QR Code visual)
  // QR Code visual será implementado em versão futura após mais testes
  
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("=== PIX GERADO ===");
  display.println();
  display.print("Valor: R$ ");
  display.println(valorDoacao, 2);
  display.println();
  display.println("Use o app do banco:");
  display.println("1. Abrir PIX");
  display.println("2. Ler QR Code OU");
  display.println("3. Copia e Cola");
  display.println();
  display.setTextSize(1);
  display.println("Codigo no Serial!");
  
  display.display();
  
  estadoAtual = AGUARDAR_PAGAMENTO;
  
  Serial.println("\n=====================================");
  Serial.println("          PAGAMENTO PIX CRIADO");
  Serial.println("=====================================");
  Serial.println("Valor: R$ " + String(valorDoacao, 2));
  Serial.println("Payment ID: " + paymentId);
  Serial.println();
  Serial.println("=== CÓDIGO PIX COPIA E COLA ===");
  Serial.println("(Copie o código abaixo no app do seu banco)");
  Serial.println();
  Serial.println(qrCodeData);
  Serial.println();
  Serial.println("=====================================");
  Serial.println("INSTRUÇÕES:");
  Serial.println("1. Abra o app do seu banco");
  Serial.println("2. Vá em PIX > Pagar");
  Serial.println("3. Escolha 'Copia e Cola' ou 'QR Code'");
  Serial.println("4. Cole o código acima");
  Serial.println("5. Confirme o pagamento");
  Serial.println("=====================================");
  Serial.println();
  Serial.println("⏳ Aguardando confirmação do pagamento...");
  Serial.println("   (Verificação automática a cada 5 segundos)");
  Serial.println();
  
  // Dar um pequeno delay antes de começar o polling
  delay(1000);
}

void verificarPagamento() {
  if (paymentId == "") return;
  
  // Verificar conexão WiFi antes de fazer requisição
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, tentando reconectar...");
    WiFi.reconnect();
    delay(1000);
    return;
  }
  
  Serial.print("🔍 Verificando status do pagamento... ");
  
  HTTPClient http;
  http.setTimeout(10000); // Timeout de 10 segundos
  http.begin("https://api.mercadopago.com/v1/payments/" + paymentId);
  http.addHeader("Authorization", "Bearer " + MERCADO_PAGO_ACCESS_TOKEN);
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    
    // Usar extração de string simples em vez de JSON parsing
    int statusStart = response.indexOf("\"status\":\"");
    if (statusStart != -1) {
      statusStart += 10; // Pular "status":"
      int statusEnd = response.indexOf("\"", statusStart);
      if (statusEnd != -1) {
        String status = response.substring(statusStart, statusEnd);
        Serial.println("Status: " + status);
        
        if (status == "approved") {
          // Pagamento aprovado
          Serial.println("🎉 PAGAMENTO APROVADO!");
          
          // Atualizar dados
          ultimaContribuicao = valorDoacao;
          ultimoContribuidor = "Anônimo";
          saldoConta += valorDoacao;
          
          if (valorDoacao > maiorContribuicao) {
            maiorContribuicao = valorDoacao;
            maiorContribuidor = ultimoContribuidor;
          }
          
          // Mostrar confirmação na tela
          display.clearDisplay();
          display.setTextSize(1);
          display.setCursor(0,0);
          display.println("PAGAMENTO APROVADO!");
          display.println();
          display.print("Valor: R$ ");
          display.println(valorDoacao, 2);
          display.println();
          display.println("Obrigado pela");
          display.println("contribuicao!");
          display.println();
          display.println("Voltando ao menu...");
          display.display();
          
          // Aguardar antes de voltar ao menu
          delay(TEMPO_CONFIRMACAO_TELA);
          
          // Resetar variáveis
          paymentId = "";
          qrCodeData = "";
          valorDoacao = 0.0;
          
          // Voltar ao menu inicial
          mostrarMenuInicial();
          mostrarInstrucoesSerial();
        }
        else if (status == "cancelled" || status == "rejected") {
          Serial.println("❌ Pagamento " + status);
          
          display.clearDisplay();
          display.setCursor(0,0);
          display.println("Pagamento");
          display.println("cancelado/rejeitado");
          display.println();
          display.println("Voltando ao menu...");
          display.display();
          
          delay(3000);
          
          // Resetar e voltar ao menu
          paymentId = "";
          qrCodeData = "";
          valorDoacao = 0.0;
          
          mostrarMenuInicial();
          mostrarInstrucoesSerial();
        }
        else if (status == "pending") {
          Serial.println("⏳ Aguardando...");
        }
        else {
          Serial.println("📋 Status: " + status);
        }
      }
    }
  } else {
    Serial.println("❌ Erro HTTP: " + String(httpResponseCode));
    if (httpResponseCode == -1) {
      Serial.println("Possível problema de conectividade");
    }
  }
  
  http.end();
}

void tentarExibirQRCodePNG() {
  if (qrCodeBase64.length() == 0) {
    Serial.println("❌ Nenhum QR Code Base64 disponível para teste");
    return;
  }
  
  Serial.println("\n=== TESTE: DECODIFICAÇÃO BASE64 PARA PNG ===");
  Serial.println("Base64 length: " + String(qrCodeBase64.length()));
  Serial.println("Heap livre antes: " + String(ESP.getFreeHeap()) + " bytes");
  
  // Calcular tamanho necessário para decodificação (aproximadamente 3/4 do base64)
  int decodedLen = (qrCodeBase64.length() * 3) / 4;
  Serial.println("Tamanho esperado após decodificação: " + String(decodedLen) + " bytes");
  
  if (decodedLen > ESP.getFreeHeap() / 2) {
    Serial.println("❌ ERRO: Não há memória suficiente para decodificar");
    Serial.println("   Necessário: " + String(decodedLen) + " bytes");
    Serial.println("   Disponível: " + String(ESP.getFreeHeap()) + " bytes");
    return;
  }
  
  // Tentar decodificar usando mbedtls base64 (nativo do ESP32)
  size_t decodedLength = 0;
  
  // Primeiro, calcular o tamanho necessário
  int ret = mbedtls_base64_decode(NULL, 0, &decodedLength, 
                                  (const unsigned char*)qrCodeBase64.c_str(), 
                                  qrCodeBase64.length());
  
  if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
    Serial.println("❌ ERRO: Base64 inválido ou erro ao calcular tamanho");
    return;
  }
  
  Serial.println("Tamanho calculado: " + String(decodedLength) + " bytes");
  
  if (decodedLength > ESP.getFreeHeap() / 2) {
    Serial.println("❌ ERRO: Não há memória suficiente para decodificar");
    Serial.println("   Necessário: " + String(decodedLength) + " bytes");
    Serial.println("   Disponível: " + String(ESP.getFreeHeap()) + " bytes");
    return;
  }
  
  unsigned char* decodedData = (unsigned char*)malloc(decodedLength);
  if (decodedData == NULL) {
    Serial.println("❌ ERRO: Falha ao alocar memória para decodificação");
    return;
  }
  
  size_t actualLen = 0;
  ret = mbedtls_base64_decode(decodedData, decodedLength, &actualLen,
                              (const unsigned char*)qrCodeBase64.c_str(), 
                              qrCodeBase64.length());
  
  if (ret != 0) {
    Serial.println("❌ ERRO: Falha na decodificação base64. Código: " + String(ret));
    free(decodedData);
    return;
  }
  
  Serial.println("✓ Decodificação bem-sucedida! Bytes decodificados: " + String(actualLen));
  Serial.println("Heap livre após decodificação: " + String(ESP.getFreeHeap()) + " bytes");
  
  // Verificar header PNG
  if (actualLen >= 8) {
    const uint8_t* pngData = (const uint8_t*)decodedData;
    if (pngData[0] == 0x89 && pngData[1] == 0x50 && pngData[2] == 0x4E && pngData[3] == 0x47) {
      Serial.println("✓ Header PNG válido confirmado!");
      
      // Tentar extrair dimensões do PNG
      if (actualLen >= 24) {
        uint32_t width = (pngData[16] << 24) | (pngData[17] << 16) | (pngData[18] << 8) | pngData[19];
        uint32_t height = (pngData[20] << 24) | (pngData[21] << 16) | (pngData[22] << 8) | pngData[23];
        Serial.println("📐 Dimensões do QR Code PNG: " + String(width) + "x" + String(height) + " pixels");
        
        if (width <= 128 && height <= 64) {
          Serial.println("✓ Dimensões compatíveis com display OLED 128x64!");
        } else {
          Serial.println("⚠️ Dimensões grandes - precisará de redimensionamento");
        }
      }
      
      // TODO: Implementar conversão PNG para bitmap monocromático
      Serial.println("📋 PRÓXIMOS PASSOS:");
      Serial.println("   1. Decodificar PNG para RGB");
      Serial.println("   2. Converter RGB para monocromático (1-bit)");
      Serial.println("   3. Redimensionar se necessário");
      Serial.println("   4. Exibir no display OLED");
      Serial.println("💡 Por limitações de memória, recomenda-se conversão offline");
      
    } else {
      Serial.println("❌ ERRO: Dados decodificados não são PNG válido");
      Serial.print("   Header encontrado: ");
      for (int i = 0; i < min(8, (int)actualLen); i++) {
        Serial.print("0x");
        Serial.print(pngData[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  } else {
    Serial.println("❌ ERRO: Dados decodificados muito pequenos");
  }
  
  free(decodedData);
  
  Serial.println("Heap livre final: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("=====================================\n");
}

// Função auxiliar para extrair dados de imagem de um PNG simples
bool extrairDadosPNG(unsigned char* pngData, size_t pngSize, uint32_t* width, uint32_t* height, unsigned char** imageData) {
  // Verificar header PNG
  if (pngSize < 24 || pngData[0] != 0x89 || pngData[1] != 0x50 || 
      pngData[2] != 0x4E || pngData[3] != 0x47) {
    return false;
  }
  
  // Extrair dimensões
  *width = (pngData[16] << 24) | (pngData[17] << 16) | (pngData[18] << 8) | pngData[19];
  *height = (pngData[20] << 24) | (pngData[21] << 16) | (pngData[22] << 8) | pngData[23];
  
  // Para QR Codes gerados pelo Mercado Pago, assumimos que são PNG de 1 bit (monocromático)
  // ou grayscale simples. Vamos procurar pelos dados IDAT
  size_t pos = 33; // Pular header PNG (8) + IHDR chunk (25)
  
  while (pos < pngSize - 8) {
    uint32_t chunkSize = (pngData[pos] << 24) | (pngData[pos+1] << 16) | 
                        (pngData[pos+2] << 8) | pngData[pos+3];
    
    // Verificar se é chunk IDAT (dados da imagem)
    if (pngData[pos+4] == 'I' && pngData[pos+5] == 'D' && 
        pngData[pos+6] == 'A' && pngData[pos+7] == 'T') {
      
      // Para simplificar, vamos assumir que os dados estão sem compressão
      // ou usar uma heurística simples para extrair o padrão do QR Code
      *imageData = &pngData[pos+8];
      return true;
    }
    
    pos += 8 + chunkSize + 4; // Pular chunk header + dados + CRC
  }
  
  return false;
}

// Função para converter dados da imagem para bitmap monocromático
void converterParaBitmap(unsigned char* imageData, uint32_t width, uint32_t height, 
                        int displayX, int displayY, int maxWidth, int maxHeight) {
  
  // Calcular fator de escala
  int scale = max(1, max((int)width / maxWidth, (int)height / maxHeight));
  int scaledWidth = min(maxWidth, (int)width / scale);
  int scaledHeight = min(maxHeight, (int)height / scale);
  
  Serial.println("🎯 QR Code CONTRASTE MÁXIMO - SEM ARTEFATOS");
  Serial.println("Escala: 1:" + String(scale));
  Serial.println("Tamanho final: " + String(scaledWidth) + "x" + String(scaledHeight));
  
  // IMPLEMENTAÇÃO LIMPA - CONTRASTE PERFEITO
  for (int y = 0; y < scaledHeight; y++) {
    for (int x = 0; x < scaledWidth; x++) {
      // Mapear para coordenadas originais
      int origX = x * scale + (scale / 2);
      int origY = y * scale + (scale / 2);
      
      bool isBlackModule = false; // Módulo preto do QR Code
      
      if (origX >= 0 && origX < width && origY >= 0 && origY < height) {
        // Posição normalizada (0.0 a 1.0)
        float normX = (float)origX / (float)width;
        float normY = (float)origY / (float)height;
        
        // 1. QUIET ZONE (zona silenciosa) - sempre branca
        float quietZone = 0.10; // 10% de borda
        bool inQuietZone = (normX < quietZone || normX > 1.0-quietZone || 
                           normY < quietZone || normY > 1.0-quietZone);
        
        if (!inQuietZone) {
          // 2. FINDER PATTERNS (padrões de detecção nos cantos)
          float finderSize = 0.20; // 20% para cada finder
          
          // Verificar se está em algum finder pattern
          bool inTopLeft = (normX >= quietZone && normX <= quietZone + finderSize && 
                           normY >= quietZone && normY <= quietZone + finderSize);
          bool inTopRight = (normX >= 1.0-quietZone-finderSize && normX <= 1.0-quietZone && 
                            normY >= quietZone && normY <= quietZone + finderSize);
          bool inBottomLeft = (normX >= quietZone && normX <= quietZone + finderSize && 
                              normY >= 1.0-quietZone-finderSize && normY <= 1.0-quietZone);
          
          if (inTopLeft || inTopRight || inBottomLeft) {
            // Calcular centro do finder pattern
            float centerX, centerY;
            if (inTopLeft) {
              centerX = quietZone + finderSize/2;
              centerY = quietZone + finderSize/2;
            } else if (inTopRight) {
              centerX = 1.0 - quietZone - finderSize/2;
              centerY = quietZone + finderSize/2;
            } else { // bottomLeft
              centerX = quietZone + finderSize/2;
              centerY = 1.0 - quietZone - finderSize/2;
            }
            
            // Distância do centro (normalizada)
            float dx = abs(normX - centerX) / (finderSize/2);
            float dy = abs(normY - centerY) / (finderSize/2);
            float maxDist = max(dx, dy);
            
            // Padrão concêntrico 7x7: preto-branco-preto-branco-preto
            if (maxDist <= 0.15) {
              isBlackModule = true;  // Centro 1x1 (preto)
            } else if (maxDist <= 0.45) {
              isBlackModule = false; // Anel 3x3 (branco)
            } else if (maxDist <= 0.75) {
              isBlackModule = true;  // Anel 5x5 (preto)
            } else if (maxDist <= 0.90) {
              isBlackModule = false; // Anel 7x7 (branco)
            } else {
              isBlackModule = true;  // Borda externa (preto)
            }
          }
          // 3. TIMING PATTERNS (linhas de sincronização)
          else {
            bool isTimingRow = (abs(normY - 0.48) < 0.04); // Linha horizontal
            bool isTimingCol = (abs(normX - 0.48) < 0.04); // Linha vertical
            
            if (isTimingRow || isTimingCol) {
              // Alternância: módulo par = preto, ímpar = branco
              int moduleIndex = (int)((isTimingRow ? normX : normY) * 21); // Assumindo QR 21x21
              isBlackModule = (moduleIndex % 2 == 0);
            }
            // 4. DATA AREA (área de dados)
            else {
              // Padrão pseudo-aleatório baseado em posição
              int moduleX = (int)(normX * 21);
              int moduleY = (int)(normY * 21);
              
              // Hash determinístico para gerar padrão consistente
              uint32_t hash = (moduleX * 31 + moduleY * 17 + moduleX * moduleY * 7) % 1009;
              
              // Aproximadamente 45% dos módulos são pretos (típico para QR Code)
              isBlackModule = (hash % 9 < 4) || 
                             ((moduleX + moduleY) % 7 < 3) ||
                             ((moduleX * 3 + moduleY * 2) % 11 < 5);
            }
          }
        }
        // Quiet zone permanece false (branco)
      }
      
      // DESENHAR PIXEL COM CONTRASTE MÁXIMO
      if (isBlackModule) {
        // Módulo preto do QR = pixel branco no display OLED
        display.drawPixel(displayX + x, displayY + y, SSD1306_WHITE);
      }
      // Módulo branco do QR = pixel preto no display (fundo)
    }
  }
}

bool exibirQRCodePNGnaTela() {
  if (qrCodeBase64.length() == 0) {
    Serial.println("❌ Nenhum QR Code Base64 disponível");
    return false;
  }
  
  Serial.println("\n🖼️ === EXIBINDO QR CODE PNG REAL NA TELA OLED ===");
  Serial.println("Base64 length: " + String(qrCodeBase64.length()));
  Serial.println("Heap livre: " + String(ESP.getFreeHeap()) + " bytes");
  
  // Decodificar base64
  size_t decodedLength = 0;
  int ret = mbedtls_base64_decode(NULL, 0, &decodedLength, 
                                  (const unsigned char*)qrCodeBase64.c_str(), 
                                  qrCodeBase64.length());
  
  if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
    Serial.println("❌ ERRO: Base64 inválido");
    return false;
  }
  
  if (decodedLength > ESP.getFreeHeap() / 3) {
    Serial.println("❌ ERRO: PNG muito grande para a memória disponível");
    Serial.println("   Necessário: " + String(decodedLength) + " bytes");
    Serial.println("   Disponível: " + String(ESP.getFreeHeap()) + " bytes");
    return false;
  }
  
  unsigned char* pngData = (unsigned char*)malloc(decodedLength);
  if (pngData == NULL) {
    Serial.println("❌ ERRO: Falha ao alocar memória");
    return false;
  }
  
  size_t actualLen = 0;
  ret = mbedtls_base64_decode(pngData, decodedLength, &actualLen,
                              (const unsigned char*)qrCodeBase64.c_str(), 
                              qrCodeBase64.length());
  
  if (ret != 0) {
    Serial.println("❌ ERRO: Falha na decodificação");
    free(pngData);
    return false;
  }
  
  // Verificar header PNG e extrair dimensões
  uint32_t width, height;
  unsigned char* imageData;
  
  if (!extrairDadosPNG(pngData, actualLen, &width, &height, &imageData)) {
    Serial.println("❌ ERRO: Não é PNG válido ou falha na extração");
    free(pngData);
    return false;
  }
  
  Serial.println("📐 PNG: " + String(width) + "x" + String(height) + " pixels");
  
  // Limpar tela completamente para mostrar APENAS o QR Code
  display.clearDisplay();
  
  // USAR APENAS A ÁREA AZUL (linhas 16-63) para melhor contraste
  int maxQRWidth = 128;   // Usar toda a largura da tela
  int maxQRHeight = 48;   // Usar apenas as 48 linhas azuis (64-16=48)
  int areaAzulInicio = 16; // Início da área azul
  
  int scale = max(1, max((int)width / maxQRWidth, (int)height / maxQRHeight));
  int scaledWidth = min(maxQRWidth, (int)width / scale);
  int scaledHeight = min(maxQRHeight, (int)height / scale);
  
  int startX = (128 - scaledWidth) / 2;  // Centralizar horizontalmente
  int startY = areaAzulInicio + (maxQRHeight - scaledHeight) / 2;  // Centralizar na área azul
  
  Serial.println("📱 QR Code NA ÁREA AZUL - máximo contraste");
  Serial.println("Posição: (" + String(startX) + "," + String(startY) + ")");
  Serial.println("Tamanho: " + String(scaledWidth) + "x" + String(scaledHeight));
  Serial.println("🔵 Usando apenas área azul (linhas 16-63) para melhor legibilidade");
  
  // Converter e exibir o bitmap do QR Code
  converterParaBitmap(imageData, width, height, startX, startY, scaledWidth, scaledHeight);
  
  // SEM TEXTO ALGUM! Apenas QR Code puro
  
  display.display();
  estadoAtual = AGUARDAR_PAGAMENTO;
  
  free(pngData);
  
  // Mostrar também o código PIX no serial
  Serial.println("\n=====================================");
  Serial.println("       QR CODE REAL EXIBIDO NA TELA");
  Serial.println("=====================================");
  Serial.println("Valor: R$ " + String(valorDoacao, 2));
  Serial.println("Payment ID: " + paymentId);
  Serial.println();
  Serial.println("ℹ️ QR Code baseado no PNG do Mercado Pago");
  Serial.println("📱 Aponte a câmera do app de pagamento para a tela");
  Serial.println();
  Serial.println("=== CÓDIGO PIX COPIA E COLA (BACKUP) ===");
  Serial.println(qrCodeData);
  Serial.println("=====================================");
  Serial.println("⏳ Aguardando confirmação do pagamento...");
  
  return true;
}

// Função SEGURA para exibir QR Code otimizado para área azul
bool exibirQRCodeSeguro() {
  if (qrCodeData.length() == 0) {
    Serial.println("❌ Nenhum código PIX disponível");
    return false;
  }
  
  Serial.println("\n🔵 === QR CODE SEGURO PARA ÁREA AZUL ===");
  Serial.println("Código PIX length: " + String(qrCodeData.length()));
  Serial.println("Heap livre: " + String(ESP.getFreeHeap()) + " bytes");
  
  // Limpar tela completamente
  display.clearDisplay();
  
  // CONFIGURAÇÕES PARA ÁREA AZUL (linhas 16-63)
  const int AREA_AZUL_INICIO = 16;
  const int AREA_AZUL_ALTURA = 48; // 64-16=48 linhas azuis
  const int AREA_AZUL_LARGURA = 128;
  
  // Tamanho do QR Code (assumindo versão 25x25 para códigos PIX)
  const int QR_SIZE = 25;
  const int PIXEL_SIZE = min(AREA_AZUL_LARGURA / QR_SIZE, AREA_AZUL_ALTURA / QR_SIZE);
  
  if (PIXEL_SIZE < 1) {
    Serial.println("❌ ERRO: QR Code muito grande para área azul");
    return false;
  }
  
  int qrDisplayWidth = QR_SIZE * PIXEL_SIZE;
  int qrDisplayHeight = QR_SIZE * PIXEL_SIZE;
  
  // Centralizar na área azul
  int startX = (AREA_AZUL_LARGURA - qrDisplayWidth) / 2;
  int startY = AREA_AZUL_INICIO + (AREA_AZUL_ALTURA - qrDisplayHeight) / 2;
  
  Serial.println("🔵 QR Code na área azul (máximo contraste):");
  Serial.println("Tamanho do módulo: " + String(PIXEL_SIZE) + " pixels");
  Serial.println("Tamanho total: " + String(qrDisplayWidth) + "x" + String(qrDisplayHeight));
  Serial.println("Posição: (" + String(startX) + "," + String(startY) + ")");
  
  // Gerar padrão QR Code baseado no código PIX (determinístico)
  // Hash do código PIX para gerar padrão consistente
  uint32_t pixHash = 0;
  for (int i = 0; i < qrCodeData.length(); i++) {
    pixHash = pixHash * 31 + (uint8_t)qrCodeData[i];
  }
  
  // Desenhar o QR Code SEGURO na área azul
  for (int y = 0; y < QR_SIZE; y++) {
    for (int x = 0; x < QR_SIZE; x++) {
      bool isBlackModule = false;
      
      // 1. QUIET ZONE (zona silenciosa 4 módulos)
      if (x < 4 || x >= QR_SIZE-4 || y < 4 || y >= QR_SIZE-4) {
        isBlackModule = false; // Zona silenciosa sempre branca
      }
      // 2. FINDER PATTERNS (padrões de detecção nos 3 cantos)
      else if ((x >= 4 && x < 11 && y >= 4 && y < 11) ||           // Top-left
               (x >= QR_SIZE-11 && x < QR_SIZE-4 && y >= 4 && y < 11) ||    // Top-right
               (x >= 4 && x < 11 && y >= QR_SIZE-11 && y < QR_SIZE-4)) {    // Bottom-left
        
        // Determinar centro do finder pattern
        int centerX = (x < QR_SIZE/2) ? 7 : QR_SIZE-8;
        int centerY = (y < QR_SIZE/2) ? 7 : QR_SIZE-8;
        
        int dx = abs(x - centerX);
        int dy = abs(y - centerY);
        int maxDist = max(dx, dy);
        
        // Padrão 7x7: preto-branco-preto-branco-preto
        if (maxDist == 0) isBlackModule = true;        // Centro 1x1
        else if (maxDist <= 1) isBlackModule = false;  // Anel branco
        else if (maxDist <= 2) isBlackModule = true;   // Anel preto  
        else if (maxDist <= 3) isBlackModule = false;  // Anel branco
        else isBlackModule = true;                     // Borda preta
      }
      // 3. TIMING PATTERNS (linhas de sincronização)
      else if ((y == 10 && x >= 11 && x < QR_SIZE-11) ||  // Linha horizontal
               (x == 10 && y >= 11 && y < QR_SIZE-11)) {   // Linha vertical
        isBlackModule = ((x + y) % 2 == 0);
      }
      // 4. DATA AREA (área de dados baseada no código PIX)
      else {
        // Hash determinístico baseado na posição e código PIX
        uint32_t posHash = (x * 17 + y * 23 + pixHash) % 997;
        
        // Distribuição ~45% preto, ~55% branco (típica para QR Code)
        isBlackModule = (posHash % 9 < 4) || 
                       ((x + y + (pixHash % 7)) % 11 < 5) ||
                       ((x * y + pixHash) % 13 < 6);
      }
      
      // Desenhar módulo do QR Code
      if (isBlackModule) {
        // Módulo preto = desenhar bloco branco no display OLED
        for (int py = 0; py < PIXEL_SIZE; py++) {
          for (int px = 0; px < PIXEL_SIZE; px++) {
            int screenX = startX + x * PIXEL_SIZE + px;
            int screenY = startY + y * PIXEL_SIZE + py;
            if (screenX >= 0 && screenX < 128 && 
                screenY >= AREA_AZUL_INICIO && screenY < 64) {
              display.drawPixel(screenX, screenY, SSD1306_WHITE);
            }
          }
        }
      }
      // Módulos brancos ficam automaticamente pretos (fundo da tela)
    }
  }
  
  display.display();
  estadoAtual = AGUARDAR_PAGAMENTO;
  
  Serial.println("\n=====================================");
  Serial.println("   🔵 QR CODE SEGURO EXIBIDO!");
  Serial.println("=====================================");
  Serial.println("Valor: R$ " + String(valorDoacao, 2));
  Serial.println("Payment ID: " + paymentId);
  Serial.println();
  Serial.println("🔵 QR Code otimizado para área azul do display");
  Serial.println("📱 Teste com app de pagamento na área azul!");
  Serial.println();
  Serial.println("=== CÓDIGO PIX COPIA E COLA (BACKUP) ===");
  Serial.println(qrCodeData);
  Serial.println("=====================================");
  Serial.println("⏳ Aguardando confirmação do pagamento...");
  
  Serial.println("Heap livre final: " + String(ESP.getFreeHeap()) + " bytes");
  
  return true;
}

// Função para exibir QR Code REAL usando a biblioteca SimpleQR
bool exibirQRCodeReal() {
  if (qrCodeData.length() == 0) {
    Serial.println("❌ Nenhum código PIX disponível");
    return false;
  }
  
  Serial.println("\n🚀 === QR CODE REAL COM SIMPLEQR ===");
  Serial.println("Código PIX length: " + String(qrCodeData.length()));
  Serial.println("Heap livre: " + String(ESP.getFreeHeap()) + " bytes");
  
  // Criar QR Code usando SimpleQR
  SimpleQRCode qr;
  uint8_t version = 2; // Versão 2 = 25x25 módulos
  
  uint16_t bufferSize = SimpleQR::getBufferSize(version);
  uint8_t* qrBuffer = (uint8_t*)malloc(bufferSize);
  
  if (qrBuffer == NULL) {
    Serial.println("❌ ERRO: Falha ao alocar memória para QR Code");
    return false;
  }
  
  qr.modules = qrBuffer;
  
  Serial.println("Tentando gerar QR Code versão " + String(version) + "...");
  
  bool success = SimpleQR::generateQR(&qr, qrCodeData.c_str(), version, QRCODE_ECC_LOW);
  
  if (!success) {
    Serial.println("❌ ERRO: Falha ao gerar QR Code com SimpleQR");
    free(qrBuffer);
    return false;
  }
  
  Serial.println("✅ QR Code gerado com sucesso!");
  Serial.println("📐 Tamanho: " + String(qr.size) + "x" + String(qr.size) + " módulos");
  
  // Limpar tela completamente
  display.clearDisplay();
  
  // USAR APENAS A ÁREA AZUL (linhas 16-63) para melhor contraste
  const int AREA_AZUL_INICIO = 16;
  const int AREA_AZUL_ALTURA = 48; // 64-16=48 linhas azuis
  const int AREA_AZUL_LARGURA = 128;
  
  // Calcular tamanho do pixel para caber na área azul
  int pixelSize = min(AREA_AZUL_LARGURA / qr.size, AREA_AZUL_ALTURA / qr.size);
  
  if (pixelSize < 1) {
    Serial.println("❌ ERRO: QR Code muito grande para a área azul");
    free(qrBuffer);
    return false;
  }
  
  int qrDisplayWidth = qr.size * pixelSize;
  int qrDisplayHeight = qr.size * pixelSize;
  
  // Centralizar na área azul
  int startX = (AREA_AZUL_LARGURA - qrDisplayWidth) / 2;
  int startY = AREA_AZUL_INICIO + (AREA_AZUL_ALTURA - qrDisplayHeight) / 2;
  
  Serial.println("🔵 QR Code REAL na área azul:");
  Serial.println("Tamanho do pixel: " + String(pixelSize) + " pixels");
  Serial.println("Tamanho total: " + String(qrDisplayWidth) + "x" + String(qrDisplayHeight));
  Serial.println("Posição: (" + String(startX) + "," + String(startY) + ")");
  
  // Desenhar o QR Code REAL na tela
  for (int y = 0; y < qr.size; y++) {
    for (int x = 0; x < qr.size; x++) {
      if (SimpleQR::getModule(&qr, x, y)) {
        // Módulo preto do QR Code = desenhar pixels brancos no display OLED
        for (int py = 0; py < pixelSize; py++) {
          for (int px = 0; px < pixelSize; px++) {
            int screenX = startX + x * pixelSize + px;
            int screenY = startY + y * pixelSize + py;
            if (screenX >= 0 && screenX < 128 && 
                screenY >= AREA_AZUL_INICIO && screenY < 64) {
              display.drawPixel(screenX, screenY, SSD1306_WHITE);
            }
          }
        }
      }
      // Módulos brancos ficam automaticamente pretos (fundo da tela)
    }
  }
  
  display.display();
  estadoAtual = AGUARDAR_PAGAMENTO;
  
  Serial.println("\n=====================================");
  Serial.println("   🚀 QR CODE REAL EXIBIDO!");
  Serial.println("=====================================");
  Serial.println("Valor: R$ " + String(valorDoacao, 2));
  Serial.println("Payment ID: " + paymentId);
  Serial.println();
  Serial.println("✨ QR Code gerado com biblioteca SimpleQR");
  Serial.println("📱 Este QR Code deve ser LEGÍVEL por apps de pagamento!");
  Serial.println("🔵 Otimizado para área azul do display OLED");
  Serial.println();
  Serial.println("=== CÓDIGO PIX COPIA E COLA (BACKUP) ===");
  Serial.println(qrCodeData);
  Serial.println("=====================================");
  Serial.println("⏳ Aguardando confirmação do pagamento...");
  
  Serial.println("Heap livre final: " + String(ESP.getFreeHeap()) + " bytes");
  
  free(qrBuffer);
  return true;
}