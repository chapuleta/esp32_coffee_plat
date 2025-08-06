# ESP32 Coffee Donation System - Instruções para Gemini CLI

## SISTEMA ATUAL FUNCIONANDO (05/08/2025) - SALDO TEMPO REAL IMPLEMENTADO

### ✅ PROBLEMA CRÍTICO RESOLVIDO - Frontend JavaScript

**PROBLEMA IDENTIFICADO:** Sistema completo funcionando (ESP32 + Firebase + Webhook), mas frontend web não mostrava dados em tempo real.

**SINTOMAS:**
- ❌ Saldo permanecia em "R$ 0,00" 
- ❌ Último doador não aparecia
- ❌ Maior doação não carregava
- ❌ Histórico de doações ficava "Carregando histórico"
- ✅ Backend funcionava (APIs retornavam dados corretos)
- ✅ Firebase atualizava normalmente
- ✅ ESP32 mostrava dados atualizados

**CAUSA RAIZ:** JavaScript principal (`script.js`) estava muito complexo com:
- Lógica de retry aninhada excessivamente 
- Event listeners múltiplos conflitantes
- Sistema de burst mode sofisticado demais
- Possível erro de sintaxe que quebrava toda execução
- Funções assíncronas com abort controllers complexos

**SOLUÇÃO IMPLEMENTADA:**
1. **Criado `script-simple.js`** - Versão limpa e funcional
2. **Removido código complexo** - Apenas funções essenciais
3. **Polling direto** - A cada 10s (saldo) e 30s (histórico)
4. **Logs de debug** - Para identificar problemas futuros

### Arquivos Criados/Modificados:

#### `web/public/script-simple.js` ⭐ **NOVO**
```javascript
// Versão simplificada que funciona
window.addEventListener('DOMContentLoaded', function() {
    loadBalance();
    loadHistory();
    setInterval(loadBalance, 10000);   // 10s
    setInterval(loadHistory, 30000);   // 30s
});

async function loadBalance() {
    const response = await fetch('/api/get-current-balance');
    const data = await response.json();
    
    // Atualiza elementos do DOM diretamente
    document.getElementById('current-balance').textContent = 
        `R$ ${data.total_amount.toFixed(2).replace('.', ',')}`;
    // ... resto da implementação limpa
}
```

#### `web/public/index.html` 📝 **MODIFICADO**
```html
<!-- Alterado de script.js para script-simple.js -->
<script src="script-simple.js"></script>
```

### Estado Confirmado Após Correção:
- **ESP32:** ✅ Conecta Wi-Fi, monitora Firebase, processa pagamentos em tempo real
- **Stream Firebase:** ✅ Auto-recovery implementado, funciona continuamente  
- **Display OLED:** ✅ Mostra saldo, último doador e maior doação atualizados
- **Web Interface:** ✅ Formulário PIX funcionando no Vercel
- **Webhook MP:** ✅ Notificações chegam e atualizam Firebase corretamente
- **Frontend Web:** ✅ **AGORA FUNCIONA** - Saldo e histórico em tempo real
- **APIs Funcionais:** ✅ `/api/get-current-balance` e `/api/get-donation-history`
- **Fluxo completo:** ✅ Pagamento → Webhook → Firebase → ESP32 → Display → Frontend

### Dados de Teste Funcionando:
```json
{
  "total_amount": 31.40,
  "last_donor": "João Silva (Teste)",
  "top_donor": "João Silva (Teste)", 
  "top_amount": 25.50
}
```

### Logs de Sucesso (Console Browser):
```javascript
🚀 SCRIPT SIMPLES CARREGADO!
🚀 DOM CARREGADO!
💰 Buscando saldo...
📡 Status: 200
📋 Dados: {total_amount: 31.4, last_donor: "João Silva (Teste)", ...}
✅ Saldo atualizado: R$ 31,40
✅ Último doador: João Silva (Teste)
✅ Maior doação: João Silva (Teste) - R$ 25,50
📜 Buscando histórico...
✅ Histórico atualizado: 1 doações
```

### Webhook Mercado Pago Também Corrigido:

**PROBLEMA:** Webhook não extraía corretamente `payment_id` do corpo da requisição.

**CORREÇÃO:**
```javascript
// Antes (não funcionava):
const paymentId = req.body.data?.id;

// Depois (funciona):
let paymentId = req.body.data?.id || req.body.resource || req.query.id;
console.log('Body completo:', JSON.stringify(req.body, null, 2));
```

### ✅ SISTEMA COMPLETAMENTE FUNCIONAL

**Frontend Web Responsivo:**
- 💰 Saldo atual centralizado e em destaque  
- 👤 Último doador exibido
- 🏆 Maior doação e doador
- 📜 Histórico completo de doações (quem, quando, quanto)
- 🔄 Atualização automática em tempo real (10s saldo, 30s histórico)
- 📱 Design responsivo (grid 2 colunas desktop, empilhado mobile)

**Eliminado da Versão Complexa:**
- ❌ Sistema burst mode após pagamentos (desnecessário)
- ❌ Detecção de visibilidade da página (complexo demais)
- ❌ Retry automático com backoff (over-engineering)
- ❌ AbortController para timeout (causava conflitos)
- ❌ Lógica de consecutiveErrors (não agregava valor)

**Mantido na Versão Simples:**
- ✅ Polling regular e confiável
- ✅ Logs claros para debug
- ✅ Tratamento básico de erros
- ✅ Formatação de valores brasileira
- ✅ Timestamps relativos no histórico

### Lição Aprendida:
- **Simplicidade > Complexidade** - Script simples é mais confiável
- **Debug First** - Logs ajudam a identificar problemas rapidamente  
- **Progressive Enhancement** - Implementar funcionalidades básicas primeiro
- **Real Data Testing** - Testar com dados reais do Firebase

---
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

## PRÓXIMA IMPLEMENTAÇÃO - SALDO REAL DO MERCADO PAGO

### ✅ NOVA FUNCIONALIDADE IMPLEMENTADA - Consulta Saldo Real MP:

**PROBLEMA IDENTIFICADO:** O sistema estava somando doações manualmente em vez de usar o saldo real da conta Mercado Pago.

**SOLUÇÃO IMPLEMENTADA:**
- Nova função `fetchMercadoPagoBalance()` que consulta o saldo real via API
- Integração com `balance_summary` endpoint do Mercado Pago
- Atualização automática a cada 10 segundos junto com verificação de dados
- Sincronização bidirecional: MP → Firebase → ESP32 → Display

### Implementação Técnica:

**1. API Endpoint Utilizada:**
```
GET https://api.mercadopago.com/v1/account/balance_summary
Authorization: Bearer ACCESS_TOKEN
```

**2. Nova Função Adicionada:**
```cpp
void fetchMercadoPagoBalance() {
  // Conecta com API do Mercado Pago
  // Consulta saldo disponível na conta
  // Compara com valor atual
  // Atualiza Firebase e display se houver mudança
}
```

**3. Configuração Necessária:**
Adicionar no `include/config.h`:
```cpp
#define MERCADOPAGO_ACCESS_TOKEN "SEU_ACCESS_TOKEN_AQUI"
```

**4. Fluxo de Funcionamento:**
1. A cada 10 segundos o ESP32 consulta a API do MP
2. Compara saldo atual com valor em `totalAmount`
3. Se diferente, atualiza Firebase com saldo real
4. Display é atualizado automaticamente
5. Serial monitor mostra logs da operação

### Como Obter o Access Token:

**1. Acesse:** https://www.mercadopago.com.br/developers/
**2. Vá em:** Suas aplicações → Sua aplicação → Credenciais
**3. Copie:** Access Token de Produção (ou Sandbox para testes)
**4. Substitua** `SEU_ACCESS_TOKEN_AQUI` no config.h

### Vantagens da Implementação:

✅ **Saldo Real:** Mostra exatamente o que tem na conta MP
✅ **Automático:** Atualiza sozinho sem intervenção manual  
✅ **Confiável:** Dados direto da fonte oficial
✅ **Compatível:** Mantém webhook para "Obrigado!" imediato
✅ **Sincronizado:** Firebase sempre atualizado com saldo real

### Status Atual:
- **Código:** ✅ Implementado e compilado com sucesso
- **Configuração:** ⚠️ Necessário adicionar Access Token real
- **Teste:** 📋 Pendente upload e verificação

### Logs Esperados:
```
💳 Consultando saldo real do Mercado Pago...
💰 Saldo MP atualizado: R$ 0.00 -> R$ 127.50
🔄 Verificando atualizações nos dados de doações...
✅ Saldo MP inalterado: R$ 127.50
```

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

**ESTADO ATUAL:** ✅ Sistema completamente funcional - ESP32 + Firebase + Webhook + Frontend Web em tempo real funcionando perfeitamente.

**CORREÇÃO CRÍTICA:** Problema era JavaScript complexo demais. Solução: script simplificado e funcional.

**TODOS OS COMPONENTES:** ✅ ESP32 display, ✅ Webhook MP, ✅ Firebase sync, ✅ Frontend responsivo tempo real.

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