# 🤖 Sistema Automático de Doações

## 📋 **Como Funciona**

O sistema agora atualiza **automaticamente** o saldo e informações no display do ESP32 sempre que alguém faz uma doação via Mercado Pago.

## 🔄 **Fluxo Automático**

```
💰 Cliente faz doação via Mercado Pago
     ↓
🔔 Mercado Pago envia webhook para /api/mercadopago-webhook
     ↓
⚡ Webhook processa automaticamente:
   • Verifica se pagamento foi aprovado
   • Soma valor ao total atual no Firebase
   • Atualiza último doador
   • Verifica se é novo maior doador
   • Registra no histórico
     ↓
📱 ESP32 detecta mudança no Firebase (polling a cada 10s)
     ↓
🖥️ Display atualiza automaticamente com novos dados
```

## ✅ **O Que o Sistema Faz Automaticamente**

### **Quando recebe pagamento aprovado:**
1. ✅ **Soma automaticamente** o valor ao total existente
2. ✅ **Atualiza último doador** com nome do pagador
3. ✅ **Verifica maior doador** e atualiza se necessário
4. ✅ **Registra histórico** completo da doação
5. ✅ **Atualiza display** do ESP32 em tempo real

### **Informações registradas:**
- 💰 **Total acumulado** (soma automática)
- 👤 **Último doador** (nome do pagador)
- 🏆 **Maior doador** (verifica se superou recorde)
- 📋 **Histórico completo** (todas as doações)
- ⚡ **Status em tempo real** (approved/pending/rejected)

## 🛠️ **Configuração**

### **1. Variáveis de Ambiente (.env)**
```env
MERCADOPAGO_ACCESS_TOKEN=APP_USR-seu-token-aqui
FIREBASE_PROJECT_ID=seu-projeto-id
FIREBASE_PRIVATE_KEY="-----BEGIN PRIVATE KEY-----\nsua-chave-aqui\n-----END PRIVATE KEY-----\n"
FIREBASE_CLIENT_EMAIL=firebase-adminsdk@seu-projeto.iam.gserviceaccount.com
FIREBASE_DATABASE_URL=https://seu-projeto-default-rtdb.firebaseio.com/
```

### **2. Deploy no Vercel**
```bash
cd web
vercel --prod
```

### **3. Configurar Webhook no Mercado Pago**
- URL: `https://seu-dominio.vercel.app/api/mercadopago-webhook`
- Eventos: `payment`

## 📊 **Estrutura do Firebase**

```json
{
  "donations": {
    "total_amount": "558.0",
    "last_donor": "João Silva",
    "top_donor": "Maria Santos",
    "top_amount": "100.00",
    "history": {
      "push_id_1": {
        "payment_id": "123456789",
        "amount": "50.00",
        "donor": "João Silva",
        "method": "pix",
        "timestamp": 1635724800000,
        "status": "approved"
      }
    }
  },
  "payment_status": {
    "status": "approved"
  }
}
```

## 🎯 **Vantagens do Sistema Automático**

- ✅ **Zero trabalho manual** - tudo automático
- ✅ **Tempo real** - display atualiza em ~10 segundos
- ✅ **Sempre correto** - não esquece de atualizar
- ✅ **Histórico completo** - registra todas as doações
- ✅ **Debugging fácil** - logs detalhados de tudo
- ✅ **Prova de erro** - registra erros para análise

## 🔍 **Monitoramento**

### **Logs do Webhook**
Todos os pagamentos são logados com detalhes:
```
🔔 Webhook do Mercado Pago recebido!
💳 Pagamento 123456789:
   - Status: approved
   - Valor: R$ 50.00
   - Doador: João Silva
   - Método: pix
✅ Pagamento aprovado! Processando automaticamente...
🧮 Atualizando saldo total com nova doação de R$ 50.00
💰 Saldo anterior: R$ 508.00
💰 Nova doação: R$ 50.00
💰 Novo saldo total: R$ 558.00
🎉 Sistema atualizado automaticamente!
```

### **Histórico de Erros**
Se algo der errado, fica registrado em `/webhook_errors` no Firebase.

## 🚀 **Status Atual**

✅ **ESP32**: Configurado para polling automático  
✅ **Firebase**: Estrutura de dados otimizada  
✅ **Webhook**: Sistema automático implementado  
🔄 **Próximo**: Deploy e teste com pagamento real

---

**🎉 Agora o sistema é 100% automático! Você só precisa compartilhar o link de pagamento e o resto acontece sozinho.**
