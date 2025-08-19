#define ENABLE_DATABASE
#define ENABLE_USER_AUTH
#define BUTTON_PIN 4 // Pino do botão. Mude se necessário.

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

// Firebase Includes
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

// Other Peripheral Includes
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h> // Nokia 5110 LCD library
#include <SPI.h>              // For Nokia 5110 LCD
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <qrcodegen.hpp>

// Nokia 5110 LCD Pin Definitions (from GEMINI.md)
#define SCLK_PIN 18
#define DIN_PIN 23
#define DC_PIN 19
#define CS_PIN 5
#define RST_PIN 14

// --- Function Prototypes ---
void printOLED(const String& message, int textSize = 1, bool clear = true);
String removeAccents(String input); // New prototype
void streamCallback(AsyncResult &aResult);
void setupFirebase();
String saldoHtml();
void handleRoot();
void handleLogin();
void handleSetSaldo();
void startWebServer();
void readDonationData();
void checkDataUpdateComplete();
void updateDisplay();
void updateMarqueeText();
void displayScrollingLine(String& textToScroll, int& scrollPos, unsigned long& lastScrollTimeVar, int yPos);
void displayQRCode();
void onButtonPress();
String extractFirstTwoNames(String fullName);


// --- Global Objects ---

// Firebase
WiFiClientSecure *ssl_client = nullptr;
AsyncClientClass *aClient = nullptr;
FirebaseApp app;
RealtimeDatabase Database;
UserAuth *user_auth = nullptr;

// Display Nokia 5110
Adafruit_PCD8544 display = Adafruit_PCD8544(SCLK_PIN, DIN_PIN, DC_PIN, CS_PIN, RST_PIN);

// NTP (Time)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3 * 3600); // GMT-3

// Web Server
WebServer server(80);
Preferences preferences;

// Donation Data
String totalAmount = "0.00";
String lastDonor = "Nenhum ainda";
String topDonor = "Nenhum ainda";
float topAmount = 0.0;
bool dataLoaded = false; // Flag para evitar leituras múltiplas
int dataUpdatesReceived = 0; // Contador para saber quando todas as leituras terminaram

// Display State
bool showingQR = false;
unsigned long qrStartTime = 0;
String marqueeTextLine1 = "";
String marqueeTextLine2 = "";
int scrollPositionLine1 = 0;
int scrollPositionLine2 = 0;
unsigned long lastScrollTimeLine1 = 0;
unsigned long lastScrollTimeLine2 = 0;
const int SCROLL_DELAY = 200; // ms
const int QR_DISPLAY_TIME = 30000; // 30s

// --- Nokia 5110 Display Functions ---

void printOLED(const String& message, int textSize, bool clear) {
  if (clear) {
    display.clearDisplay();
  }
  display.setTextSize(textSize);
  display.setTextColor(BLACK); // Changed from SSD1306_WHITE to BLACK for Nokia 5110
  display.setCursor(0, 0);
  display.print(removeAccents(message)); // Changed from println to print to avoid line breaks
  display.display();
}

// New function to remove accents
String removeAccents(String input) {
    input.replace("á", "a"); input.replace("Á", "A");
    input.replace("é", "e"); input.replace("É", "E");
    input.replace("í", "i"); input.replace("Í", "I");
    input.replace("ó", "o"); input.replace("Ó", "O");
    input.replace("ú", "u"); input.replace("Ú", "U");
    input.replace("â", "a"); input.replace("Â", "A");
    input.replace("ê", "e"); input.replace("Ê", "E");
    input.replace("ô", "o"); input.replace("Ô", "O");
    input.replace("ç", "c"); input.replace("Ç", "C");
    input.replace("ã", "a"); input.replace("Ã", "A");
    input.replace("õ", "o"); input.replace("Õ", "O");
    return input;
}

String extractFirstTwoNames(String fullName) {
    int firstSpace = fullName.indexOf(' ');
    if (firstSpace == -1) return fullName;
    
    int secondSpace = fullName.indexOf(' ', firstSpace + 1);
    if (secondSpace == -1) return fullName;
    
    return fullName.substring(0, secondSpace);
}

void updateMarqueeText() {
    String topDonorShort = removeAccents(extractFirstTwoNames(topDonor)); // Applied removeAccents
    String lastDonorShort = removeAccents(extractFirstTwoNames(lastDonor)); // Applied removeAccents
    
    // Split into two lines
    marqueeTextLine1 = removeAccents("Maior doador: ") + topDonorShort + removeAccents(" (R$") + String(topAmount, 2) + ")     "; // Applied removeAccents
    marqueeTextLine2 = removeAccents("Ultimo doador: ") + lastDonorShort + "     "; // Applied removeAccents
}

// Generic scrolling function - cria substring visível para evitar quebra de linha
void displayScrollingLine(String& textToScroll, int& scrollPos, unsigned long& lastScrollTimeVar, int yPos) {
    if (millis() - lastScrollTimeVar >= SCROLL_DELAY) {
        scrollPos++;
        // Reinicia quando o scroll passou por todo o texto
        if (scrollPos >= textToScroll.length()) { 
            scrollPos = 0;
        }
        lastScrollTimeVar = millis();
    }
    
    // Limpa apenas a região específica desta linha (altura de 8 pixels para font size 1)
    display.fillRect(0, yPos, display.width(), 8, WHITE);
    
    // Define configurações de texto
    display.setTextSize(1);
    display.setTextColor(BLACK);
    
    // Nokia 5110 tem 84 pixels de largura, com fonte size 1 cabe ~14 caracteres
    const int maxChars = 14;
    
    // Cria substring que vai ser exibida (rotacionando através do texto)
    String visibleText = "";
    for (int i = 0; i < maxChars; i++) {
        int charIndex = (scrollPos + i) % textToScroll.length();
        visibleText += textToScroll.charAt(charIndex);
    }
    
    // Desenha o texto visível na posição fixa
    display.setCursor(0, yPos);
    display.print(visibleText);
}

void displayQRCode() {
    display.clearDisplay();
    
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(
        "https://webhook-coffee.vercel.app",
        qrcodegen::QrCode::Ecc::MEDIUM_ECC
    );
    
    int size = qr.getSize();
    int scale = min((display.width()-10) / size, (display.height()-10) / size);
    int offsetX = (display.width() - size * scale) / 2;
    int offsetY = (display.height() - size * scale) / 2;
    
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qr.getModule(x, y)) {
                display.fillRect(offsetX + x * scale, offsetY + y * scale,
                               scale, scale, BLACK); // Changed from SSD1306_WHITE to BLACK
            }
        }
    }
    
    display.display();
}

void onButtonPress() {
    if (!showingQR) {
      Serial.println("Botão pressionado! Exibindo QR Code.");
      showingQR = true;
      qrStartTime = millis();
      displayQRCode();
    }
}

void updateDisplay() {
    if (showingQR) {
        if (millis() - qrStartTime >= QR_DISPLAY_TIME) {
            showingQR = false;
            scrollPositionLine1 = 0; // Reset scroll for both lines
            scrollPositionLine2 = 0;
        } else {
            // QR já está na tela, não faz nada
            return;
        }
    }
    
    // Layout simples e limpo
    display.clearDisplay();
    
    // Linha 0: Instrução fixa (sem rolagem)
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0, 0);
    display.print("Botao = QR");
    
    // Prepara strings para rolagem
    String topDonorShort = extractFirstTwoNames(topDonor);
    String topLine = "Top: " + removeAccents(topDonorShort) + " (R$" + String(topAmount, 2) + ")     ";
    
    String lastDonorShort = extractFirstTwoNames(lastDonor);
    String lastLine = "Ult: " + removeAccents(lastDonorShort) + "     ";
    
    // Linhas com rolagem
    displayScrollingLine(topLine, scrollPositionLine1, lastScrollTimeLine1, 12);
    displayScrollingLine(lastLine, scrollPositionLine2, lastScrollTimeLine2, 30);
    
    display.display();
}


// --- Firebase Functions ---

void readDonationData() {
  if (!aClient) {
    Serial.println("ERRO: aClient não disponível para leitura");
    return;
  }
  
  dataUpdatesReceived = 0;
  Serial.println("=== INICIANDO LEITURA DOS DADOS ===");
  
  Database.get(*aClient, "/donations/total_amount", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      totalAmount = removeAccents(aResult.to<RealtimeDatabaseResult>().to<String>()); // Applied removeAccents
      Serial.printf("✅ Total amount updated: %s\n", totalAmount.c_str());
    } else {
      Serial.printf("❌ Erro ao ler total_amount: %s\n", aResult.uid().c_str());
    }
    checkDataUpdateComplete();
  }, false, "readTotal");
  
  Database.get(*aClient, "/donations/last_donor", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      lastDonor = removeAccents(aResult.to<RealtimeDatabaseResult>().to<String>()); // Applied removeAccents
      Serial.printf("✅ Last donor updated: %s\n", lastDonor.c_str());
    }
    else {
      Serial.printf("❌ Erro ao ler last_donor: %s\n", aResult.uid().c_str());
    }
    checkDataUpdateComplete();
  }, false, "readLast");
  
  Database.get(*aClient, "/donations/top_donor", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      topDonor = removeAccents(aResult.to<RealtimeDatabaseResult>().to<String>()); // Applied removeAccents
      Serial.printf("✅ Top donor updated: %s\n", topDonor.c_str());
    }
    else {
      Serial.printf("❌ Erro ao ler top_donor: %s\n", aResult.uid().c_str());
    }
    checkDataUpdateComplete();
  }, false, "readTop");

  Database.get(*aClient, "/donations/top_amount", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      topAmount = aResult.to<RealtimeDatabaseResult>().to<float>();
      Serial.printf("✅ Top amount updated: %.2f\n", topAmount);
    } else {
      Serial.printf("❌ Erro ao ler top_amount: %s\n", aResult.uid().c_str());
    }
    checkDataUpdateComplete();
  }, false, "readTopAmount");
}

void checkDataUpdateComplete() {
  dataUpdatesReceived++;
  Serial.printf("📊 Updates recebidos: %d/4\n", dataUpdatesReceived);
  
  if (dataUpdatesReceived >= 4) {
    Serial.println("🎉 Todos os dados carregados!");
    dataUpdatesReceived = 0; // Reset
  }
}

void streamCallback(AsyncResult &aResult) {
    if (!aResult.isResult())
        return;

    if (aResult.isEvent()) {
        Serial.printf("Event: %s, Message: %s, Code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
        
        if (aResult.eventLog().code() == 10 && aResult.uid() == "authTask" && !dataLoaded) {
            Serial.println("🎉 Firebase autenticado! Lendo dados das doações...");
            dataLoaded = true;
            delay(1000);
            readDonationData();
        }
    }

    if (aResult.available()) {
        RealtimeDatabaseResult &stream = aResult.to<RealtimeDatabaseResult>();
        
        if (aResult.uid() == "streamTask" && stream.isStream() && (stream.dataPath() == "/" || stream.dataPath() == "/status")) {
            String status = "";
            if (stream.dataPath() == "/status") {
                status = stream.to<String>();
            } else if (stream.dataPath() == "/") {
                JsonDocument doc;
                if (deserializeJson(doc, stream.to<String>()) == DeserializationError::Ok) {
                    status = doc["status"].as<String>();
                }
            }
            
            if (status == "approved") {
                Serial.println("Donation approved! Processing...");
                printOLED("Obrigado!", 2, true);
                delay(2000);
                readDonationData(); // Atualiza todos os dados
                
                if (aClient) {
                    Database.set<String>(*aClient, "/payment_status/status", "processed");
                    delay(1000);
                    Database.get(*aClient, "/payment_status", streamCallback, true, "streamTask");
                }
            }
        }
    }
}

void setupFirebase() {
  Serial.println("Configuring Firebase...");

  ssl_client = new WiFiClientSecure();
  ssl_client->setInsecure();
  
  aClient = new AsyncClientClass(*ssl_client);
  
  user_auth = new UserAuth(FIREBASE_API_KEY, FIREBASE_USER_EMAIL, FIREBASE_USER_PASSWORD);

  initializeApp(*aClient, app, getAuth(*user_auth), streamCallback, "authTask");
  
  app.getApp<RealtimeDatabase>(Database);
  Database.url(FIREBASE_HOST);

  Database.set<String>(*aClient, "/payment_status/status", "waiting");
  delay(1000);
  Database.get(*aClient, "/payment_status", streamCallback, true, "streamTask");
  
  Serial.println("Firebase configurado com sucesso!");
}

// --- Web Server Functions ---
const char* login_username = "admin";
const char* login_password = "senha123";

String saldoHtml() {
  float saldo = preferences.getFloat("saldo", 0.0);
  return String("<h2>Saldo Total: R$ ") + String(saldo, 2) + "</h2>";
}

void handleRoot() {
  String html = "<form method='POST' action='/login'>'"
               "Usuario: <input name='username'><br>"
               "Senha: <input type='password' name='password'><br>"
               "<input type='submit' value='Entrar'>"
               "</form>";
  server.send(200, "text/html", html);
}

void handleLogin() {
  if (server.method() == HTTP_POST) {
    if (server.arg("username") == login_username && server.arg("password") == login_password) {
      String html = saldoHtml();
      html += "<form method='POST' action='/set_saldo'>"
              "Novo saldo: <input name='novo_saldo'><br>"
              "<input type='submit' value='Alterar'>"
              "</form>";
      server.send(200, "text/html", html);
    } else {
      server.send(401, "text/html", "Login invalido");
    }
  } else {
    server.send(405, "text/html", "Metodo nao permitido");
  }
}

void handleSetSaldo() {
  if (server.method() == HTTP_POST) {
    float novoSaldo = server.arg("novo_saldo").toFloat();
    preferences.putFloat("saldo", novoSaldo);
    String html = saldoHtml();
    html += "<p>Saldo alterado com sucesso!</p><br><a href='/'>Voltar</a>";
    server.send(200, "text/html", html);
  } else {
    server.send(405, "text/html", "Metodo nao permitido");
  }
}

void startWebServer() {
  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/set_saldo", handleSetSaldo);
  server.begin();
  Serial.println("Servidor web iniciado.");
}

// --- Main Setup and Loop ---

void setup() {
  Serial.begin(115200);
  preferences.begin("coffee-app", false);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Init OLED
  display.begin(); // Initialize PCD8544
  display.setContrast(55); // Set contrast for Nokia 5110
  // No error check for begin() as it doesn't return a boolean for PCD8544
  printOLED("Conectando...", 1, true);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());
  printOLED(WiFi.localIP().toString(), 1, true);
  delay(2000);

  // Sync Time
  timeClient.begin();
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Serial.println("Hora sincronizada.");

  // Start Services
  setupFirebase();
  startWebServer();
}

void loop() {
  app.loop();
  server.handleClient();
  
  if (digitalRead(BUTTON_PIN) == HIGH) { // Changed from LOW to HIGH
    onButtonPress();
    delay(200); // Debounce
  }
  
  if (dataLoaded) {
    updateDisplay();
  }
  
  delay(10);
}
