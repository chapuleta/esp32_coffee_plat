 *  Executing task: C:\Users\victo\.platformio\penv\Scripts\platformio.exe device monitor 


Please build project in debug configuration to get more details about an exception.
See https://docs.platformio.org/page/projectconf/build_configurations.html


--- Terminal on COM4 | 115200 8-N-1
--- Available filters and text transformations: colorize, debug, default, direct, esp32_exception_decoder, hexlify, log2file, nocontrol, printable, send_on_enter, time
--- More details at https://bit.ly/pio-monitor-filters
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H
ets Jul 29 2019 12:21:46

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...........................
Wi-Fi conectado.
Endereco IP: 192.168.18.62
Sincronizando hora com NTP...
Hora sincronizada.
11:17:31
Configuring Firebase...
Event: authTask, Message: authenticating, Code: 7
Event: authTask, Message: auth request sent, Code: 8
Servidor web iniciado.
Event: authTask, Message: auth response received, Code: 9
Event: authTask, Message: ready, Code: 10
Stream Data Path: /
Stream Value: {"status":"approved"}
Is Stream: true
Stream Type: 6
 *  Terminal will be reused by tasks, press any key to close it. 