Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\Adafruit_GFX.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\Adafruit_GrayOLED.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\Adafruit_SPITFT.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\glcdfont.c.o
src/main.cpp:3:10: fatal error: FirebaseESP32.h: No such file or directory

***********************************************************************
* Looking for FirebaseESP32.h dependency? Check our library registry!
*
* CLI  > platformio lib search "header:FirebaseESP32.h"
* Web  > https://registry.platformio.org/search?q=header:FirebaseESP32.h
*
***********************************************************************

 #include <FirebaseESP32.h>
          ^~~~~~~~~~~~~~~~~
compilation terminated.
*** [.pio\build\esp32dev\src\main.cpp.o] Error 1
====================================== [FAILED] Took 23.02 seconds =================





---



PS C:\Users\victo\OneDrive\Documentos\PlatformIO\Projects\esp32_coffee_plat> 1 platformio run
No linha:1 caractere:3
+ 1 platformio run
+   ~~~~~~~~~~
Token 'platformio' inesperado na expressão ou instrução.
    + CategoryInfo          : ParserError: (:) [], ParentContainsErrorRecordException
    + FullyQualifiedErrorId : UnexpectedToken
 
PS C:\Users\victo\OneDrive\Documentos\PlatformIO\Projects\esp32_coffee_plat> 1 pio run
No linha:1 caractere:3
+ 1 pio run
+   ~~~
Token 'pio' inesperado na expressão ou instrução.
    + CategoryInfo          : ParserError: (:) [], ParentContainsErrorRecordException
    + FullyQualifiedErrorId : UnexpectedToken

PS C:\Users\victo\OneDrive\Documentos\PlatformIO\Projects\esp32_coffee_plat> 



---



Compiling .pio\build\esp32dev\libc36\Adafruit BusIO\Adafruit_SPIDevice.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\Adafruit_GFX.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\Adafruit_GrayOLED.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\Adafruit_SPITFT.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\glcdfont.c.o
src/main.cpp:34:21: error: variable or field 'streamCallback' declared void
 void streamCallback(StreamData data) {
                     ^~~~~~~~~~
src/main.cpp:34:21: error: 'StreamData' was not declared in this scope
src/main.cpp:34:21: note: suggested alternative: 'Stream'
 void streamCallback(StreamData data) {
                     ^~~~~~~~~~
                     Stream
src/main.cpp: In function 'void setup()':
src/main.cpp:111:10: error: 'FirebaseConfig' {aka 'struct firebase_cfg_t'} has no member named 'stream_callback'; did you mean 'token_status_callback'?
   config.stream_callback = streamCallback;
          ^~~~~~~~~~~~~~~
          token_status_callback
src/main.cpp:111:28: error: 'streamCallback' was not declared in this scope
   config.stream_callback = streamCallback;
                            ^~~~~~~~~~~~~~
src/main.cpp:111:28: note: suggested alternative: 'streamTimeoutCallback'
   config.stream_callback = streamCallback;
                            ^~~~~~~~~~~~~~
                            streamTimeoutCallback
src/main.cpp:112:10: error: 'FirebaseConfig' {aka 'struct firebase_cfg_t'} has no member named 'timeout_callback'; did you mean 'token_status_callback'?
   config.timeout_callback = streamTimeoutCallback;
          ^~~~~~~~~~~~~~~~
          token_status_callback
src/main.cpp:119:10: error: 'FirebaseConfig' {aka 'struct firebase_cfg_t'} has no member named 'setMaxPayloadLength'
   config.setMaxPayloadLength(4096); // For FirebaseConfig object
          ^~~~~~~~~~~~~~~~~~~
src/main.cpp:122:17: error: 'class Firebase_ESP_Client' has no member named 'beginStream'
   if (!Firebase.beginStream(fbdo, "/payment_status")) {
                 ^~~~~~~~~~~
src/main.cpp: In function 'void loop()':
src/main.cpp:130:16: error: 'class Firebase_ESP_Client' has no member named 'is'
   if (Firebase.is
                ^~
src/main.cpp:130:18: error: expected ')' before 'connected'
   if (Firebase.is
      ~           ^
                  )
   connected()) {
   ~~~~~~~~~
Compiling .pio\build\esp32dev\lib6b9\Adafruit SSD1306\Adafruit_SSD1306.cpp.o
Archiving .pio\build\esp32dev\libe4f\libSPI.a
Archiving .pio\build\esp32dev\lib37b\libWire.a
Compiling .pio\build\esp32dev\libf6b\Update\HttpsOTAUpdate.cpp.o
*** [.pio\build\esp32dev\src\main.cpp.o] Error 1
======================================= [FAILED] Took 8.39 seconds =======================================

 *  The terminal process "C:\Users\victo\.platformio\penv\Scripts\platformio.exe 'run', '--target', 'upload'" terminated with exit code: 1. 
 *  Terminal will be reused by tasks, press any key to close it. 


---

 52 compatible libraries
Scanning dependencies...
Dependency Graph
|-- Adafruit SSD1306 @ 2.5.15
|-- Adafruit GFX Library @ 1.12.1
|-- Firebase Arduino Client Library for ESP8266 and ESP32 @ 4.4.17+sha.3d26298
|-- NTPClient @ 3.2.1+sha.fab3f49
|-- WiFi @ 2.0.0
|-- Preferences @ 2.0.0
|-- WebServer @ 2.0.0
Building in release mode
Compiling .pio\build\esp32dev\src\main.cpp.o
Archiving .pio\build\esp32dev\libf6b\libUpdate.a
src/main.cpp: In function 'void streamCallback(FirebaseData&)':
src/main.cpp:35:64: error: invalid use of member function 'String FirebaseData::dataPath()' (did you forget the '()' ?)
   Serial.printf("Stream path: %s, Type: %s, Event: %s\n", data.dataPath.c_str(), data.dataType.c_str(), data.eventType.c_str());
                                                           ~~~~~^~~~~~~~
src/main.cpp:35:87: error: invalid use of member function 'String FirebaseData::dataType()' (did you forget the '()' ?)
   Serial.printf("Stream path: %s, Type: %s, Event: %s\n", data.dataPath.c_str(), data.dataType.c_str(), data.eventType.c_str());
                                                                                  ~~~~~^~~~~~~~
src/main.cpp:35:110: error: invalid use of member function 'String FirebaseData::eventType()' (did you forget the '()' ?)
   Serial.printf("Stream path: %s, Type: %s, Event: %s\n", data.dataPath.c_str(), data.dataType.c_str(), data.eventType.c_str());
                                                                                                         ~~~~~^~~~~~~~~
src/main.cpp:37:12: error: invalid use of member function 'String FirebaseData::dataType()' (did you forget the '()' ?)
   if (data.dataType == "json") {
       ~~~~~^~~~~~~~
src/main.cpp:38:31: error: 'class FirebaseData' has no member named 'json'
     FirebaseJson &json = data.json;
                               ^~~~
src/main.cpp: In function 'void setup()':
src/main.cpp:115:17: error: 'class FB_RTDB' has no member named 'setMaxPayloadLength'; did you mean 'getPayloadLen'?
   Firebase.RTDB.setMaxPayloadLength(&fbdo, 4096); // For FirebaseData object
                 ^~~~~~~~~~~~~~~~~~~
                 getPayloadLen
src/main.cpp:118:12: error: 'class Firebase_ESP_Client' has no member named 'setStreamCallback'
   Firebase.setStreamCallback(fbdo, streamCallback, streamTimeoutCallback);
            ^~~~~~~~~~~~~~~~~
Compiling .pio\build\esp32dev\lib014\FS\vfs_api.cpp.o
*** [.pio\build\esp32dev\src\main.cpp.o] Error 1
======================================= [FAILED] Took 7.66 seconds =======================================

 *  The terminal process "C:\Users\victo\.platformio\penv\Scripts\platformio.exe 'run', '--target', 'upload'" terminated with exit code: 1. 
 *  Terminal will be reused by tasks, press any key to close it. 


---

-- Firebase Arduino Client Library for ESP8266 and ESP32 @ 4.4.17+sha.3d26298
|-- NTPClient @ 3.2.1+sha.fab3f49
|-- WiFi @ 2.0.0
|-- Preferences @ 2.0.0
|-- WebServer @ 2.0.0
Building in release mode
Compiling .pio\build\esp32dev\src\main.cpp.o
src/main.cpp: In function 'void streamCallback(FirebaseData&)':
src/main.cpp:38:31: error: 'class FirebaseData' has no member named 'json'
     FirebaseJson *json = data.json();
                               ^~~~
src/main.cpp: In function 'void setup()':
src/main.cpp:115:10: error: 'FirebaseConfig' {aka 'struct firebase_cfg_t'} has no member named 'setMaxPayloadLength'
   config.setMaxPayloadLength(4096); // For FirebaseConfig object
          ^~~~~~~~~~~~~~~~~~~
src/main.cpp:118:42: error: invalid conversion from 'void (*)(FirebaseData&)' to 'FirebaseData::StreamEventCallback' {aka 'void (*)(FirebaseStream)'} [-fpermissive]
   Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);
                                          ^~~~~~~~~~~~~~
In file included from .pio/libdeps/esp32dev/Firebase Arduino Client Library for ESP8266 and ESP32/src/Firebase.h:60,
                 from src/main.cpp:3:
.pio/libdeps/esp32dev/Firebase Arduino Client Library for ESP8266 and ESP32/src/rtdb/FB_RTDB.h:2204:80: note:   initializing argument 2 of 'void FB_RTDB::setStreamCallback(FirebaseData*, FirebaseData::StreamEventCallback, FirebaseData::StreamTimeoutCallback, size_t)'
   void setStreamCallback(FirebaseData *fbdo, FirebaseData::StreamEventCallback dataAvailableCallback,    
                                              ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~     
Archiving .pio\build\esp32dev\lib014\libFS.a
*** [.pio\build\esp32dev\src\main.cpp.o] Error 1
======================================= [FAILED] Took 5.75 seconds =======================================

 *  The terminal process "C:\Users\victo\.platformio\penv\Scripts\platformio.exe 'run', '--target', 'upload'" terminated with exit code: 1. 
 *  Terminal will be reused by tasks, press any key to close it. 


---


- Preferences @ 2.0.0
|-- WebServer @ 2.0.0
Building in release mode
Compiling .pio\build\esp32dev\src\main.cpp.o
src/main.cpp: In function 'void streamCallback(FirebaseStream)':
src/main.cpp:38:31: error: 'class FirebaseStream' has no member named 'json'
     FirebaseJson *json = data.json();
                               ^~~~
Compiling .pio\build\esp32dev\lib500\LittleFS\LittleFS.cpp.o
*** [.pio\build\esp32dev\src\main.cpp.o] Error 1
======================================= [FAILED] Took 8.04 seconds =======================================

 *  The terminal process "C:\Users\victo\.platformio\penv\Scripts\platformio.exe 'run', '--target', 'upload'" terminated with exit code: 1. 
 *  Terminal will be reused by tasks, press any key to close it. 


 ---

  WiFi @ 2.0.0
|-- Preferences @ 2.0.0
|-- WebServer @ 2.0.0
Building in release mode
Compiling .pio\build\esp32dev\src\main.cpp.o
src/main.cpp: In function 'void streamCallback(FirebaseStream)':
src/main.cpp:38:31: error: 'class FirebaseStream' has no member named 'json'
     FirebaseJson *json = data.json();
                               ^~~~
Archiving .pio\build\esp32dev\lib500\libLittleFS.a
*** [.pio\build\esp32dev\src\main.cpp.o] Error 1
======================================= [FAILED] Took 6.14 seconds =======================================

 *  The terminal process "C:\Users\victo\.platformio\penv\Scripts\platformio.exe 'run', '--target', 'upload'" terminated with exit code: 1. 
 *  Terminal will be reused by tasks, press any key to close it. 


 ---

  *  Executing task: C:\Users\victo\.platformio\penv\Scripts\platformio.exe device monitor 

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
entry 0x400805e4
Connecting to Wi-Fi..........
WiFi connected.
IP Address: 192.168.18.62
Synchronizing time with NTP server...

Time synchronized.
06:13:22
> WARN.mRunUntil: Terminating because the ssl engine closed.
> ERROR.mConnectSSL: Failed to initlalize the SSL layer.
> ERROR.mConnectSSL: SSL received fatal alert - Unknown error code.



--

 *  Executing task: C:\Users\victo\.platformio\penv\Scripts\platformio.exe device monitor 

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
entry 0x400805e4
Connecting to Wi-Fi...........
WiFi connected.
IP Address: 192.168.18.62
Synchronizing time with NTP server...

Time synchronized.
06:15:38
> WARN.mRunUntil: Terminating because the ssl engine closed.
> ERROR.mConnectSSL: Failed to initlalize the SSL layer.
> ERROR.mConnectSSL: Incoming protocol or record version is unsupported.
Stream timeout, re-establishing...
not connected
Stream path: /, Type: json, Event: put
Status: approved
Donation approved! Updating OLED and Firebase status...
Firebase status updated to processed.
Stream path: /, Type: json, Event: put
Status: processed


----


 *  Executing task: C:\Users\victo\.platformio\penv\Scripts\platformio.exe device monitor 

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
entry 0x400805e4
Connecting to Wi-Fi..........................
WiFi connected.
IP Address: 192.168.18.62
Synchronizing time with NTP server...

Time synchronized.
06:24:43
Stream timeout, re-establishing...
not connected
> ERROR.mRunUntil: SSL internals timed out!
> ERROR.mConnectSSL: Failed to initlalize the SSL layer.
> ERROR.mConnectSSL: Unknown error code.
Stream error:
Stream path: /, Type: json, Event: put
Status: processed



Stream path: /, Type: null, Event: put
Stream path: /, Type: json, Event: put
Status: approved
Donation approved! Updating OLED and Firebase status...
Firebase status updated to processed.
Stream path: /, Type: json, Event: put
Status: processed

---


iling .pio\build\esp32dev\libe4f\SPI\SPI.cpp.o
Compiling .pio\build\esp32dev\libc36\Adafruit BusIO\Adafruit_BusIO_Register.cpp.o
Compiling .pio\build\esp32dev\libc36\Adafruit BusIO\Adafruit_GenericDevice.cpp.o
Compiling .pio\build\esp32dev\libc36\Adafruit BusIO\Adafruit_I2CDevice.cpp.o
Compiling .pio\build\esp32dev\libc36\Adafruit BusIO\Adafruit_SPIDevice.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\Adafruit_GFX.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\Adafruit_GrayOLED.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\Adafruit_SPITFT.cpp.o
Compiling .pio\build\esp32dev\lib629\Adafruit GFX Library\glcdfont.c.o
src/main.cpp:12:1: error: 'FirebaseData' does not name a type; did you mean 'Firebase'?
 FirebaseData fbdo;
 ^~~~~~~~~~~~
 Firebase
src/main.cpp:13:1: error: 'FirebaseAuth' does not name a type; did you mean 'FirebaseApp'?
 FirebaseAuth auth;
 ^~~~~~~~~~~~
 FirebaseApp
src/main.cpp:14:1: error: 'FirebaseConfig' does not name a type; did you mean 'Firebase'?
 FirebaseConfig config;
 ^~~~~~~~~~~~~~
 Firebase
src/main.cpp:34:21: error: variable or field 'streamCallback' declared void
 void streamCallback(FirebaseStream data) {
                     ^~~~~~~~~~~~~~
src/main.cpp:34:21: error: 'FirebaseStream' was not declared in this scope
src/main.cpp:34:21: note: suggested alternative: 'FirebaseError'
 void streamCallback(FirebaseStream data) {
                     ^~~~~~~~~~~~~~
                     FirebaseError
src/main.cpp: In function 'void streamTimeoutCallback(bool)':
src/main.cpp:72:18: error: 'fbdo' was not declared in this scope
   Serial.println(fbdo.errorReason());
                  ^~~~
src/main.cpp: In function 'void setup()':
src/main.cpp:110:3: error: 'config' was not declared in this scope
   config.host = FIREBASE_HOST;
   ^~~~~~
src/main.cpp:110:3: note: suggested alternative: 'confstr'
   config.host = FIREBASE_HOST;
   ^~~~~~
   confstr
src/main.cpp:113:12: error: 'class firebase_ns::FirebaseClient' has no member named 'begin'
   Firebase.begin(&config, &auth);
            ^~~~~
src/main.cpp:113:28: error: 'auth' was not declared in this scope
   Firebase.begin(&config, &auth);
                            ^~~~
src/main.cpp:113:28: note: suggested alternative: 'auto'
   Firebase.begin(&config, &auth);
                            ^~~~
                            auto
src/main.cpp:114:12: error: 'class firebase_ns::FirebaseClient' has no member named 'reconnectWiFi'       
   Firebase.reconnectWiFi(true);
            ^~~~~~~~~~~~~
src/main.cpp:117:3: error: 'fbdo' was not declared in this scope
   fbdo.setBSSLBufferSize(4096, 1024); // For FirebaseData object
   ^~~~
src/main.cpp:119:12: error: 'class firebase_ns::FirebaseClient' has no member named 'RTDB'
   Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);
            ^~~~
src/main.cpp:119:42: error: 'streamCallback' was not declared in this scope
   Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);
                                          ^~~~~~~~~~~~~~
src/main.cpp:119:42: note: suggested alternative: 'streamTimeoutCallback'
   Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);
                                          ^~~~~~~~~~~~~~
                                          streamTimeoutCallback
src/main.cpp:122:17: error: 'class firebase_ns::FirebaseClient' has no member named 'RTDB'
   if (!Firebase.RTDB.beginStream(&fbdo, "/payment_status")) {
                 ^~~~
src/main.cpp: In function 'void loop()':
src/main.cpp:130:16: error: 'class firebase_ns::FirebaseClient' has no member named 'ready'
   if (Firebase.ready()) {
                ^~~~~
Compiling .pio\build\esp32dev\lib6b9\Adafruit SSD1306\Adafruit_SSD1306.cpp.o
*** [.pio\build\esp32dev\src\main.cpp.o] Error 1
======================================= [FAILED] Took 8.96 seconds =======================================

 *  The terminal process "C:\Users\victo\.platformio\penv\Scripts\platformio.exe 'run', '--target', 'upload'" terminated with exit code: 1. 
 *  Terminal will be reused by tasks, press any key to close it. 



 ---


 dafruit SSD1306 @ 2.5.15
|-- Adafruit GFX Library @ 1.12.1
|-- FirebaseClient @ 2.1.5
|-- NTPClient @ 3.2.1+sha.fab3f49
|-- WiFi @ 2.0.0
|-- Preferences @ 2.0.0
|-- WebServer @ 2.0.0
Building in release mode
Compiling .pio\build\esp32dev\src\main.cpp.o
src/main.cpp:12:1: error: 'RealtimeDatabase' does not name a type
 RealtimeDatabase Database;
 ^~~~~~~~~~~~~~~~
src/main.cpp: In function 'void setup()':
src/main.cpp:76:12: error: 'struct firebase_ns::user_auth_data' has no member named 'legacy_token'        
   userAuth.legacy_token.val[app_tk_ns::token] = FIREBASE_AUTH;
            ^~~~~~~~~~~~
src/main.cpp:77:3: error: 'Database' was not declared in this scope
   Database.url(FIREBASE_HOST);
   ^~~~~~~~
src/main.cpp:77:3: note: suggested alternative: 'Databases'
   Database.url(FIREBASE_HOST);
   ^~~~~~~~
   Databases
src/main.cpp:81:44: error: 'streamEventCallback' was not declared in this scope
   Database.get(aClient, "/payment_status", streamEventCallback, true);
                                            ^~~~~~~~~~~~~~~~~~~
src/main.cpp:83:13: error: a function-definition is not allowed here before '{' token
 void loop() {
             ^
src/main.cpp:94:1: error: expected '}' at end of input
 }
 ^
src/main.cpp:41:14: note: to match this '{'
 void setup() {
              ^
Archiving .pio\build\esp32dev\libf5c\libSD.a
*** [.pio\build\esp32dev\src\main.cpp.o] Error 1
======================================= [FAILED] Took 5.68 seconds =======================================

 *  The terminal process "C:\Users\victo\.platformio\penv\Scripts\platformio.exe 'run', '--target', 'upload'" terminated with exit code: 1. 
 *  Terminal will be reused by tasks, press any key to close it. 


 ---

  WiFi @ 2.0.0
|-- Preferences @ 2.0.0
|-- WebServer @ 2.0.0
Building in release mode
Compiling .pio\build\esp32dev\src\main.cpp.o
src/main.cpp:13:1: error: 'RealtimeDatabase' does not name a type
 RealtimeDatabase Database;
 ^~~~~~~~~~~~~~~~
src/main.cpp: In function 'void streamEventCallback(AsyncResult&)':
src/main.cpp:42:15: error: 'class AsyncResult' has no member named 'rtdbResult'; did you mean 'isResult'? 
   if (aResult.rtdbResult.isStream()) {
               ^~~~~~~~~~
               isResult
src/main.cpp:43:69: error: 'class AsyncResult' has no member named 'rtdbResult'; did you mean 'isResult'? 
     Serial.printf("Stream path: %s, Type: %s, Event: %s\n", aResult.rtdbResult.dataPath().c_str(), aResult.rtdbResult.type().c_str(), aResult.rtdbResult.event().c_str());
                                                                     ^~~~~~~~~~
                                                                     isResult
src/main.cpp:43:108: error: 'class AsyncResult' has no member named 'rtdbResult'; did you mean 'isResult'?
     Serial.printf("Stream path: %s, Type: %s, Event: %s\n", aResult.rtdbResult.dataPath().c_str(), aResult.rtdbResult.type().c_str(), aResult.rtdbResult.event().c_str());
                                                                                                          
  ^~~~~~~~~~
                                                                                                          
  isResult
src/main.cpp:43:143: error: 'class AsyncResult' has no member named 'rtdbResult'; did you mean 'isResult'?
     Serial.printf("Stream path: %s, Type: %s, Event: %s\n", aResult.rtdbResult.dataPath().c_str(), aResult.rtdbResult.type().c_str(), aResult.rtdbResult.event().c_str());
                                                                                                          
                                     ^~~~~~~~~~
                                                                                                          
                                     isResult
src/main.cpp:45:17: error: 'class AsyncResult' has no member named 'rtdbResult'; did you mean 'isResult'? 
     if (aResult.rtdbResult.type() == realtime_database_data_type_json) {
                 ^~~~~~~~~~
                 isResult
src/main.cpp:46:7: error: 'FirebaseJsonData' was not declared in this scope
       FirebaseJsonData result;
       ^~~~~~~~~~~~~~~~
src/main.cpp:46:7: note: suggested alternative: 'Firebase'
       FirebaseJsonData result;
       ^~~~~~~~~~~~~~~~
       Firebase
src/main.cpp:49:15: error: 'class AsyncResult' has no member named 'rtdbResult'; did you mean 'isResult'? 
       aResult.rtdbResult.data().get(result, "status");
               ^~~~~~~~~~
               isResult
src/main.cpp:49:37: error: 'result' was not declared in this scope
       aResult.rtdbResult.data().get(result, "status");
                                     ^~~~~~
src/main.cpp:49:37: note: suggested alternative: 'aResult'
       aResult.rtdbResult.data().get(result, "status");
                                     ^~~~~~
                                     aResult
src/main.cpp:60:11: error: 'Database' was not declared in this scope
           Database.set(aClient, "/payment_status/status", "processed");
           ^~~~~~~~
src/main.cpp:60:11: note: suggested alternative: 'Databases'
           Database.set(aClient, "/payment_status/status", "processed");
           ^~~~~~~~
           Databases
src/main.cpp: In function 'void setup()':
src/main.cpp:114:12: error: 'struct firebase_ns::user_auth_data' has no member named 'legacy_token'       
   userAuth.legacy_token.val[app_tk_ns::token] = FIREBASE_AUTH;
            ^~~~~~~~~~~~
src/main.cpp:115:3: error: 'Database' was not declared in this scope
   Database.url(FIREBASE_HOST);
   ^~~~~~~~
src/main.cpp:115:3: note: suggested alternative: 'Databases'
   Database.url(FIREBASE_HOST);
   ^~~~~~~~
   Databases
Archiving .pio\build\esp32dev\lib256\libSPIFFS.a
*** [.pio\build\esp32dev\src\main.cpp.o] Error 1
======================================= [FAILED] Took 6.87 seconds =======================================

 *  The terminal process "C:\Users\victo\.platformio\penv\Scripts\platformio.exe 'run', '--target', 'upload'" terminated with exit code: 1. 
 *  Terminal will be reused by tasks, press any key to close it. 