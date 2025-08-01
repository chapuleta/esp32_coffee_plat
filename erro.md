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
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060a30  A0      : 0x800dcac4  A1      : 0x3ffb21f0  
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0x124f61cd  A12     : 0x124f61cd  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060a30  A0      : 0x800dcac4  A1      : 0x3ffb21f0
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0xa04943ed  A12     : 0xa04943ed  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f  


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060830  A0      : 0x800dcac4  A1      : 0x3ffb21f0  
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0xa70e6bde  A12     : 0xa70e6bde  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f  


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060830  A0      : 0x800dcac4  A1      : 0x3ffb21f0  
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0x13e0d616  A12     : 0x13e0d616  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f  


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060a30  A0      : 0x800dcac4  A1      : 0x3ffb21f0  
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0xe73d4e84  A12     : 0xe73d4e84  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f  


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060a30  A0      : 0x800dcac4  A1      : 0x3ffb21f0
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0x597fc381  A12     : 0x597fc381  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f  


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060830  A0      :00x800dcac4  A1      : 0x3ffb21f0
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0x97d0729f  A12     : 0x97d0729f  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060a30  A0      : 0x800dcac4  A1      : 0x3ffb21f0
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0x5ca4777f  A12     : 0x5ca4777f  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060830  A0      : 0x800dcac4  A1      : 0x3ffb21f0
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0x12602469  A12     : 0x12602469  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060830  A0      : 0x800dcac4  A1      : 0x3ffb21f0  
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0xc8c320bb  A12     : 0xc8c320bb  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT) 
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
  #0  0x40080400 in _init at ??:?

entry 0x400805e4
Conectando ao Wi-Fi...Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.     

Core  1 register dump:
PC      : 0x40089e78  PS      : 0x00060830  A0      : 0x800dcac4  A1      : 0x3ffb21f0  
  #0  0x40089e78 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:150

A2      : 0x00000000  A3      : 0x00000000  A4      : 0x00000400  A5      : 0x00000000
A6      : 0x00ff0000  A7      : 0x00000040  A8      : 0x0000008d  A9      : 0x0000000d
A10     : 0xffffff80  A11     : 0x037b9522  A12     : 0x037b9522  A13     : 0x3ffc4898
A14     : 0x3ffb21dc  A15     : 0x3ffbd0bc  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
EXCVADDR: 0x00000000  LBEG    : 0x40089e78  LEND    : 0x40089e83  LCOUNT  : 0x0000003f


Backtrace: 0x40089e75:0x3ffb21f0 0x400dcac1:0x3ffb2200 0x400d5276:0x3ffb2220 0x400dbab6:0x3ffb2240 0x400e78f6:0x3ffb2290
  #0  0x40089e75 in memset at /builds/idf/crosstool-NG/.build/HOST-x86_64-w64-mingw32/xtensa-esp32-elf/src/newlib/newlib/libc/machine/xtensa/memset.S:143
  #1  0x400dcac1 in Adafruit_SSD1306::clearDisplay() at .pio/libdeps/esp32dev/Adafruit SSD1306/Adafruit_SSD1306.cpp:687
  #2  0x400d5276 in printOLED(String const&, int, bool) at src/main.cpp:54
  #3  0x400dbab6 in setup() at src/main.cpp:172        
  #4  0x400e78f6 in loopTask(void*) at C:/Users/victo/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:42





ELF file SHA256: de53ab4b0b86ec45

 *  Terminal will be reused by tasks, press any key to close it. 
