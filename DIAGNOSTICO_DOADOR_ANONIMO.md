# 🔍 **DIAGNÓSTICO: Por que "Doador Anônimo"?**

## ❌ **Problema Identificado:**
O Mercado Pago **NÃO ENVIA dados do pagador por padrão** nos webhooks! Eles só enviam:
- ✅ ID do pagamento
- ✅ Status do pagamento  
- ❌ **Dados do pagador (nome, email, etc.)**

## 🔧 **Solução Implementada:**

### **Nova URL com Correção:**
```
https://webhook-coffee-bd9qyhcoc-joao-victor-ferreira-abreus-projects.vercel.app/api/mercadopago-webhook
```

### **Melhorias no Webhook:**

1. **Consulta Direta à API:** Faz requisição extra para buscar dados completos
2. **Múltiplas Fontes:** Verifica `payer` e `additional_info.payer`
3. **Logs Detalhados:** Mostra exatamente o que está sendo recebido
4. **Parâmetro `expand=payer`:** Força o MP a incluir dados do pagador

## 🧪 **Como Testar:**

### **1. Atualizar URL no Mercado Pago:**
```
https://webhook-coffee-bd9qyhcoc-joao-victor-ferreira-abreus-projects.vercel.app/api/mercadopago-webhook
```

### **2. Fazer Pagamento PIX Real:**
- Valor: R$ 0,01
- **IMPORTANTE:** Use dados reais (nome, CPF, email)
- PIX anônimo = continua anônimo!

### **3. Verificar Logs:**
```bash
vercel logs https://webhook-coffee-bd9qyhcoc-joao-victor-ferreira-abreus-projects.vercel.app
```

## 📋 **Limitações do Mercado Pago:**

### **PIX Anônimo:**
- Se o usuário pagar sem informar dados = "Doador Anônimo"
- **Solução:** Instruir doadores a preencherem dados

### **Dados Disponíveis:**
- ✅ Nome (se informado)
- ✅ Email (se informado) 
- ✅ CPF (se informado)
- ❌ Telefone (raramente disponível em PIX)

## 🎯 **Resultado Esperado:**

Agora o webhook vai:
1. ✅ Receber notificação do pagamento
2. ✅ Fazer consulta adicional para buscar dados do pagador  
3. ✅ Extrair nome real (se disponível)
4. ✅ Atualizar Firebase com dados corretos
5. ✅ ESP32 mostra nome real no display

---

**🔍 IMPORTANTE:** Se ainda aparecer "Anônimo", significa que o pagador não informou os dados ou fez PIX anônimo!
