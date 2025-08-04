#define ENABLE_DATABASE
#define ENABLE_USER_AUTH

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

// Firebase Includes
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

// Other Peripheral Includes
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <Preferences.h>

// --- Function Prototypes ---
void printOLED(const String& message, int textSize = 1, bool clear = true);
void streamCallback(AsyncResult &aResult);
void setupFirebase();
String saldoHtml();
void handleRoot();
void handleLogin();
void handleSetSaldo();
void startWebServer();

// --- Global Objects ---

// Firebase
WiFiClientSecure *ssl_client = nullptr;
AsyncClientClass *aClient = nullptr;
FirebaseApp app;
RealtimeDatabase Database;
UserAuth *user_auth = nullptr;

// Display OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// NTP (Time)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3 * 3600); // GMT-3

// Web Server
WebServer server(80);
Preferences preferences;

// --- OLED Display Functions ---

void printOLED(const String& message, int textSize, bool clear) {
  if (clear) {
    display.clearDisplay();
  }
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}

#include <ArduinoJson.h>

// --- Firebase Functions ---

void streamCallback(AsyncResult &aResult) {
    if (!aResult.isResult())
        return;

    if (aResult.isEvent()) {
        Serial.printf("Event: %s, Message: %s, Code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
    }

    if (aResult.available()) {
        RealtimeDatabaseResult &stream = aResult.to<RealtimeDatabaseResult>();
        Serial.printf("Stream Data Path: %s\n", stream.dataPath().c_str());
        Serial.printf("Stream Value: %s\n", stream.to<String>().c_str());
        Serial.printf("Is Stream: %s\n", stream.isStream() ? "true" : "false");
        Serial.printf("Stream Type: %d\n", stream.type());
        
        // Processa tanto mudanças no path raiz "/" quanto em "/status"
        if (stream.isStream() && (stream.dataPath() == "/" || stream.dataPath() == "/status")) {
            String dataValue = stream.to<String>();
            String status = "";
            
            // Se o path é "/status", o valor direto é o status
            if (stream.dataPath() == "/status") {
                status = dataValue;
                status.replace("\"", ""); // Remove aspas se existirem
                Serial.printf("Status direto recebido: %s\n", status.c_str());
            } 
            // Se o path é "/", precisa fazer parse JSON
            else if (stream.dataPath() == "/") {
                StaticJsonDocument<200> doc;
                DeserializationError error = deserializeJson(doc, dataValue);
                
                if (!error) {
                    status = doc["status"].as<String>();
                    Serial.printf("Status extraído do JSON: %s\n", status.c_str());
                } else {
                    Serial.printf("Erro no JSON parse: %s\n", error.f_str());
                    return;
                }
            }
            
            // Processa o status aprovado
            if (status == "approved") {
                Serial.println("Donation approved! Processing...");
                printOLED("Obrigado!", 2);
                delay(2000);
                printOLED("Aguardando...", 1);
                
                if (aClient) {
                    Serial.println("Atualizando status para 'processed'...");
                    Database.set<String>(*aClient, "/payment_status/status", "processed");
                    
                    // CRÍTICO: Reinicia o stream para escutar próximos pagamentos
                    Serial.println("Reiniciando stream para próximos pagamentos...");
                    delay(1000); // Pequena pausa para garantir que a atualização foi enviada
                    Database.get(*aClient, "/payment_status", streamCallback, true, "streamTask");
                }
            }
            else if (status == "processed") {
                Serial.println("Status já processado, aguardando próximo pagamento...");
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

  Serial.println("Inicializando Firebase App...");
  initializeApp(*aClient, app, getAuth(*user_auth), streamCallback, "authTask");
  
  Serial.println("Configurando Realtime Database...");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(FIREBASE_HOST);

  // Primeiro, reseta o status para garantir estado limpo
  Serial.println("Resetando status para estado inicial...");
  Database.set<String>(*aClient, "/payment_status/status", "waiting");
  
  delay(2000); // Aguarda a escrita ser processada
  
  Serial.println("Iniciando stream de monitoramento...");
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

  // Connect to Wi-Fi
  // Init OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha na alocacao do SSD1306"));
    for (;;);
  }
  printOLED("Conectando...", 1);

  // Connect to Wi-Fi
  Serial.print("Conectando ao Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());
  printOLED(WiFi.localIP().toString(), 1);
  delay(2000);

  // Sync Time
  Serial.println("Sincronizando hora com NTP...");
  timeClient.begin();
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Serial.println("Hora sincronizada.");
  Serial.println(timeClient.getFormattedTime());

  printOLED("Aguardando...", 1);

  // Start Services
  setupFirebase();
  startWebServer();
}

void loop() {
  app.loop();
  server.handleClient();
  delay(10);
}