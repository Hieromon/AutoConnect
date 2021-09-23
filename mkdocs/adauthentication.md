The Sketch may use authentication to protect custom Web pages and prevent unauthorized access. AutoConnect has implemented HTTP authentication feature that can be applied to multiple scopes using the [authentication methods](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer#authentication) provided by the platform's WebServer library for ESP8266 or ESP32.[^1]

[^1]:ESP32 Arduino core has the authenticate method provided by the WebServer library, similar to that of the ESP8266. 

- [Applying HTTP authentication](#applying-http-authentication)
- [Applying HTTP authentication for Built-in OTA](#applying-http-authentication-for-built-in-ota)
- [Authentication within the captive portal state](#authentication-within-the-captive-portal-state)

## Applying HTTP authentication

[*AutoConnectConfig::auth*](apiconfig.md#auth) setting allows the Sketch to HTTP authenticate with "[**BASIC**](https://tools.ietf.org/html/rfc2617#section-2)" or "[**DIGEST**](https://tools.ietf.org/html/rfc2617#section-3)" scheme. [*AutoConnectConfig::authScope*](apiconfig.md#authscope) specifies the scope covered by authentication which is the whole range for all pages of the Sketch, custom web pages, or AutoConnect pages. [*AutoConnectConfig::username*](apiconfig.md#username) and [*AutoConnectConfig::password*](apiconfig.md#password) specifies credential as user-id/password pairs.

The Sketch enables HTTP authentication with the [*AutoConnectConfig::auth*](apiconfig.md#auth) setting, also specifies the authentication scheme:

- **AC_AUTH_NONE**  
    AutoConnect pages and custom Web pages do not require authentication. Not protected from all HTTP access. This is the default.
- **AC_AUTH_DIGEST**  
    Protect AutoConnect pages and custom Web pages with DIGEST authentication.
- **AC_AUTH_BASIC**  
    Protect AutoConnect pages and custom Web pages with BASIC authentication.

Note that the authentication scope specified in [*AutoConnectConfig::authScope*](apiconfig.md#authscope) is different from the protection space shown by [**Realm**](https://tools.ietf.org/html/rfc7235#section-2.2) for HTTP authentication. AutoConnect assumes only one Realm and defines it as [**AUTOCONNECT_AUTH_REALM**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L268) in [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h) header file. Instead, the Sketch will be able to expand or narrow the range of authentication by [*AutoConnectConfig::authScope*](apiconfig.md#authscope) setting, which can be either as follows:

- **AC_AUTHSCOPE_PORTAL**  
    Require authentication to access for all AutoConnect pages, including custom Web pages.
- **AC_AUTHSCOPE_AUX**  
    Require authentication to access for all custom Web pages, excepting AutoConnect pages. This is the Default.
- **AC_AUTHSCOPE_PARTIAL**  
    Authenticate only specific custom Web pages which are specified by [*AutoConnectAux::authentication*](apiaux.md#authentication) function or [JSON](acjson.md#auth).

Also, a credential used for authentication is specified in the Sketch using the [*AutoConnectConfig::username*](apiconfig.md#username) and [*AutoConnectConfig::password*](apiconfig.md#password) settings.[^2]

[^2]:The default user name and password for authentication inherits the setting of [AutoConnectConfig::apid](apiconfig.md#apid) and [AutoConnectConfig::psk](apiconfig.md#psk).

Here's a minimal Sketch with HTTP authentication for the custom Web page:

```cpp hl_lines="26 27 28 29"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

static const char PAGE_AUTH[] PROGMEM = R"(
{
  "uri": "/auth",
  "title": "Auth",
  "menu": true,
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "AutoConnect has authorized",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    }
  ]
}
)";

WebServerClass    server;
AutoConnect       portal(server);
AutoConnectConfig config;

void setup() {
  config.auth = AC_AUTH_DIGEST;
  config.authScope = AC_AUTHSCOPE_AUX;
  config.username = "user";
  config.password = "password";
  portal.config(config);
  portal.load(FPSTR(PAGE_AUTH));
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

If you want to authenticate only specific pages in a Sketch that handles multiple custom Web pages, set **AC_AUTHSCOPE_PARTIAL** to [*AutoConnectConfig::authScope*](apiconfig.md#authscope). Then, it specifies the authentication scheme with the [**auth**](acjson.md#auth) key in the JSON description of the page should be authenticated.  

AutoConnect determines which authentication method to use for custom Web pages (such as AutoConnectAux) based on its association with [*AutoConnectConfig::authScope*](apiconfig.md#authscope) setting. The table below shows which authentication scheme will be finally adopted. As shown in this table, the final authentication scheme depends on the [*AutoConnectConfig::authScope*](apiconfig.md#authscope) setting, and if **AC_AUTHSCOPE_PARTIAL** is specified it, [AutoConnectAux's authentication](apiaux.md#authentication) setting takes precedence over the [*AutoConnectConfig::auth*](apiconfig.md#auth) setting.

| AutoConnectConfig::authScope | Authentication scheme for the custom Web page |
|---|---|
| AC_AUTHSCOPE_PORTAL | Specified by AutoConnectConfig::auth |
| AC_AUTHSCOPE_AUX | Specified by AutoConnectConfig::auth |
| AC_AUTHSCOPE_PARTIAL | Specified by [AutoConnectAux::authentication](apiaux.md#authentication), The default values is **AC_AUTH_NONE**. |

Authentication designation for AutoConnectAux can also be specified by giving the following value to the **auth** key by the JSON description:

- "auth" : "basic"
- "auth" : "digest"
- "auth" : "none"

The following example Sketch has two custom Web pages, `Hello` and `Auth`. It applies authentication only to the `Auth` page by setting AC_AUTHSCOPE_PARTIAL to AutoConnectConfig::authScope.

```cpp hl_lines="26 45"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

static const char PAGE_HELLO[] PROGMEM = R"(
{
  "uri": "/hello",
  "title": "Hello",
  "menu": true,
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "Hello, word",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    }
  ]
}
)";

static const char PAGE_AUTH[] PROGMEM = R"(
{
  "uri": "/auth",
  "title": "Auth",
  "menu": true,
  "auth": "digest",
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "AutoConnect has authorized",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    }
  ]
}
)";

WebServerClass    server;
AutoConnect       portal(server);
AutoConnectConfig config;

void setup() {
  // It's a default value but has no meaning in the AC_AUTHSCOPE_PARTIAL setting.
  // config.auth = AC_AUTH_NONE;
  config.authScope = AC_AUTHSCOPE_PARTIAL;
  config.username = "user";
  config.password = "password";
  portal.config(config);
  portal.load(FPSTR(PAGE_HELLO));
  portal.load(FPSTR(PAGE_AUTH));
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

!!! info "PageBuilder v1.4.0 or later needed"
    [PageBuilder](https://github.com/Hieromon/PageBuilder) v1.4.0 or later is required to use HTTP authentication with AutoConnect. Also, v1.4.2 or later is required to eliminate SPIFFS, which is deprecated as a file system for ESP8266 module.

!!! warning "Can not use DIGEST authentication for ESP32 arduino core 1.0.4 stable release"
    For ESP32, Arduino core 1.0.4 stable has a bug for DIGEST authentication. The upstream of the master is recommended. (or use BASIC authentication)

## Applying HTTP authentication for Built-in OTA

[*AutoConnectConfig::auth*](apiconfig.md#auth) setting also affects the [built-in OTA](otaupdate.md) feature. **AC_AUTH_BASIC** or **AC_AUTH_DIGEST** setting allows Built-in OTA to authenticate with the [UPDATE](otabrowser.md#updates-with-the-web-browserupdated-wv115) page. This setting is valid even if [*AutoConnectConfig::authScope*](apiconfig.md#authscope) is **AC_AUTHSCOPE_PARTIAL**. That is if the AutoConnectConfig::auth setting is BASIC or DIGEST, authentication will be required for Built-in OTA. See also [Authentication with AutoconnectOTA](otabrowser.md#authentication-with-autoconnectota).

## Authentication within the captive portal state

When accessing the ESP module from an iOS or Android device in the captive portal state, the HTTP authentication framework is disabled in the OS of the client device. Even if the ESP module responds with a `401 unauthorized` with `WWW-Authenticate`, those client device OSs under the captive portal do not display the login dialog and deprive the user of the opportunity to enter their credentials. There will always be an unauthorized error.

AutoConnect's authentication operation based on HTTP (not HTTPS) depends on the OS of the client device, so in the captive portal state, most devices will unconditionally result in an authentication error. Therefore, the default authentication behavior of AutoConnect does not apply authentication in the captive portal state. (It will be ignored even if the AutoConnect setting is not AC_AUTH_NONE)

However, if you want to deny unauthorized access to the protected page even in the captive portal state, you can use the extension bit of [*AutoConnectConfig::authScope*](apiconfig.md#authScope). The **AC_AUTHSCOPE_WITHCP** flag allows AutoConnect to authentication in the captive portal state. It is set using a logical OR operator for the [*AutoConnectConfig::authScope*](apiconfig.md#authScope) setting and AutoConnect will enable authentication at the captive portal if the **AC_AUTHSCOPE_WITHCP** is ON.

```cpp hl_lines="4"
AutoConnectConfig config;
...
config.auth = AC_AUTH_DIGEST;
config.authScope = AC_AUTHSCOPE_AUX | AC_AUTHSCOPE_WITHCP;
...
```
