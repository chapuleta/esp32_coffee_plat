# 🎯 SISTEMA DE DOAÇÃO COM FORMULÁRIO - GUIA DE TESTE

## 📋 Resumo das Mudanças

O sistema agora coleta **dados do doador no formulário** antes de gerar o PIX, garantindo que o nome apareça no ESP32 mesmo para pagamentos PIX anônimos.

### ✅ O que foi implementado:

1. **Frontend atualizado** (`index.html` + `script.js`):
   - Formulário com nome, email, telefone e valor
   - Botões de valores pré-definidos + campo customizado
   - Validação completa dos dados
   - Interface moderna e responsiva

2. **Backend atualizado** (`create-payment.js`):
   - Recebe dados do doador do formulário
   - Inclui informações no `external_reference` do pagamento
   - Validações de dados e valores

3. **Webhook melhorado** (`mercadopago-webhook.js`):
   - **PRIORIDADE**: Extrai dados do `external_reference` primeiro
   - Fallback: Usa dados do payer object se disponível
   - Logs detalhados para debugging

## 🚀 Como Testar

### 1. Deploy na Vercel
```bash
cd web
vercel --prod
```

### 2. Teste do Formulário
1. Acesse: `https://seu-projeto.vercel.app`
2. Preencha o formulário:
   - **Nome completo**: "João Silva"
   - **Email** (opcional): "joao@email.com"
   - **Telefone** (opcional): "(11) 99999-9999"
   - **Valor**: R$ 5,00 ou customizado

3. Clique em "Gerar PIX Personalizado"

### 3. Verificar QR Code
- QR Code deve ser gerado com sucesso
- Deve mostrar: "Doador: João Silva"
- Deve incluir PIX Copia e Cola

### 4. Simular Pagamento PIX
Use o simulador do Mercado Pago ou faça um pagamento real de teste.

### 5. Verificar Webhook
Nos logs da Vercel, deve aparecer:
```
🎯 Dados do doador extraídos do formulário com sucesso!
✅ Nome extraído do formulário: João Silva
✅ Email extraído do formulário: joao@email.com
```

### 6. Verificar ESP32
O display deve mostrar:
```
Total: R$ X.XX
Ultimo: João Silva
Maior: João Silva R$ 5.00
```

## 🔍 Debug e Logs

### Logs do Frontend (Console do Browser)
```javascript
📋 Dados recebidos para criação do PIX: {name: "João Silva", email: "joao@email.com", ...}
💳 Criando pagamento PIX: {...}
✅ Pagamento criado com sucesso: {...}
```

### Logs do Backend (Vercel)
```javascript
📋 Dados recebidos para criação do PIX: {name: "João Silva", ...}
💳 Criando pagamento PIX: {external_reference: "{\"name\":\"João Silva\",...}"}
✅ Pagamento criado com sucesso: {id: "123456", ...}
```

### Logs do Webhook (Vercel)
```javascript
🔔 Webhook do Mercado Pago recebido!
📋 External reference encontrado: {"name":"João Silva",...}
🎯 Dados do doador extraídos do formulário com sucesso!
✅ Nome extraído do formulário: João Silva
```

## 📱 Interface do Usuário

### Formulário Responsivo
- Campos organizados e validados
- Botões de valores rápidos (R$ 2, R$ 5, R$ 10, R$ 20)
- Campo customizado para outros valores
- Validação em tempo real

### Experiência de Pagamento
1. Usuário preenche dados
2. Seleciona valor ou digita personalizado
3. Clica em "Gerar PIX Personalizado"
4. Recebe QR Code + PIX Copia e Cola
5. Após pagamento, nome aparece automaticamente no ESP32

## 🛠️ Troubleshooting

### Problema: Nome ainda aparece "Doador Anônimo"
**Verificar**:
1. `external_reference` está sendo criado corretamente?
2. Webhook está extraindo dados do `external_reference`?
3. Logs mostram "Dados do doador extraídos do formulário"?

### Problema: QR Code não é gerado
**Verificar**:
1. Access Token do Mercado Pago configurado?
2. Valor mínimo respeitado (R$ 0,50)?
3. Nome do doador preenchido?

### Problema: Webhook não processa
**Verificar**:
1. URL do webhook configurada na Vercel?
2. Firebase credentials configuradas?
3. Logs da Vercel para erros?

## 🎉 Resultado Esperado

**ANTES**: PIX → "Doador Anônimo"
**DEPOIS**: Formulário → PIX → "João Silva" no ESP32

O sistema agora garante que **sempre** teremos o nome do doador, mesmo em pagamentos PIX anônimos, porque coletamos a informação **antes** de gerar o pagamento!

## 📞 Próximos Passos

1. ✅ Testar fluxo completo
2. ✅ Verificar logs em produção
3. 🎯 Confirmar nome no ESP32
4. 📊 Monitorar histórico de doações
5. 🎨 Personalizar visual se necessário
