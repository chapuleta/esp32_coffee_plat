#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

extern Preferences preferences;
WebServer server(80);

const char* login_username = "admin";
const char* login_password = "senha123";

String saldoHtml() {
  float saldo = preferences.getFloat("saldo", 0.0);
  return String("<h2>Saldo Total: R$ ") + String(saldo, 2) + "</h2>";
}

void handleRoot() {
  String html = "<form method='POST' action='/login'>"
               "Usuário: <input name='username'><br>"
               "Senha: <input type='password' name='password'><br>"
               "<input type='submit' value='Entrar'>"
               "</form>";
  server.send(200, "text/html", html);
}

void handleLogin() {
  if (server.method() == HTTP_POST) {
    String user = server.arg("username");
    String pass = server.arg("password");
    if (user == login_username && pass == login_password) {
      float saldo = preferences.getFloat("saldo", 0.0);
      String html = saldoHtml();
      html += "<form method='POST' action='/set_saldo'>"
              "Novo saldo: <input name='novo_saldo'><br>"
              "<input type='submit' value='Alterar'>"
              "</form>";
      server.send(200, "text/html", html);
    } else {
      server.send(401, "text/html", "Login inválido");
    }
  } else {
    server.send(405, "text/html", "Método não permitido");
  }
}

void handleSetSaldo() {
  if (server.method() == HTTP_POST) {
    String novoSaldoStr = server.arg("novo_saldo");
    float novoSaldo = novoSaldoStr.toFloat();
    preferences.putFloat("saldo", novoSaldo);
    String html = saldoHtml();
    html += "<p>Saldo alterado com sucesso!</p>";
    server.send(200, "text/html", html);
  } else {
    server.send(405, "text/html", "Método não permitido");
  }
}

void startWebServer() {
  // preferences.begin("doacoes", false); // Já inicializado no main.cpp
  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/set_saldo", handleSetSaldo);
  server.begin();
}

void handleWebServer() {
  server.handleClient();
}
