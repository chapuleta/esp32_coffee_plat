# 🔍 **WEBHOOK MELHORADO - DADOS DO PAGADOR**

## ✅ **Nova URL do Webhook:**
```
https://webhook-coffee-ihtnsu1dj-joao-victor-ferreira-abreus-projects.vercel.app/api/mercadopago-webhook
```

## 🆕 **Melhorias Implementadas:**

### **📋 Dados Completos do Pagador:**
O webhook agora extrai e registra:

- ✅ **Nome completo** (primeiro nome + sobrenome)
- ✅ **Email** do pagador
- ✅ **Telefone** do pagador  
- ✅ **CPF** (mascarado para privacidade: 123.***.**45)
- ✅ **Tipo de PIX** (QR Code ou Copia e Cola)
- ✅ **Timestamp** completo da transação
- ✅ **Log detalhado** de todos os dados recebidos

### **🔍 Debugging Avançado:**
Agora o webhook fará log completo de:
```javascript
📋 Dados completos do pagamento: {
  // JSON completo da API do Mercado Pago
}

💳 Pagamento 123456789:
   - Status: approved
   - Valor: R$ 0.03
   - Método: pix (QR Code PIX)
   - Doador: João Victor Silva
   - Email: joao@email.com
   - Telefone: (11) 99999-9999
   - Documento: 123.***.**45
```

### **💾 Histórico Detalhado no Firebase:**
```json
{
  "donations/history/push_id": {
    "payment_id": "123456789",
    "amount": "0.03",
    "donor_name": "João Victor Silva",
    "donor_email": "joao@email.com", 
    "donor_phone": "(11) 99999-9999",
    "donor_document": "123.***.**45",
    "payment_method": "pix",
    "pix_info": "QR Code PIX",
    "timestamp": 1635724800000,
    "status": "approved",
    "processed_at": "2025-08-05T08:15:00.000Z"
  }
}
```

## 🧪 **Como Testar:**

1. **Atualizar URL no Mercado Pago** com a nova URL
2. **Fazer novo pagamento PIX** de R$ 0,01
3. **Verificar logs** em tempo real:
   ```bash
   vercel logs https://webhook-coffee-ihtnsu1dj-joao-victor-ferreira-abreus-projects.vercel.app
   ```
4. **Confirmar dados no Firebase** (histórico completo)

## 🎯 **Agora você terá:**
- ✅ **Nome real** do doador no display
- ✅ **Histórico completo** de quem doou
- ✅ **Informações de contato** para agradecimentos
- ✅ **Debugging detalhado** para resolver problemas
- ✅ **Dados mascarados** para privacidade

---

**🚀 Sistema pronto para capturar todos os dados do pagador PIX!**
