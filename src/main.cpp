#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <qrcodegen.hpp>
#include "config.h"
#include <Preferences.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define OLED_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float saldoConta = 0.0;
String paymentId = "";
String qrCodeData = "";

String ultimoContribuidor = "N/A";
float ultimaContribuicao = 0.0;
String maiorContribuidor = "N/A";
float maiorContribuicao = 0.0;

// Função para exibir QR Code de qualquer link na tela OLED
void exibirQRCodeLink(String link) {
  if (link.length() == 0) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Erro: Link vazio");
    display.display();
    return;
  }
  Serial.println("\n🚀 === QR CODE DE LINK ===");
  Serial.println("Link: " + link);
  Serial.println("Heap livre: " + String(ESP.getFreeHeap()) + " bytes");
  try {
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(link.c_str(), qrcodegen::QrCode::Ecc::MEDIUM_ECC);
    display.clearDisplay();
    display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
    int pixelSize = std::min(128 / qr.getSize(), 64 / qr.getSize());
    int qrDisplayWidth = qr.getSize() * pixelSize;
    int qrDisplayHeight = qr.getSize() * pixelSize;
    int startX = (128 - qrDisplayWidth) / 2;
    int startY = (64 - qrDisplayHeight) / 2;
    for (int y = 0; y < qr.getSize(); y++) {
      for (int x = 0; x < qr.getSize(); x++) {
        if (qr.getModule(x, y)) {
          for (int py = 0; py < pixelSize; py++) {
            for (int px = 0; px < pixelSize; px++) {
              int screenX = startX + x * pixelSize + px;
              int screenY = startY + y * pixelSize + py;
              if (screenX >= 0 && screenX < 128 && screenY >= 0 && screenY < 64) {
                display.drawPixel(screenX, screenY, SSD1306_BLACK);
              }
            }
          }
        }
      }
    }
    display.display();
    Serial.println("QR code de link exibido!");
  } catch (const std::exception& e) {
    Serial.println("Erro ao gerar QR code de link: " + String(e.what()));
  }
}
#include <Adafruit_SSD1306.h>
#include <qrcodegen.hpp>  // Nova biblioteca QR Code profissional (Nayuki)
#include "config.h"
#include <Preferences.h>

extern Preferences preferences;
Preferences preferences; // Definindo a instância global de Preferences


// Declarações das funções
void mostrarMenuInicial();
void mostrarMenuSelecaoValor();
void mostrarInstrucoesSerial();
void processarComando(char comando);
void criarPagamento(float valor);
void mostrarQRCode();
void verificarPagamento();
bool exibirQRCodeReal(); // Função para QR Code REAL usando QRCodeGen (Nayuki)
void handleWebServer(); // Adicione a declaração da função para o linker
void verificarRailway() {
  // Função stub: implementar polling do endpoint Railway se necessário
}
void iniciarFluxoDoacaoNome(); // Declaração para linker

void receberDoacaoNome(String nome, float valor);

#include <WebServer.h>
WebServer server(80);

// Endpoint para servir o formulário HTML
void handleForm() {
  String html = "<html><head><title>Doação com Nome</title></head><body>";
  html += "<h2>Doação com Nome</h2>";
  html += "<form method='POST' action='/doacao_nome'>";
  html += "Nome: <input type='text' name='nome'><br>";
  html += "Valor: <input type='number' step='0.01' name='valor'><br>";
  html += "<input type='submit' value='Doar'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

// Endpoint para receber o POST do formulário
void handleDoacaoNome() {
  String nome = server.hasArg("nome") ? server.arg("nome") : "";
  float valor = server.hasArg("valor") ? server.arg("valor").toFloat() : 0.0;
  receberDoacaoNome(nome, valor);
  server.send(200, "text/html", "<html><body><h2>Obrigado pela doação!</h2></body></html>");
}

void startWebServer() {
  server.on("/form", handleForm);
  server.on("/doacao_nome", HTTP_POST, handleDoacaoNome);
  server.begin();
  Serial.println("Servidor web iniciado!");
}

void handleWebServer() {
  server.handleClient();
}

// Configurações da tela OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1

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
int numValoresPredefinidos = sizeof(valoresPredefinidos) / sizeof(valoresPredefinidos[0]);
int opcaoSelecionada = 0;

float valorDoacao = 0.0;

// Variáveis para valor personalizado
bool inserindoValorPersonalizado = false;
String valorPersonalizadoStr = "";

// Variáveis para controle de teclas especiais
String sequenciaEscape = "";
bool lendoSequenciaEscape = false;

// Novas variáveis para fluxo de doação
bool aguardandoTipoDoacao = false;
bool doacaoAnonima = true;
String nomeDoUsuario = "";

// ======= NOVO FLUXO: POLLING PARA NOME/VALOR DO RAILWAY =======
String railwayNome = "";
float railwayValor = 0.0;
bool aguardandoRailway = false;

void setup() {
  Serial.begin(115200);
  preferences.begin("doacoes", false);
  saldoConta = preferences.getFloat("saldo", 0.0);
  
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
  // Removido: exibição do saldo antes do WiFi conectar
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

  // Iniciar servidor web SOMENTE após WiFi conectar
  extern void startWebServer();
  startWebServer();

  mostrarMenuInicial();
  mostrarInstrucoesSerial();
}


// Variáveis globais para nome do usuário
bool aguardandoNomeUsuario = false;
String nomeUsuarioBuffer = "";

void loop() {

  if (aguardandoNomeUsuario) {
    // Espera o usuário digitar o nome no Serial
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        nomeUsuarioBuffer.trim();
        if (nomeUsuarioBuffer.length() > 0) {
          nomeDoUsuario = nomeUsuarioBuffer;
          aguardandoNomeUsuario = false;
          nomeUsuarioBuffer = "";
          Serial.println("Nome confirmado: " + nomeDoUsuario);
          mostrarMenuSelecaoValor();
          mostrarInstrucoesSerial();
        } else {
          Serial.println("Nome vazio! Digite novamente:");
        }
      } else if (c == 27) { // ESC cancela
        aguardandoNomeUsuario = false;
        nomeUsuarioBuffer = "";
        Serial.println("\nCancelado!");
        mostrarMenuInicial();
        mostrarInstrucoesSerial();
      } else {
        nomeUsuarioBuffer += c;
      }
    }
    return;
  }
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
  
    // Polling para nome/valor doador via endpoint externo
    if (aguardandoRailway) {
        verificarRailway();
        delay(2000); // Polling a cada 2 segundos
    }

    // Verificar pagamento se estiver aguardando
    if (estadoAtual == AGUARDAR_PAGAMENTO && paymentId != "") {
        verificarPagamento();
        delay(VERIFICACAO_PAGAMENTO_INTERVALO);
    }

    handleWebServer(); // Mantém o servidor web ativo

    // ======= AUTO-REFRESH DO DISPLAY OLED QUANDO O SALDO MUDA NA FLASH =======
    static float saldoContaAnterior = -9999.99;
    float saldoAtualFlash = preferences.getFloat("saldo", 0.0);
    if (fabs(saldoAtualFlash - saldoContaAnterior) > 0.001) {
        saldoContaAnterior = saldoAtualFlash;
        saldoConta = saldoAtualFlash;
        mostrarMenuInicial();
        // Opcional: mostrar instruções no serial
        mostrarInstrucoesSerial();
    }
}

// Função para buscar saldo da conta Mercado Pago
void atualizarSaldoConta() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  http.begin("https://api.mercadopago.com/v1/account/balance");
  http.addHeader("Authorization", "Bearer " + MERCADO_PAGO_ACCESS_TOKEN);
  int httpCode = http.GET();
  if (httpCode == 200) {
    String resp = http.getString();
    int balStart = resp.indexOf("\"available_balance\":");
    if (balStart != -1) {
      balStart += 20;
      int balEnd = resp.indexOf(".", balStart);
      if (balEnd != -1) balEnd += 3; // Pega até duas casas decimais
      else balEnd = resp.indexOf(",", balStart);
      if (balEnd == -1) balEnd = resp.length();
      String saldoStr = resp.substring(balStart, balEnd);
      saldoStr.trim();
      saldoConta = saldoStr.toFloat();
      Serial.println("💰 Saldo Mercado Pago atualizado: R$ " + String(saldoConta, 2));
    }
  } else {
    Serial.println("❌ Falha ao buscar saldo Mercado Pago: " + String(httpCode));
  }
  http.end();
}

void atualizarSaldoContaFlash(float novoSaldo) {
  saldoConta = novoSaldo;
  preferences.putFloat("saldo", saldoConta);
}

void mostrarMenuInicial() {
  saldoConta = preferences.getFloat("saldo", 0.0); // Atualiza saldo da flash
  atualizarSaldoConta(); // Atualiza saldo Mercado Pago
  display.clearDisplay();
  display.setTextSize(2); // Saldo em destaque
  display.setCursor(0,0);
  display.print("R$ ");
  display.println(saldoConta, 2);
  display.setTextSize(1);
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

void mostrarPerguntaTipoDoacao() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Como deseja doar?");
  display.println();
  display.println("1 - Anônimo");
  display.println("2 - Com nome");
  display.display();
  Serial.println("\nComo deseja doar?");
  Serial.println("1 - Anônimo");
  Serial.println("2 - Com nome");
  aguardandoTipoDoacao = true;
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
  
  if (aguardandoTipoDoacao) {
    if (comando == '1') {
      doacaoAnonima = true;
      aguardandoTipoDoacao = false;
      mostrarMenuSelecaoValor();
      mostrarInstrucoesSerial();
    } else if (comando == '2') {
      doacaoAnonima = false;
      aguardandoTipoDoacao = false;
      // Gera QR Code do formulário web e aguarda POST no endpoint local
      iniciarFluxoDoacaoNome();
    }
    return;
  }
  
  if (comando == '1' && estadoAtual == MENU_INICIAL) {
    mostrarPerguntaTipoDoacao();
    return;
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

void criarPagamentoRailwayFlow() {
  // Gera um QR code que aponta para a página Railway para entrada de nome/valor
  String sessionId = String(millis()) + String(random(1000,9999));
  String railwayUrl = "https://your-railway-app.com/donate?session=" + sessionId;

  // Exibe QR code do Railway na tela OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Acesse para doar:");
  display.println();
  display.println("Escaneie QR");
  display.display();

  // Gera e exibe QR code do Railway
  try {
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(railwayUrl.c_str(), qrcodegen::QrCode::Ecc::MEDIUM_ECC);
    int pixelSize = std::min(128 / qr.getSize(), 64 / qr.getSize());
    int startX = (128 - qr.getSize() * pixelSize) / 2;
    int startY = (64 - qr.getSize() * pixelSize) / 2;
    for (int y = 0; y < qr.getSize(); y++) {
      for (int x = 0; x < qr.getSize(); x++) {
        if (qr.getModule(x, y)) {
          for (int py = 0; py < pixelSize; py++) {
            for (int px = 0; px < pixelSize; px++) {
              int screenX = startX + x * pixelSize + px;
              int screenY = startY + y * pixelSize + py;
              if (screenX >= 0 && screenX < 128 && screenY >= 0 && screenY < 64) {
                display.drawPixel(screenX, screenY, SSD1306_BLACK);
              }
            }
          }
        }
      }
    }
    display.display();
    Serial.println("QR code Railway exibido: " + railwayUrl);
  } catch (const std::exception& e) {
    Serial.println("Erro ao gerar QR code Railway: " + String(e.what()));
  }
  // TODO: Implementar polling para Railway API para receber nome/valor e gerar QR de pagamento
}

void mostrarPerguntaNomeUsuario() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Digite seu nome no Serial");
  display.display();
  Serial.println("\nDigite seu nome e pressione ENTER:");
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
  if (doacaoAnonima) {
    payer["email"] = "anonimo@exemplo.com";
    payer["first_name"] = "Anônimo";
  } else {
    payer["email"] = "doador@exemplo.com";
    payer["first_name"] = nomeDoUsuario;
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.println("Enviando requisição para Mercado Pago...");
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode == 201) {
    String response = http.getString();
    Serial.println("Pagamento criado com sucesso!");
    
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
        
        if (exibirQRCodeReal()) {
          Serial.println("✅ QR Code REAL exibido com sucesso!");
        } else {
          Serial.println("❌ Falha ao exibir QR Code. Usando interface textual.");
          mostrarQRCode();
        }
      } else {
         Serial.println("✗ Erro ao extrair QR Code - fim não encontrado");
         mostrarQRCode();
      }
    } else {
        Serial.println("✗ Erro ao extrair QR Code da resposta da API.");
        mostrarQRCode(); // Fallback para interface de texto
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

          // ===== EXTRAIR VALOR DO PAGAMENTO DA RESPOSTA =====
          float valorPago = 0.0;
          int valorStart = response.indexOf("\"transaction_amount\":");
          if (valorStart != -1) {
            valorStart += 21; // Pular "transaction_amount":
            int valorEnd = response.indexOf(",", valorStart);
            if (valorEnd == -1) valorEnd = response.indexOf("}", valorStart);
            if (valorEnd != -1) {
              String valorStr = response.substring(valorStart, valorEnd);
              valorStr.trim();
              valorPago = valorStr.toFloat();
              Serial.println("💵 Valor pago extraído da resposta: R$ " + String(valorPago, 2));
            }
          }
          if (valorPago <= 0.0) valorPago = valorDoacao; // Fallback

          // ===== EXTRAIR DADOS DO PAGADOR =====
          String nomeDoador = "PIX";
          String emailDoador = "";
          String payerId = "";
          int payerIdStart = response.indexOf("\"payer\":");
          if (payerIdStart != -1) {
            int idStart = response.indexOf("\"id\":\"", payerIdStart);
            if (idStart != -1) {
              idStart += 7; // Pular "id":"
              int idEnd = response.indexOf("\"", idStart);
              if (idEnd != -1) {
                payerId = response.substring(idStart, idEnd);
                payerId.trim();
                Serial.println("👤 Payer ID encontrado: " + payerId);
              }
            }
          }
          // Buscar nome completo via API de clientes Mercado Pago
          String firstName = "";
          String lastName = "";
          if (payerId.length() > 0 && payerId != "null") {
            HTTPClient httpCustomer;
            String customerUrl = "https://api.mercadopago.com/v1/customers/" + payerId;
            httpCustomer.begin(customerUrl);
            httpCustomer.addHeader("Authorization", "Bearer " + MERCADO_PAGO_ACCESS_TOKEN);
            int customerCode = httpCustomer.GET();
            if (customerCode == 200) {
              String customerResp = httpCustomer.getString();
              int fnStart = customerResp.indexOf("\"first_name\":\"");
              if (fnStart != -1) {
                fnStart += 15;
                int fnEnd = customerResp.indexOf("\"", fnStart);
                if (fnEnd != -1) {
                  firstName = customerResp.substring(fnStart, fnEnd);
                  firstName.trim();
                }
              }
              int lnStart = customerResp.indexOf("\"last_name\":\"");
              if (lnStart != -1) {
                lnStart += 14;
                int lnEnd = customerResp.indexOf("\"", lnStart);
                if (lnEnd != -1) {
                  lastName = customerResp.substring(lnStart, lnEnd);
                  lastName.trim();
                }
              }
              if (firstName.length() > 0 || lastName.length() > 0) {
                nomeDoador = firstName;
                if (lastName.length() > 0) nomeDoador += " " + lastName;
                Serial.println("👤 Nome completo do doador: " + nomeDoador);
              }
            } else {
              Serial.println("❌ Falha ao buscar nome completo do doador (API customers)");
            }
            httpCustomer.end();
          }

          // ===== ATUALIZAR DADOS DE DOAÇÃO =====
          ultimaContribuicao = valorPago;
          ultimoContribuidor = nomeDoador;
          saldoConta += valorPago;
          atualizarSaldoContaFlash(saldoConta); // Salva saldo na flash

          Serial.println("💰 Atualizando saldo: R$ " + String(saldoConta, 2));
          Serial.println("👤 Último doador: " + ultimoContribuidor);
          Serial.println("💵 Última contribuição: R$ " + String(ultimaContribuicao, 2));

          if (valorPago > maiorContribuicao) {
            maiorContribuicao = valorPago;
            maiorContribuidor = ultimoContribuidor;
            Serial.println(String("🏆 NOVO MAIOR DOADOR: ") + String(maiorContribuicao) + " - R$ " + String(maiorContribuicao, 2));
          }

          // Mostrar confirmação na tela com dados atualizados
          display.clearDisplay();
          display.setTextSize(1);
          display.setCursor(0,0);
          display.println("PAGAMENTO APROVADO!");
          display.println();
          display.print("Doador: ");
          display.println(nomeDoador);
          display.print("Valor: R$ ");
          display.println(valorPago, 2);
          display.println();
          display.print("Saldo total:");
          display.print("R$ ");
          display.println(saldoConta, 2);
          display.println();
          display.println("Obrigado!");
          display.display();

          // Salvar saldo atualizado na flash
          atualizarSaldoContaFlash(saldoConta);

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

// Função para exibir QR Code REAL usando a biblioteca QRCodeGen (Nayuki)
bool exibirQRCodeReal() {
  if (qrCodeData.length() == 0) {
    Serial.println("❌ Nenhum código PIX disponível");
    return false;
  }
  
  Serial.println("\n🚀 === QR CODE REAL COM QRCODEGEN (NAYUKI) ===");
  Serial.println("Código PIX length: " + String(qrCodeData.length()));
  Serial.println("Heap livre: " + String(ESP.getFreeHeap()) + " bytes");
  
  try {
    // Criar QR Code usando QRCodeGen (biblioteca profissional)
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(qrCodeData.c_str(), qrcodegen::QrCode::Ecc::MEDIUM_ECC);
    
    Serial.println("✅ QR Code gerado com sucesso!");
    Serial.println("📐 Tamanho: " + String(qr.getSize()) + "x" + String(qr.getSize()) + " módulos");
    Serial.println("📋 Versão: " + String(qr.getVersion()));
    
    // Limpar tela completamente
    display.clearDisplay();
    
    // PREENCHER TODA A TELA COM BRANCO (fundo branco)
    display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
    
    // USAR TODA A TELA 128x64 para maximizar o QR Code
    const int DISPLAY_WIDTH = 128;
    const int DISPLAY_HEIGHT = 64;
    
    // Calcular tamanho do pixel para usar toda a tela
    int pixelSize = std::min(DISPLAY_WIDTH / qr.getSize(), DISPLAY_HEIGHT / qr.getSize());
    
    if (pixelSize < 1) {
      Serial.println("❌ ERRO: QR Code muito grande para a tela");
      Serial.println("   QR Size: " + String(qr.getSize()) + "x" + String(qr.getSize()));
      Serial.println("   Tela disponível: " + String(DISPLAY_WIDTH) + "x" + String(DISPLAY_HEIGHT));
      return false;
    }
    
    int qrDisplayWidth = qr.getSize() * pixelSize;
    int qrDisplayHeight = qr.getSize() * pixelSize;
    
    // Centralizar na tela inteira
    int startX = (DISPLAY_WIDTH - qrDisplayWidth) / 2;
    int startY = (DISPLAY_HEIGHT - qrDisplayHeight) / 2;
    
    Serial.println(" QR Code REAL na tela completa:");
    Serial.println("Tamanho do pixel: " + String(pixelSize) + " pixels");
    Serial.println("Tamanho total: " + String(qrDisplayWidth) + "x" + String(qrDisplayHeight));
    Serial.println("Posição: (" + String(startX) + "," + String(startY) + ")");
    
    // Desenhar o QR Code REAL na tela
    for (int y = 0; y < qr.getSize(); y++) {
      for (int x = 0; x < qr.getSize(); x++) {
        if (qr.getModule(x, y)) {
          // Módulo preto do QR Code = desenhar pixels PRETOS no display OLED (fundo branco)
          for (int py = 0; py < pixelSize; py++) {
            for (int px = 0; px < pixelSize; px++) {
              int screenX = startX + x * pixelSize + px;
              int screenY = startY + y * pixelSize + py;
              if (screenX >= 0 && screenX < DISPLAY_WIDTH && 
                  screenY >= 0 && screenY < DISPLAY_HEIGHT) {
                display.drawPixel(screenX, screenY, SSD1306_BLACK);
              }
            }
          }
        }
        // Módulos brancos do QR Code ficam automaticamente brancos (fundo já é branco)
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
    Serial.println("✨ QR Code gerado com biblioteca QRCodeGen (Nayuki)");
    Serial.println("📱 Este QR Code DEVE ser LEGÍVEL por qualquer app de pagamento!");
    Serial.println("️ Usando toda a tela 128x64 para máxima qualidade");
    Serial.println();
    Serial.println("=== CÓDIGO PIX COPIA E COLA (BACKUP) ===");
    Serial.println(qrCodeData);
    Serial.println("=====================================");
    Serial.println("⏳ Aguardando confirmação do pagamento...");
    
    Serial.println("Heap livre final: " + String(ESP.getFreeHeap()) + " bytes");
    
    return true;
    
  } catch (const std::exception& e) {
    Serial.println("❌ ERRO ao gerar QR Code: " + String(e.what()));
    Serial.println("Heap livre: " + String(ESP.getFreeHeap()) + " bytes");
    return false;
  }
}

// ======= NOVO FLUXO: POLLING PARA NOME/VALOR DO RAILWAY =======
// NOVO FLUXO: Recebe nome/valor via endpoint local do ESP32
void iniciarFluxoDoacaoNome() {
    // Gera QR code para página web externa onde o doador informa nome/valor
    String urlForm = "https://seu-formulario.com?destino=http://" + WiFi.localIP().toString() + ":8080/doacao_nome";
    exibirQRCodeLink(urlForm);
    Serial.println("\nAguardando envio de nome/valor via endpoint local...");
}

// Função para ser chamada pelo endpoint /doacao_nome
void receberDoacaoNome(String nome, float valor) {
    if (nome.length() > 0 && valor > 0.0) {
        nomeDoUsuario = nome;
        valorDoacao = valor;
        criarPagamento(valor);
    }
}