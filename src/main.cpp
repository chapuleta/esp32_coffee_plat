#define ENABLE_DATABASE
#define ENABLE_LEGACY_TOKEN

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
LegacyToken *legacy_token = nullptr;

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

// --- Firebase Functions ---

void streamCallback(AsyncResult &aResult) {
    if (!aResult.isResult())
        return;

    if (aResult.isEvent()) {
        Serial.printf("Event: %s, Message: %s, Code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
    }

    if (aResult.available()) {
        RealtimeDatabaseResult &stream = aResult.to<RealtimeDatabaseResult>();
        if (stream.isStream() && stream.dataPath() == "/status") {
            String status = stream.to<String>();
            if (status == "approved") {
                Serial.println("Donation approved! Processing...");
                printOLED("Obrigado!", 2);
                delay(2000);
                printOLED("Aguardando...", 1);
                if (aClient) {
                    Database.set<String>(*aClient, "/payment_status/status", "processed");
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
  
  legacy_token = new LegacyToken(FIREBASE_AUTH);

  initializeApp(*aClient, app, getAuth(*legacy_token), streamCallback, "authTask");
  
  app.getApp<RealtimeDatabase>(Database);
  Database.url(FIREBASE_HOST);

  Database.get(*aClient, "/payment_status", streamCallback, true, "streamTask");
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