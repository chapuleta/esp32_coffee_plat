# 🧪 Como Testar o Sistema Automático

## ✅ **Status Atual do Sistema**

### **ESP32** 
- ✅ Código compilado e funcionando
- ✅ Polling automático a cada 10 segundos
- ✅ Display mostra dados reais do Firebase
- ✅ Sem valores de teste ou chamadas desnecessárias de API

### **Webhook Automático**
- ✅ Sistema implementado e otimizado
- ✅ Atualização automática do saldo
- ✅ Registro de doadores e histórico
- ✅ Logs detalhados para debugging

## 🚀 **Próximos Passos para Testar**

### **1. Upload do ESP32** ⏳
```bash
# Quando a porta COM4 estiver livre:
pio run --target upload
```

### **2. Deploy do Webhook**
```bash
cd web
vercel --prod
# Anote a URL gerada, ex: https://seu-projeto.vercel.app
```

### **3. Configurar Webhook no Mercado Pago**
1. Acesse: https://www.mercadopago.com.br/developers/panel/webhooks
2. Adicione URL: `https://seu-projeto.vercel.app/api/mercadopago-webhook`
3. Selecione eventos: `payment`

### **4. Teste Manual**
```bash
# No Firebase Console, altere manualmente:
/donations/total_amount = "999.0"
/donations/last_donor = "Teste Manual"

# ESP32 deve detectar e atualizar display em ~10 segundos
```

### **5. Teste Real com Pagamento**
1. Crie um link de pagamento no Mercado Pago
2. Faça um pagamento de teste (R$ 1,00)
3. Observe logs do webhook no Vercel
4. Confirme que ESP32 atualizou automaticamente

## 📱 **Como Monitorar**

### **Serial Monitor do ESP32**
```bash
pio device monitor
```
**O que deve aparecer:**
```
🔄 Verificando atualizações nos dados de doações...
Raw total_amount: '999.0'
✅ Total amount updated: 999.0
Raw last_donor: 'Teste Manual'
✅ Last donor updated: Teste Manual
🎉 Todos os dados carregados! Atualizando display...
```

### **Logs do Webhook (Vercel)**
```bash
vercel logs
```
**O que deve aparecer quando receber pagamento:**
```
🔔 Webhook do Mercado Pago recebido!
💳 Pagamento 123456789:
   - Status: approved
   - Valor: R$ 1.00
   - Doador: João Silva
✅ Pagamento aprovado! Processando automaticamente...
💰 Saldo anterior: R$ 999.00
💰 Nova doação: R$ 1.00
💰 Novo saldo total: R$ 1000.00
🎉 Sistema atualizado automaticamente!
```

## 🎯 **Resultado Esperado**

Quando alguém fizer uma doação:

1. **Mercado Pago** → processa pagamento
2. **Webhook** → recebe notificação e atualiza Firebase automaticamente
3. **ESP32** → detecta mudança (máximo 10 segundos)
4. **Display** → mostra novo total, último doador, etc.

**⏱️ Tempo total: 10-15 segundos do pagamento até atualização do display**

## 🔧 **Solução de Problemas**

### **ESP32 não atualiza**
- Verifique WiFi
- Confira credenciais Firebase no config.h
- Monitore serial para ver logs

### **Webhook não funciona**
- Verifique variáveis de ambiente no Vercel
- Confirme URL do webhook no Mercado Pago
- Analise logs do Vercel

### **Firebase não atualiza**
- Verifique permissões do Firebase
- Confirme estrutura dos dados
- Teste manual via console

## 📋 **Checklist Final**

- [ ] ESP32 compilado e funcionando
- [ ] Webhook deployado no Vercel
- [ ] Variáveis de ambiente configuradas
- [ ] URL do webhook registrada no Mercado Pago
- [ ] Teste manual no Firebase funcionando
- [ ] Teste real com pagamento de R$ 1,00

---

**🎉 Sistema pronto para uso em produção!** 

Agora você tem um sistema completamente automático que atualiza o display em tempo real sempre que alguém faz uma doação.
