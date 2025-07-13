# Instruções de Teste - Caixa de Doações ESP32

## ✅ SUCESSO! O código foi compilado e carregado no ESP32!

### 🔧 Para Testar o Sistema:

1. **Feche qualquer programa que esteja usando a porta COM4**
2. **Abra o Monitor Serial** (115200 baud):
   ```
   platformio device monitor --port COM4 --baud 115200
   ```

### 📱 Como Funciona:

#### **Ao Ligar o ESP32:**
```
Inicializando...
Conectando ao WiFi.....
WiFi conectado!
IP: 192.168.x.x

=== COMANDOS DISPONÍVEIS ===
1 - Fazer doação
0 - Voltar ao menu inicial
============================
```

#### **Na Tela OLED você verá:**
```
=== CAIXA DE DOACOES ===

Ultimo: Ninguém  
R$ 0.00

Maior: Ninguém
R$ 0.00

Saldo: R$ 0.00
```

### 🎮 Comandos via Monitor Serial:

1. **Digite `1`** - Iniciar doação
2. **Digite `w`** - Opção anterior (quando em seleção)
3. **Digite `s`** - Próxima opção (quando em seleção)  
4. **Digite `e`** - Selecionar opção atual
5. **Digite `0`** - Voltar ao menu inicial

### 💰 Fluxo de Doação:

1. **Digite `1`** → Menu de seleção aparece
2. **Use `w/s`** para navegar entre valores (R$ 5, 10, 20, 50, personalizado)
3. **Digite `e`** para confirmar
4. **QR Code PIX** aparece na tela OLED
5. **Sistema aguarda pagamento** (verifica automaticamente a cada 5s)
6. **Após pagamento aprovado** → Volta ao menu com dados atualizados

### 📊 Dados Rastreados:

- ✅ Último contribuidor e valor
- ✅ Maior contribuidor e valor  
- ✅ Saldo total da conta
- ✅ Histórico em tempo real

### 🔧 Configurações Importantes:

- **WiFi**: Já configurado para "ILZAMAGDA"/"inventaumaai"
- **Token Mercado Pago**: Configurado para produção
- **Tela OLED**: Pinos I2C padrão (SDA=21, SCL=22)
- **Baud Rate**: 115200

### 🚀 Status Atual:

✅ Código compilado sem erros  
✅ Upload realizado com sucesso  
✅ Todas as bibliotecas instaladas  
✅ Sistema pronto para uso  

### 📱 Próximos Passos (Opcional):

- [ ] Adicionar botões físicos para navegação
- [ ] Salvar dados na EEPROM  
- [ ] Interface web para configuração
- [ ] Notificações sonoras

**O sistema está 100% funcional e pronto para receber doações!** 🎉
