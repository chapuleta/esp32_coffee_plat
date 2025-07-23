#include <Arduino.h>
#include <WiFi.h>
#include "config.h" // Suas configurações de WiFi e Firebase

// Bibliotecas do Firebase
#include <Firebase_ESP_Client.h>

// Bibliotecas do Display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <qrcode.h>

// --- Configurações do Display OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Objetos do Firebase ---
FirebaseData stream;
FirebaseAuth auth;
FirebaseConfig config;

// --- Protótipos de Funções ---
void updateDisplayWithMessage(const String& message);
void streamCallback(FirebaseStream data);

void setup() {
  Serial.begin(115200);

  // Inicializa o display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao iniciar SSD1306"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  updateDisplayWithMessage("Iniciando...");
  display.display();

  // Conecta ao Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Conectado!");
  updateDisplayWithMessage("WiFi Conectado!\nAguardando doacao...");
  
  // Configura o Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Configura o "listener" para o nosso quadro de avisos
  // A função 'streamCallback' será chamada sempre que algo mudar em "/payment_status"
  if (!Firebase.RTDB.beginStream(&stream, "/payment_status")) {
    Serial.println("------------------------------------");
    Serial.println("Erro ao configurar o stream do Firebase");
    Serial.println("Razao: " + stream.errorReason());
    Serial.println("------------------------------------");
  } else {
    Serial.println("Stream do Firebase configurado com sucesso.");
  }
}

// Função que é chamada AUTOMATICAMENTE quando há uma mudança no Firebase
void streamCallback(FirebaseStream data) {
  Serial.printf("Aviso recebido do Firebase! Caminho: %s, Evento: %s\n", data.streamPath().c_str(), data.eventType().c_str());
  
  // Verificamos apenas se o dado no caminho /status foi alterado
  if (data.dataPath() == "/status" && data.dataType() == "string") {
    String status = data.stringData();
    Serial.println("Status recebido: " + status);

    if (status == "approved") {
      Serial.println("PAGAMENTO APROVADO! ATUALIZANDO DISPLAY...");
      updateDisplayWithMessage("Obrigado pela\nsua doacao!");

      // Importante: Limpa o aviso para não processar de novo
      // Isso evita que o ESP32 agradeça de novo se for reiniciado
      Firebase.RTDB.setString(&stream, "/payment_status/status", "processed");
    }
  }
}

void loop() {
  // O loop principal agora só precisa garantir que o Firebase está sendo lido.
  // A biblioteca cuida de chamar o 'streamCallback' automaticamente.
  // NOTA: A biblioteca Firebase-ESP-Client mais recente gerencia isso em background.
  // Esta verificação é mais por garantia e para versões mais antigas.
  if (Firebase.isTokenExpired()) {
      Firebase.refreshToken(&config);
  }
  // Pequeno delay para não sobrecarregar
  delay(100); 
}

// Função auxiliar para atualizar o display
void updateDisplayWithMessage(const String& message) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}