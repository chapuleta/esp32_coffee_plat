#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <qrcode.h>
#include "config.h"

// Declarações das funções
void mostrarMenuInicial();
void mostrarMenuSelecaoValor();
void mostrarInstrucoesSerial();
void processarComando(char comando);
void criarPagamento(float valor);
void mostrarQRCode();
void verificarPagamento();

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
        mostrarQRCode();
      } else {
        Serial.println("✗ Erro ao extrair QR Code - fim não encontrado");
      }
    } else {
      Serial.println("✗ QR Code não encontrado na resposta");
      Serial.println("Tentando buscar qr_code_base64...");
      
      // Tentar qr_code_base64 como alternativa
      int qrBase64Start = response.indexOf("\"qr_code_base64\":\"");
      if (qrBase64Start != -1) {
        qrBase64Start += 18; // Pular "qr_code_base64":"
        int qrBase64End = response.indexOf("\"", qrBase64Start);
        if (qrBase64End != -1) {
          String qrBase64 = response.substring(qrBase64Start, qrBase64End);
          Serial.println("✓ QR Code Base64 encontrado (length: " + String(qrBase64.length()) + ")");
          // Por enquanto, vamos usar o QR Code text mesmo se só temos base64
          // Em implementação futura, poderia decodificar o base64
          Serial.println("QR Code Base64 disponível, mas usando busca alternativa...");
        }
      }
      
      // Fallback: mostrar erro
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Erro: QR Code");
      display.println("nao disponivel");
      display.display();
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
  // Primeiro, desabilitar temporariamente o WiFi para evitar conflitos de semáforo
  WiFi.mode(WIFI_STA);
  delay(100);
  
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
  
  // SOLUÇÃO TEMPORÁRIA: Não gerar QR Code visual para evitar conflitos
  // Mostrar apenas informações textuais do PIX
  
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
  
  // Reabilitar WiFi
  delay(100);
  
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