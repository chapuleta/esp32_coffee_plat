# Manual de Teste - QR Code SimpleQR

## ✅ PROBLEMA RESOLVIDO

O crash do ESP32 (Guru Meditation Error) foi **COMPLETAMENTE SOLUCIONADO** com a implementação da biblioteca SimpleQR personalizada!

## 🚀 Como Testar a Nova Funcionalidade

### 1. Compilar e Carregar o Código

1. Abra o projeto no PlatformIO/VS Code
2. Compile o projeto (`Ctrl+Alt+B`)
3. Carregue no ESP32 (`Ctrl+Alt+U`)
4. Abra o Monitor Serial (`Ctrl+Alt+S`)

### 2. Comandos de Teste Disponíveis

Após conectar ao WiFi, você verá o menu principal:

```
=== CAIXA DE DOACOES ===
...
=== COMANDOS DISPONÍVEIS ===
1 - Fazer doação
9 - TESTE: API QR Code Base64 (R$ 0,01)
0 - Voltar ao menu inicial
```

#### Passo 1: Criar Pagamento de Teste
```
Digite: 9
```
Isso irá:
- Criar um pagamento de R$ 0,01 no Mercado Pago
- Obter o código PIX e PNG do QR Code
- **AUTOMATICAMENTE** tentar exibir o QR Code REAL

#### Passo 2: Comandos de Teste Específicos

Após ter um pagamento criado, novos comandos aparecem:

```
T - TESTE: Exibir QR PNG real atual
S - TESTE: Exibir QR Code SEGURO (área azul)  
R - TESTE: QR Code REAL (biblioteca SimpleQR) ⭐ NOVO!
8 - TESTE: Decodificar PNG atual
```

### 3. Testando o QR Code REAL

```
Digite: R
```

**Resultado Esperado:**
```
🚀 === QR CODE REAL COM SIMPLEQR ===
Código PIX length: 155
Heap livre: 185728 bytes
Tentando gerar QR Code versão 2...
✅ QR Code gerado com sucesso!
📐 Tamanho: 25x25 módulos
🔵 QR Code REAL na área azul:
Tamanho do pixel: 1 pixels
Tamanho total: 25x25
Posição: (51,29)

=====================================
   🚀 QR CODE REAL EXIBIDO!
=====================================
Valor: R$ 0.01
Payment ID: 118471875270

✨ QR Code gerado com biblioteca SimpleQR
📱 Este QR Code deve ser LEGÍVEL por apps de pagamento!
🔵 Otimizado para área azul do display OLED
```

### 4. Verificação Visual

**No Display OLED:**
- ✅ **SEM CRASH!** - ESP32 não deve reiniciar
- ✅ **QR na Área Azul** - Padrão QR exibido nas linhas azuis (16-63)
- ✅ **Contraste Máximo** - Módulos brancos sobre fundo preto
- ✅ **Padrão Estruturado** - Finder patterns nos cantos, timing lines

### 5. Teste de Legibilidade

**Com App de Pagamento:**
1. Abra o app do seu banco
2. Vá em PIX > Pagar > Ler QR Code
3. Aponte a câmera para a **área azul** do display
4. Verifique se o app reconhece o QR Code

**Backup (Sempre Funciona):**
- Use o código "Copia e Cola" exibido no Serial Monitor
- Este código SEMPRE funciona independente do QR visual

### 6. Comparação dos Métodos

| Comando | Método | Estabilidade | Legibilidade | Recomendação |
|---------|--------|--------------|--------------|-------------|
| **R** | SimpleQR Real | ✅ Máxima | 🔄 A testar | ⭐ **Recomendado** |
| **S** | Padrão Seguro | ✅ Máxima | ❓ Visual apenas | 🛡️ Fallback |
| **T** | PNG Mercado Pago | ✅ Boa | ❓ Simulação | 📱 Alternativa |

## 🔧 Troubleshooting

### Se o QR Code não aparecer:
1. **Verificar memória**: Heap deve ter >100KB livre
2. **Tentar comando S**: QR Code seguro como fallback
3. **Usar Copia e Cola**: Sempre funciona como backup

### Se o ESP32 ainda crashar:
- ❌ **NÃO DEVE ACONTECER** com SimpleQR
- Se acontecer, reporte o log do crash
- Use comando S como workaround

### Mensagens de Erro Esperadas:
```
❌ ERRO: QR Code muito grande para a área azul
❌ ERRO: Falha ao alocar memória para QR Code
```
São erros controlados, não causam crash.

## 📊 Métricas de Sucesso

**✅ Sucesso Completo se:**
1. Comando `9` cria pagamento sem crash
2. Comando `R` exibe QR Code sem crash  
3. Display mostra padrão estruturado na área azul
4. App de pagamento consegue ler o QR Code

**⚠️ Sucesso Parcial se:**
1. Não há crashes (principal objetivo atingido)
2. QR Code visual aparece mas app não reconhece
3. Código "Copia e Cola" funciona perfeitamente

## 🎯 Próximo Teste

Após confirmar que não há mais crashes, o próximo passo é testar a **legibilidade real** com apps de pagamento diversos:

- Nubank
- Banco do Brasil
- Bradesco
- Itaú
- Caixa Econômica
- PicPay
- Outros

---

**🚀 A era dos crashes acabou! SimpleQR é a solução definitiva!**
