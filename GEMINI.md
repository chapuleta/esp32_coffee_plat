# ESP32 Coffee Donation System - Instruções para Gemini CLI

## SISTEMA ATUAL FUNCIONANDO (04/08/2025)

### Estado Confirmado:
- **ESP32:** Conecta Wi-Fi, monitora Firebase, processa pagamentos em tempo real
- **Stream Firebase:** Auto-recovery implementado, funciona continuamente
- **Display OLED:** Mostra "Obrigado!" e informações de doação (FASE 1 IMPLEMENTADA)
- **Web Interface:** Formulário PIX funcionando no Vercel
- **Webhook MP:** Notificações chegam e atualizam Firebase corretamente
- **Fluxo completo:** Pagamento → Webhook → Firebase → ESP32 → Display (FUNCIONANDO)

### ✅ FASE 1 IMPLEMENTADA E TESTADA - Display OLED com Informações de Doação:

**STATUS: ✅ FUNCIONANDO - Compilação bem-sucedida (RAM: 14.7%, Flash: 77.6%)**

**Status:** CONCLUÍDA e firmware uploadado com sucesso

**Funcionalidades adicionadas:**
- Display mostra saldo total em destaque (fonte maior)
- Mostra nome do último doador
- Mostra nome do maior doador  
- Mantém call-to-action para formulário web
- Mostra "Sistema ativo" em vez do IP
- Lê dados do Firebase automaticamente no startup
- Mantém funcionalidade "Obrigado!" quando há pagamento

**Melhorias implementadas:**
- **Saldo em destaque:** Fonte tamanho 2 para R$ valor
- **Mais espaço para nomes:** Até 15 caracteres para doadores
- **Layout otimizado:** Removido IP desnecessário
- **API Firebase correta:** Usando `aResult.isResult()` e `aResult.c_str()`
- **ArduinoJson atualizado:** Usando `JsonDocument` em vez de `StaticJsonDocument`

**Layout atual do display:**
```
R$ 0.00          (fonte grande)
Maior: Nenhum ainda
Ultimo: Nenhum ainda  
Doe via formulario web
Sistema ativo
```

### Arquivos Principais Funcionando:
- `src/main.cpp` - Código ESP32 funcional com Fase 1 implementada
- `web/api/create-payment.js` - Criação PIX funcionando
- `web/api/mercadopago-webhook.js` - Webhook funcionando
- `web/public/index.html` - Interface funcionando
- `include/config.h` - Credenciais ESP32 (local, não versionado)

### Contexto do Projeto:
- **Migração bem-sucedida:** ngrok → Firebase (eliminando computador adicional)
- **Segurança implementada:** Credenciais em variáveis ambiente Vercel
- **Performance:** Sistema processa múltiplos pagamentos sem reset manual

## PRÓXIMA IMPLEMENTAÇÃO - FASE 2 OBRIGATÓRIA

### IMPORTANTE: Fase 1 implementada e funcionando
- **Display OLED** agora mostra saldo, doadores e IP
- **Sistema continua** processando pagamentos normalmente
- **NÃO QUEBRAR** o código atual sem autorização expressa
- **IMPLEMENTAR APENAS UMA funcionalidade por vez**

### PARA TESTAR FASE 1:
**✅ FIRMWARE JÁ UPLOADADO** - ESP32 está rodando com Fase 1

Atualizar Firebase manualmente com estes dados para testar:
```json
{
  "donations": {
    "total_amount": 45.50,
    "donation_count": 3,
    "last_donor": "João Silva", 
    "top_donor": "Maria Santos"
  }
}
```
Após atualizar no Firebase, o ESP32 deve mostrar:
```
R$ 45.50         (fonte grande)
Maior: Maria Santos
Ultimo: João Silva
Doe via formulario web
Sistema ativo
```

### Observação sobre o IP:
- **IP removido do display** - Não fazia sentido ocupar espaço limitado
- **Interface web local** ainda funciona (acesso via `http://IP_ESP32/`)
- **Focado no essencial:** Saldo, doadores e call-to-action

### Fase 2 - Entrada Serial Temporária:

#### Objetivo:
Adicionar entrada via Serial Monitor para permitir doações diretas do ESP32:
- Valores pré-definidos: 1=R$3, 2=R$5, 3=R$15, 4=R$50
- **SEM teclado físico ainda**
- **SEM QR Code ainda** 
- **SEM modificar webhook ou formulário web**

#### Implementação Específica:

**1. Adicionar ao `src/main.cpp` (SEM REMOVER código existente):**
```cpp
// Adicionar no loop() após server.handleClient():
if (Serial.available()) {
  char key = Serial.read();
  
  float amount = 0.0;
  switch(key) {
    case '1': amount = 3.00; break;
    case '2': amount = 5.00; break; 
    case '3': amount = 15.00; break;
    case '4': amount = 50.00; break;
    default: return; // Ignora outras teclas
  }
  
  if (amount > 0) {
    Serial.printf("Processando doação anônima de R$ %.2f\n", amount);
    processAnonymousDonation(amount);
  }
}

// Adicionar função para processar doação anônima:
void processAnonymousDonation(float amount) {
  // Atualiza total
  float currentTotal = totalAmount.toFloat();
  float newTotal = currentTotal + amount;
  
  // Atualiza Firebase
  Database.set<float>(*aClient, "/donations/total_amount", newTotal);
  Database.set<String>(*aClient, "/donations/last_donor", "Anônimo");
  
  // Feedback local
  totalAmount = String(newTotal, 2);
  lastDonor = "Anônimo";
  
  // Mostra agradecimento
  printOLED("Obrigado!", 2);
  delay(2000);
  displayInfo();
  
  Serial.printf("Doação processada: R$ %.2f. Novo total: R$ %.2f\n", amount, newTotal);
}
```

**2. Adicionar protótipo da função:**
```cpp
void processAnonymousDonation(float amount);
```

#### REGRAS CRÍTICAS DESTA FASE:

1. **NÃO REMOVER** nenhum código da Fase 1
2. **NÃO MODIFICAR** a função streamCallback() existente
3. **NÃO DUPLICAR** declarações de objetos globais
4. **APENAS ADICIONAR** as novas funções especificadas
5. **TESTAR** via Serial Monitor: digitar 1, 2, 3 ou 4
6. **VERIFICAR** se display atualiza com novos valores

#### Esperado Após Implementação:
- Usuário digita "1" no Serial Monitor → Doação anônima R$ 3,00
- Display atualiza saldo total e último doador = "Anônimo"
- Firebase é atualizado automaticamente
- Sistema continua processando pagamentos web normalmente
- Fase 1 continua funcionando perfeitamente

**IMPLEMENTAR APENAS ESTA FASE. AGUARDAR CONFIRMAÇÃO DE FUNCIONAMENTO ANTES DE PROSSEGUIR.**

## FASES FUTURAS (após Fase 1 confirmada)

### Fase 2 - Entrada Serial Temporária:
- Adicionar input via Serial Monitor para teste
- Valores pré-definidos: 1=R$3, 2=R$5, 3=R$15, 4=R$50
- **SEM teclado físico ainda**
- **SEM QR Code ainda**

### Fase 3 - QR Code Display:
- Integração com Mercado Pago para QR real
- Exibir QR Code na tela do ESP32
- **NÃO usar PIX keys simulados**

### Fase 4 - Interface Web Responsiva:
- Melhorar formulário atual
- Página dedicada para QR Code

### Fase 5 - Teclado Físico:
- Substituir Serial por teclado matricial
- Hardware físico conectado

### Fase 6 - Webhook Update para Doações Anônimas:
- Modificar webhook para processar doações anônimas
- Atualizar contador de doações
- Determinar maior doador automaticamente

## REGRAS CRÍTICAS DE IMPLEMENTAÇÃO

### METODOLOGIA OBRIGATÓRIA:
1. **UMA fase por vez** - Nunca implementar múltiplas funcionalidades
2. **TESTE obrigatório** - Usuário deve confirmar funcionamento
3. **NÃO QUEBRAR** código funcionando atual
4. **NÃO DUPLICAR** objetos globais ou funções

### FIREBASE API CORRETA:
```cpp
// SEMPRE usar esta sintaxe:
Database.get(*aClient, "/path", callback, isStream, "taskID");
Database.set<String>(*aClient, "/path", "value");
Database.set<float>(*aClient, "/path", 25.50);

// NUNCA usar (não existe):
Database.setFloat() // ❌ ERRO
Database.setString() // ❌ ERRO
```

### QR CODE IMPLEMENTATION:
```cpp
// Biblioteca QRCodeGen uso correto:
#include <qrcodegen.hpp>
qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(pixKey.c_str(), qrcodegen::QrCode::Ecc::MEDIUM);

// IMPORTANTE: pixKey deve vir do Mercado Pago API, não simular
```

### KEYPAD SETUP (apenas para Fase 5):
```cpp
// Sempre definir ANTES de usar:
#include <Keypad.h>
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3};
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
```

### ERROS COMUNS A EVITAR:
1. **Código duplicado** - Objetos WiFiClientSecure, display, etc. declarados duas vezes
2. **Função loop() duplicada** - Duas definições causam erro compilação
3. **PIX key fake** - Usar sempre dados reais Mercado Pago
4. **API incorreta** - Usar Database.set<>(), não setFloat()
5. **Keypad sem definição** - Declarar objeto antes de usar getKey()

## DEBUGGING E TROUBLESHOOTING

### Para problemas ESP32:
1. Usar Serial Monitor manual (não por comando)
2. Mensagens de erro em `erro.md`
3. Verificar Wi-Fi primeiro
4. Confirmar Firebase auth
5. Check stream initialization

### Para problemas web:
1. Logs Vercel deployment  
2. Verificar environment variables
3. Testar webhook com simulador MP
4. Validar Firebase rules

### Arquivos de Referência:
- **Exemplos Firebase:** `lib/FirebaseClient-main/examples/`
- **Código exemplo:** `exemplo.md`
- **Logs de erro:** `erro.md` (atualizar sempre com novos erros)

## CREDENCIAIS E SEGURANÇA

### ESP32 Local (`include/config.h`):
```cpp
#define WIFI_SSID "sua_wifi"
#define WIFI_PASSWORD "sua_senha"  
#define FIREBASE_HOST "https://projeto.firebaseio.com/"
#define FIREBASE_API_KEY "AIza..."
#define FIREBASE_USER_EMAIL "email@gmail.com"
#define FIREBASE_USER_PASSWORD "senha"
```

### Vercel Environment Variables:
- MERCADOPAGO_ACCESS_TOKEN
- FIREBASE_PROJECT_ID
- FIREBASE_PRIVATE_KEY
- FIREBASE_CLIENT_EMAIL
- FIREBASE_DATABASE_URL

### Lembre-se:
- `config.h` é local, não versionado
- Nunca commit credenciais no Git
- Use MCP Mercado Pago para consultar docs oficiais

---

**ESTADO ATUAL:** ✅ Fase 1 implementada e funcionando. Sistema processa pagamentos web + mostra informações no display.

**PRÓXIMO PASSO:** Implementar Fase 2 - Entrada Serial. Testar antes de prosseguir para Fase 3.

**COMUNICAÇÃO ESPERADA:** 
- Gemini: "Implementei Fase 2 - Entrada Serial. Por favor, teste digitando 1,2,3,4 no Serial Monitor antes de prosseguir para Fase 3."
- Usuário: "Funcionando" ou "Tem erro, ajustar"

---

## STATUS DAS IMPLEMENTAÇÕES

### ✅ CONCLUÍDO:
- **Fase 1:** Display OLED com saldo, doadores e IP - FUNCIONANDO

### 🔄 PRÓXIMO:
- **Fase 2:** Entrada Serial temporária (1=R$3, 2=R$5, 3=R$15, 4=R$50)

### 📋 PENDENTE:
- **Fase 3:** QR Code Display
- **Fase 4:** Interface Web Responsiva
- **Fase 5:** Teclado Físico
- **Fase 6:** Webhook Update para Doações Anônimas