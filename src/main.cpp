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
#include <ArduinoJson.h>

// --- Function Prototypes ---
void printOLED(const String& message, int textSize = 1, bool clear = true);
void streamCallback(AsyncResult &aResult);
void setupFirebase();
String saldoHtml();
void handleRoot();
void handleLogin();
void handleSetSaldo();
void startWebServer();
void readDonationData();
void displayInfo();
void checkDataUpdateComplete();
void checkForDataUpdates(); // Nova função para verificar mudanças periodicamente

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

// Donation Data
String totalAmount = "0.00";
String lastDonor = "Nenhum ainda";
String topDonor = "Nenhum ainda";
bool dataLoaded = false; // Flag para evitar leituras múltiplas
int dataUpdatesReceived = 0; // Contador para saber quando todas as 3 leituras terminaram
unsigned long lastDataCheck = 0; // Timestamp da última verificação de dados
const unsigned long DATA_CHECK_INTERVAL = 10000; // Verifica a cada 10 segundos

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

void readDonationData() {
  if (!aClient) {
    Serial.println("ERRO: aClient não disponível para leitura");
    return;
  }
  
  Serial.println("=== INICIANDO LEITURA DOS DADOS ===");
  
  // Primeiro, vamos ler todo o nó donations para ver o que existe
  Database.get(*aClient, "/donations", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      RealtimeDatabaseResult &dbResult = aResult.to<RealtimeDatabaseResult>();
      String allData = dbResult.to<String>();
      Serial.printf("🔍 TODOS OS DADOS EM /donations: '%s'\n", allData.c_str());
    } else {
      Serial.println("❌ Erro ao ler /donations completo");
    }
  }, false, "readAll");
  
  Database.get(*aClient, "/donations/total_amount", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      RealtimeDatabaseResult &dbResult = aResult.to<RealtimeDatabaseResult>();
      String rawValue = dbResult.to<String>();
      Serial.printf("Raw total_amount: '%s'\n", rawValue.c_str());
      
      if (rawValue.length() > 0) {
        totalAmount = rawValue;
        Serial.printf("✅ Total amount updated: %s\n", totalAmount.c_str());
      } else {
        Serial.println("❌ Total amount está vazio no Firebase");
      }
      checkDataUpdateComplete(); // Verifica se pode atualizar display
    } else {
      Serial.printf("❌ Erro ao ler total_amount: %s\n", aResult.uid().c_str());
      checkDataUpdateComplete(); // Conta mesmo se houver erro
    }
  }, false, "readTotal");
  
  Database.get(*aClient, "/donations/last_donor", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      RealtimeDatabaseResult &dbResult = aResult.to<RealtimeDatabaseResult>();
      String rawValue = dbResult.to<String>();
      Serial.printf("Raw last_donor: '%s'\n", rawValue.c_str());
      
      if (rawValue.length() > 0) {
        lastDonor = rawValue;
        lastDonor.replace("\"", ""); // Remove aspas se existirem
        Serial.printf("✅ Last donor updated: %s\n", lastDonor.c_str());
      } else {
        Serial.println("❌ Last donor está vazio no Firebase");
      }
      checkDataUpdateComplete(); // Verifica se pode atualizar display
    } else {
      Serial.printf("❌ Erro ao ler last_donor: %s\n", aResult.uid().c_str());
      checkDataUpdateComplete(); // Conta mesmo se houver erro
    }
  }, false, "readLast");
  
  Database.get(*aClient, "/donations/top_donor", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      RealtimeDatabaseResult &dbResult = aResult.to<RealtimeDatabaseResult>();
      String rawValue = dbResult.to<String>();
      Serial.printf("Raw top_donor: '%s'\n", rawValue.c_str());
      
      if (rawValue.length() > 0) {
        topDonor = rawValue;
        topDonor.replace("\"", ""); // Remove aspas se existirem
        Serial.printf("✅ Top donor updated: %s\n", topDonor.c_str());
      } else {
        Serial.println("❌ Top donor está vazio no Firebase");
      }
      checkDataUpdateComplete(); // Verifica se pode atualizar display
    } else {
      Serial.printf("❌ Erro ao ler top_donor: %s\n", aResult.uid().c_str());
      checkDataUpdateComplete(); // Conta mesmo se houver erro
    }
  }, false, "readTop");
  
  Serial.println("=== SOLICITAÇÕES DE LEITURA ENVIADAS ===");
}

void checkDataUpdateComplete() {
  dataUpdatesReceived++;
  Serial.printf("📊 Updates recebidos: %d/3\n", dataUpdatesReceived);
  
  if (dataUpdatesReceived >= 3) {
    Serial.println("🎉 Todos os dados carregados! Atualizando display...");
    displayInfo();
    dataUpdatesReceived = 0; // Reset para próximas leituras
  }
}

void checkForDataUpdates() {
  // Só verifica se passou o intervalo de tempo
  if (millis() - lastDataCheck < DATA_CHECK_INTERVAL) {
    return;
  }
  
  lastDataCheck = millis();
  Serial.println("🔄 Verificando atualizações nos dados de doações...");
  
  // Resetar o contador para não interferir com o display
  dataUpdatesReceived = 0;
  
  // Lê apenas uma vez para comparar com os dados atuais
  Database.get(*aClient, "/donations/total_amount", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      RealtimeDatabaseResult &dbResult = aResult.to<RealtimeDatabaseResult>();
      String rawValue = dbResult.to<String>();
      rawValue.replace("\"", "");
      
      if (rawValue.length() > 0 && rawValue != totalAmount) {
        Serial.printf("💰 Total amount mudou de %s para %s\n", totalAmount.c_str(), rawValue.c_str());
        totalAmount = rawValue;
        displayInfo();
      }
    }
  }, false, "checkTotal");
  
  Database.get(*aClient, "/donations/last_donor", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      RealtimeDatabaseResult &dbResult = aResult.to<RealtimeDatabaseResult>();
      String rawValue = dbResult.to<String>();
      rawValue.replace("\"", "");
      
      if (rawValue.length() > 0 && rawValue != lastDonor) {
        Serial.printf("👤 Last donor mudou de %s para %s\n", lastDonor.c_str(), rawValue.c_str());
        lastDonor = rawValue;
        displayInfo();
      }
    }
  }, false, "checkLast");
  
  Database.get(*aClient, "/donations/top_donor", [](AsyncResult &aResult) {
    if (aResult.isResult()) {
      RealtimeDatabaseResult &dbResult = aResult.to<RealtimeDatabaseResult>();
      String rawValue = dbResult.to<String>();
      rawValue.replace("\"", "");
      
      if (rawValue.length() > 0 && rawValue != topDonor) {
        Serial.printf("🏆 Top donor mudou de %s para %s\n", topDonor.c_str(), rawValue.c_str());
        topDonor = rawValue;
        displayInfo();
      }
    }
  }, false, "checkTop");
}

void displayInfo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Primeira linha: Saldo (maior)
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("R$ ");
  display.println(totalAmount);
  
  // Voltar para tamanho normal
  display.setTextSize(1);
  
  // Segunda linha: Maior doador
  display.setCursor(0, 20);
  display.print("Maior: ");
  display.println(topDonor.substring(0, 15)); // Mais caracteres
  
  // Terceira linha: Último doador
  display.setCursor(0, 30);
  display.print("Ultimo: ");
  display.println(lastDonor.substring(0, 14)); // Mais caracteres
  
  // Quarta linha: Call to action
  display.setCursor(0, 45);
  display.println("Doe via formulario web");
  
  // Quinta linha: Status
  display.setCursor(0, 55);
  display.println("Sistema ativo");
  
  display.display();
}

void streamCallback(AsyncResult &aResult) {
    if (!aResult.isResult())
        return;

    if (aResult.isEvent()) {
        Serial.printf("Event: %s, Message: %s, Code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
        
        // Quando a autenticação estiver pronta, lê os dados das doações
        if (aResult.eventLog().code() == 10 && aResult.uid() == "authTask" && !dataLoaded) {
            Serial.println("🎉 Firebase autenticado! Lendo dados das doações...");
            dataLoaded = true; // Marca que já carregou para evitar repetições
            delay(1000); // Pequena pausa para garantir que está tudo configurado
            readDonationData();
        }
    }

    if (aResult.available()) {
        RealtimeDatabaseResult &stream = aResult.to<RealtimeDatabaseResult>();
        Serial.printf("Stream Data Path: %s\n", stream.dataPath().c_str());
        Serial.printf("Stream Value: %s\n", stream.to<String>().c_str());
        Serial.printf("Is Stream: %s\n", stream.isStream() ? "true" : "false");
        Serial.printf("Stream Type: %d\n", stream.type());
        Serial.printf("Stream Task ID: %s\n", aResult.uid().c_str());
        
        // Processa mudanças no payment_status
        if (aResult.uid() == "streamTask" && stream.isStream() && (stream.dataPath() == "/" || stream.dataPath() == "/status")) {
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
                JsonDocument doc;
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
                displayInfo(); // Usa nova função em vez de printOLED
                
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

  displayInfo(); // Mostra informações iniciais

  // Start Services
  setupFirebase();
  
  // TEMPORÁRIO: Usar dados fixos para testar o display
  Serial.println("� USANDO DADOS FIXOS PARA TESTE DO DISPLAY");
  totalAmount = "45.50";
  lastDonor = "João Silva";
  topDonor = "Maria Santos";
  
  displayInfo(); // Atualiza display com dados fixos
  Serial.println("✅ Display atualizado com dados fixos!");
  
  startWebServer();
}

void loop() {
  app.loop();
  server.handleClient();
  
  // Verifica mudanças nos dados de doação periodicamente
  if (aClient && dataLoaded) {
    checkForDataUpdates();
  }
  
  delay(10);
}