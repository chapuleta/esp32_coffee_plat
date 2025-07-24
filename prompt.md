Objetivo: Gerar o código C++ (main.cpp) para um dispositivo ESP32 usando o framework Arduino e PlatformIO.

Contexto do Projeto:
O projeto é um notificador de doações. Um dispositivo ESP32 com display OLED precisa exibir uma mensagem de "Obrigado!" quando uma doação via PIX é confirmada.

A arquitetura do sistema é desacoplada:

Uma Aplicação Web (em Vercel) lida com a criação do pagamento via API do Mercado Pago.

Quando o pagamento é aprovado, a Aplicação Web escreve um status em um Firebase Realtime Database.

O ESP32 (nosso foco) precisa "escutar" esse banco de dados e reagir à mudança de status.

O Problema a ser Resolvido:
O código atual do ESP32 está com dificuldades para estabelecer e manter uma conexão de stream funcional com o Firebase Realtime Database. Estamos enfrentando erros de baixo nível na conexão segura (TLS/SSL), como Connection reset by peer e Incoming protocol or record version is unsupported.

Requisitos e Constraints Essenciais:

Biblioteca: O projeto deve usar a versão estável V4 da biblioteca mobizt/Firebase ESP32 Client (configurada no platformio.ini como @4.4.1). O código precisa ser 100% compatível com a API desta versão.

Sincronização de Tempo: Já descobrimos que a sincronização de tempo com um servidor NTP após a conexão Wi-Fi é obrigatória para resolver os erros de SSL/TLS.

Caminho no Firebase: O ESP32 deve monitorar o nó /payment_status. A lógica deve ser acionada quando o campo filho status dentro deste nó for alterado para o valor "approved".

Ação Final: Após detectar o status "approved", o ESP32 deve atualizar o display OLED e, em seguida, alterar o status no Firebase para "processed" para evitar notificações duplicadas.