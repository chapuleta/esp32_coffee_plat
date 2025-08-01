# AsyncResult

## Description

The async result class.

```cpp
class AsyncResult
```

1. ## 🔹  const char *c_str()

Get the pointer to the internal response payload string buffer.

```cpp
const char *c_str()
```

**Returns:**

- `const char *` - The pointer to internal response payload string.


2. ## 🔹  String payload() const

Get the server response payload string.

```cpp
String payload() const
```

**Returns:**

- `const String` - The payload string.


3. ## 🔹  size_t length() const

Get the length of response payload string.

```cpp
size_t length() const
```

**Returns:**

- `size_t` - The payload string length.


4. ## 🔹  String path() const

Get the path of the resource of the request.

```cpp
String path() const
```

**Returns:**

- `String` - The path of the resource of the request.


5. ## 🔹  String etag() const

Get the etag of the server response headers.

```cpp
String etag() const
```

**Params:**

- `String` - The ETag of response header.


6. ## 🔹   String uid() const

Get the unique identifier of async task.

```cpp
String uid() const
```

**Returns:**

- `String` - The UID of async task.


7. ## 🔹  String debug()

Get the debug information.

```cpp
String debug()
```

**Returns:**

- `String` - The debug information.

8. ## 🔹  void clear()

Clear the async result.

```cpp
void clear()
```

9. ## 🔹  RealtimeDatabaseResult &to()

Get the reference to the internal RealtimeDatabaseResult object.

```cpp
RealtimeDatabaseResult &to()
```

**Returns:**

- `RealtimeDatabaseResult &` - The reference to the internal RealtimeDatabaseResult object.


10. ## 🔹  int available()

Get the number of bytes of available response payload.

```cpp
int available()
```

**Returns:**

- `int` - The number of bytes available.


11. ## 🔹  bool isResult()

Check if the AsyncResult data/information is available.

```cpp
bool isResult()
```

**Returns:**

- `bool` - Returns true if data/information is available.


12. ## 🔹  app_log_t &appEvent()

Get the reference of internal app event information.

```cpp
app_log_t &EventLog()
```

```cpp
app_log_t &appEvent()
```

**Returns:**

- `app_log_t &` - The reference of internal app event.


13. ## 🔹  app_log_t &debugLog()

Get the reference of internal app debug information.

```cpp
app_log_t &debugLog()
```

**Returns:**

- `app_log_t &` - The reference of internal app data log.


14. ## 🔹  bool uploadProgress()

Check if file/BLOB upload information is available.

```cpp
bool uploadProgress()
```

**Returns:**

- `bool` - Returns true if upload information is available.


15. ## 🔹  upload_data_t uploadInfo() const

Get the file/BLOB upload information.

```cpp
upload_data_t uploadInfo() const
```

**Returns:**

- `upload_data_t` - The file/BLOB upload information.


16. ## 🔹  bool downloadProgress()

Check if the file/BLOB download information is availablle.

```cpp
bool downloadProgress()
```

**Returns:**

- `bool` - Returns true if download information is available.


17. ## 🔹  download_data_t downloadInfo() const

Get the file/BLOB download information.

```cpp
download_data_t downloadInfo() const
```

**Returns:**

- `download_data_t` - The file/BLOB download information.


18. ## 🔹  bool isOTA() const

Check if the result is from OTA download task.

```cpp
bool isOTA() const
```

**Returns:**

- `bool` - Returns true if the result is from OTA download task.


19. ## 🔹  bool isError()

Check if the error occurred in async task.

```cpp
bool isError()
```

**Returns:**

- `bool` - Returns true if error occurred.


20. ## 🔹  bool isDebug()

Check if the debug information in available.

```cpp
bool isDebug()
```

**Returns:**

- `bool` - Returns true if debug information in available.


21. ## 🔹  bool isEvent()

Check if the app event information in available.

```cpp
bool isEvent()
```

**Returns:**

- `bool` - Returns true if app event information in available.


22. ## 🔹  FirebaseError &error()

Get the reference of internal FirebaseError object.

```cpp
FirebaseError &error()
```

**Returns:**

- `FirebaseError &` - The internal FirebaseError object.