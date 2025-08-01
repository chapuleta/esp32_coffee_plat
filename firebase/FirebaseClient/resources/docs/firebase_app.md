# FirebaseApp

## Description

The Firebase App class that handles the authentication/authorization task.

```cpp
class FirebaseApp
```

## Functions

1. ## 🔹  bool isInitialized() const

Get the app initialization status.

```cpp
bool isInitialized() const
```

**Returns:**

- `bool` - Return true if initialized.


2. ## 🔹  void loop(JWTClass *jwt = nullptr)

The authentication and async tasks handler.

Since v2.0.0, the async task in any AsyncClient's queue that belongs to this app will be maintained to run which includes the tasks in the AsyncClient's queue that was assigned to the initializeApp function.

Then calling individual Firebase service's class's loop is not neccessary and can be ignored.

```cpp
void loop(JWTClass *jwt = nullptr)
```

**Params:**

- `jwt` - Optional. The pointer to `JWTClass` class object to handle the JWT token generation and signing.

3. ## 🔹  bool ready()

Get the authentication/autorization process status.

```cpp
bool ready()
```

**Returns:**

- `bool` - Return true if the auth process was finished. Returns false if `isExpired()` returns true.


4. ## 🔹  void getApp(T &app)

Appy the authentication/authorization credentials to the Firebase services app.

```cpp
void getApp(T &app)
```

**Params:**

- `app` - The Firebase services calss object e.g. RealtimeDatabase, Storage, Messaging, CloudStorage and CloudFunctions.


5. ## 🔹   String getToken() const

Get the auth token.

```cpp
String getToken() const
```

**Returns:**

- `String` - String of auth tokens based on the authentication/authoeization e.g. ID token and access token.


6. ## 🔹  firebase_token_type getTokenType()

Get the token type.

```cpp
firebase_token_type getTokenType()
```

**Returns:**

- `firebase_token_type` enums - The enums are included following: `token_type_id` (3), `token_type_access` (2), `token_type_legacy` (1) and `token_type_no` (0).


7. ## 🔹  String getRefreshToken() const

Get the refresh token.

```cpp
String getRefreshToken() const
```

**Returns:**

- `String` - String of refresh token after user sign in or authorization using ID token.

8. ## 🔹  String getUid() const

Get unique identifier.

```cpp
String getUid() const
```

**Returns:**

- `String` - String of unique identifier after user sign in or authorization using ID token.

9. ## 🔹  bool isAuthenticated() const

Get the authentication status since app initialized.

```cpp
bool isAuthenticated() const
```

**Returns:**

- `bool` - Return true once authenticated since initialized. It will reset when app re-initialized and user management task was executed.


10. ## 🔹  bool isExpired()

Get the auth token expiration status.

```cpp
bool isExpired()
```

**Returns:**

- `bool` - Return true if auth token was expired upon the expire period setup.


11. ## 🔹  unsigned long ttl()

Get the remaining time to live of token until expired.

```cpp
unsigned long ttl()
```

**Params:**

- `unsigned long` - The integer value of ttl.


12. ## 🔹  void setCallback(AsyncResultCallback cb) 

Set the async result callback function.

```cpp
void setCallback(AsyncResultCallback cb)
```

**Params:**

- `cb` - The async result callback function (AsyncResultCallback).


13. ## 🔹  void setAsyncResult(AsyncResult &aResult)

Set the async result class object.

```cpp
void setAsyncResult(AsyncResult &aResult)
```

**Params:**

- `aResult` - The async result (AsyncResult).


14. ## 🔹  void setUID(const String &uid)

Set the UID for authentication task.

The UID will be applied when used with the async result callback only.

The async result object set via `initializeApp` and `FirebaseApp::setAsyncResult` will not take effect.

```cpp
void setUID(const String &uid)
```

**Params:**

- `uid` - The unique identifier for the authentication task.


15. ## 🔹  void setTime(uint32_t sec)

Set the app UNIX timestamp.

```cpp
void setTime(uint32_t sec)
```

**Params:**

- `sec` - The UNIX timestamp in seconds.

**Params:**

- `jwtClass` - The JWT token processor object.

16. ## 🔹  auth_data_t *getAuth()

Get the pointer to the internal auth data.

```cpp
auth_data_t *getAuth()
```

**Returns:**

- `auth_data_t*` - The pointer to internal auth data.


17. ## 🔹  void autoRefresh(bool enable)

Set the option to enable/disable re-authentication.

```cpp
void autoAuthenticate(bool enable)
```

**Params:**

- `enable` - Set to true to enable or false to disable.

18. ## 🔹  void authenticate()

Force library to re-authenticate (refresh the auth token).

```cpp
void authenticate()
```

19. ## 🔹  void setJWTProcessor(JWTClass &jwtClass)

Set the JWT token processor object (DEPRECATED).

This function should be executed before calling initializeApp.

Use FirebaseApp::loop function instead for assigning the JWT token processor for individual app (thread safe).

```cpp
void setJWTProcessor(JWTClass &jwtClass)
```

**Params:**

- `jwtClass` - The pointer to JWTClass class object to handle the JWT token generation and signing.