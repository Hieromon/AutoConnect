The default behavior of AutoConnect is to launch the captive portal if *1st-WiFi.begin* attempting inside [AutoConnect::begin](api.md#begin) fails. You can change this default behavior through the [AutoConnectConfig](apiconfig.md) settings join together with Sketch code that implements to control the WiFi connection attempting.

- [Captive portal start control](#captive-portal-start-control)
- [Captive portal start detection](#captive-portal-start-detection)
- [Captive portal timeout control](#captive-portal-timeout-control)
- [Disable the captive portal](#disable-the-captive-portal)
- [Launch the captive portal on demand by external trigger](#launch-the-captive-portal-on-demand-by-external-trigger)
- [Launch the captive portal on-demand at losing WiFi](#launch-the-captive-portal-on-demand-at-losing-wifi)
- [Shutdown the captive portal](#shutdown-the-captive-portal)
- [Sketch execution during the captive portal loop](#sketch-execution-during-the-captive-portal-loop)

## Captive portal start control

AutoConnect will launch the captive portal based on the [AutoConnectConfig](apiconfig.md) settings when the WiFi connection status will become to certain conditions. [*AutoConnectConfig::autoRise*](apiconfig.md#autorise) and [*AutoConnectConfig::immediateStart*](apiconfig.md#immediatestart) are concern the conditions to launch the captive portal. Also, the [*AutoConnectConfig::retainPortal*](apiconfig.md#retainportal) controls the continuity of the captive portal state and allows the Sketch to launch the captive portal dynamically even while in a [**handleClient**](api.md#handleclient) loop.

The [**autoRise**](apiconfig.md#autorise) allows or disallows the launch of the captive portal. AutoConnect launches the captive portal only if the autoRise is **true**. If the autoRise is **false**, the captive portal will not start even if the WiFi connection is lost.

Basically, the captive portal initiation is triggered by the result of *1st-WiFi.begin*, but Sketch can control it according to direct the following four actions by configuring AutoConnectConfig with two parameters, the [**immediateStart**](apiconfig.md#immediatestart) and the [**autoRise**](apiconfig.md#autorise).

<table>
  <tr>
    <th rowspan="2" align="center">AutoConnectConfig<br>::immediateStart</th>
    <th colspan="2" align="center">AutoConnectConfig::autoRise</th>
  </tr>
  <tr>
    <td>true</td>
    <td>false</td>
  </tr>
  <tr>
    <td>true</td>
    <td>Skip *1st-WiFi.begin*<br>ESP module becomes SoftAP and the captive portal starts immediately.<br></td>
    <td>Not attempt WiFi connection.<br>Only WebServer will start in STA mode.</td>
  </tr>
  <tr>
    <td>false</td>
    <td>Attempts WiFi connection in STA mode.<br>In some cases, the autoReconnect may restore the connection even if 1st-WiFiBeing fails.<br>If the connection is completely lost, the captive portal will be launched.<br><b>This is the default.</b></td>
    <td>Attempts WiFi connection in STA mode.<br>In some cases, the autoReconnect may restore the connection even if 1st-WiFiBeing fails.<br>ESP module stays in STA mode and WebServer will start.</td>
  </tr>
</table>

The [**retainPortal**](apiconfig.md#retainportal) specifies the continuity of the captive portal after [AutoConnect::begin](api.md#begin), allowing the captive portal would be launched after the Sketch execution has transitioned into the **loop** function. When AutoConnect establishes a WiFi connection while in the captive portal within [AutoConnect::begin](api.md#begin), it stops the DNS server to close the captive portal with SoftAP still running. In this situation, if the Sketch loses the established WiFi connection while executing the loop function, it can reopen the captive portal.

However, this behavior can be redundant depending on the Sketch characteristics. In such a case, you can prevent to starting the captive portal during the `loop()` by [**autoRise**](apiconfig.md#autorise) setting to **false**.

## Captive portal start detection

The captive portal will only be activated if 1st-WiFi::begin fails. Sketch can detect with the [AutoConnect::onDetect](api.md#ondetect) function that the captive portal has started. For example, the Sketch can be written like as follows that turns on the LED at the start captive portal.

```cpp hl_lines="3 13"
AutoConnect Portal;

bool startCP(IPAddress& ip) {
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.println("C.P. started, IP:" + ip.toString());
  return true;
}

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  Portal.onDetect(startCP);
  if (Portal.begin()) {
    digitalWrite(BUILTIN_LED, LOW);
  }
}

void loop() {
  Portal.handleClient();
}
```

## Captive portal timeout control

Once AutoConnect has entered the captive portal state due to the above conditions, the default behavior is that [AutoConnect::begin](api.md#begin) will not exit until a WiFi connection is established. Captive portal timeout control prevents AutoConnect from blocking the Sketch progress. It allows Sketch to abort AutoConnect::begin and returns control to Sketch. 

AutoConnect has two parameters for timeout control. The first is the timeout value used when trying to connect to the specified AP. It works like a typical timeout control for connection attempts with WiFi.begin. This setting is specified by the [*AutoConnectConfig::beginTimeout*](apiconfig.md#begintimeout) or third argument of the [AutoConnect::begin](api.md#begin) function. The default value is the macro defined by [**AUTOCONNECT_TIMEOUT**](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h#L103) in the [`AutoConnectDefs.h`](https://github.com/Hieromon/AutoConnect/blob/master/src/AutoConnectDefs.h) header file.

Another timeout control is for the captive portal itself. It is useful if you want to keep the Sketch offline running even if a WiFi connection is not possible. You can also combine its setting with the [**immediateStart**](#launch-the-captive-portal-on-demand-by-external-trigger) option to create highly mobile sketches. The timeout of the captive portal is specified by the [*AutoConnectConfig::portalTimeout*](apiconfig.md#portaltimeout) as follows.

```cpp hl_lines="9"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

AutoConnect  portal;
AutoConnectConfig  config;

void setup() {
  config.portalTimeout = 60000;  // It will time out in 60 seconds
  portal.config(config);
  portal.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Some sketch code for the connected scene is here.
  }
  else {
    // Some sketch code for not connected scene is here.
  }
  portal.handleClient();
}
```

Also, if you want to stop AutoConnect completely when the captive portal is timed out, you need to call the [AutoConnect::end](api.md#end) function. It looks like the following code:

```cpp
bool acEnable;

void setup() {
  config.portalTimeout = 60000;  // It will time out in 60 seconds
  portal.config(config);
  acEnable = portal.begin();
  if (!acEnable) {
    portal.end();
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Some sketch code for the connected scene is here.
  }
  else {
    // Some sketch code for not connected scene is here.
  }
  if (acEnable) {
    portal.handleClient();
  }
}
```

AutoConnectConfig has another option related to timeouts that you can enable to take advantage of the captive portal feature after a timeout occurrence. The [*AutoConnectConfig::retainPortal*](apiconfig.md#retainportal) option will not shut down SoftAP and the internal DNS server even though [AutoConnect::begin](api.md#begin) has aborted due to a timeout occurrence. Even after the captive portal times out, you can always try to connect to the AP while keeping the Sketch running offline.

The following sample code is the Sketch above with the [**retainPortal**](apiconfig.md#retainportal) setting added. As you can see, the implementation for captive portal continuation does not affect the main logic of the Sketch.

```cpp hl_lines="10"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

AutoConnect  portal;
AutoConnectConfig  config;

void setup() {
  config.portalTimeout = 60000;  // It will time out in 60 seconds
  config.retainPortal = true;
  portal.config(config);
  portal.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Some sketch code for the connected scene is here.
  }
  else {
    // Some sketch code for not connected scene is here.
  }
  portal.handleClient();
}
```

## Disable the captive portal

It can also prevent the captive portal from starting even if the connection at the *1st-WiFi.begin* fails. In this case, [AutoConnect::begin](api.md#begin) behaves same as *WiFi.begin*.

For disabling the captive portal, [**autoRise**](apiconfig.md#autorise) sets to false with [AutoConnectConfig](apiconfig.md).

```cpp hl_lines="4"
AutoConnect       portal;
AutoConnectConfig acConfig;

acConfig.autoRise = false;
portal.config(acConfig);
portal.begin();
```

## Launch the captive portal on demand by external trigger

The [default behavior](lsbegin.md) of [AutoConnect::begin](api.md#begin) gives priority to connect to the least recently established access point. In general, We expect this behavior in most situations, but will intentionally launch the captive portal on some occasion.

Here section describes how to launch on demand the captive portal, and suggests two templates that you can use to implement it.

1. Offline for usual operation, connect to WiFi with an external switch

    You can use this template if the ESP module does not connect to WiFi at an ordinal situation and need to establish by a manual trigger. In this case, it is desirable that AutoConnect not start until an external switch fires. This behavior is similar to the [WiFiManager's startConfigPortal](https://github.com/tzapu/WiFiManager#on-demand-configuration-portal) function.  
    [*AutoConnectConfig::immediateStart*](apiconfig.md#immediatestart) is an option to launch the portal by the SoftAP immediately without attempting *1st-WiFi.begin*. Also, by setting the [*AutoConnectConfig::autoRise*](apiconfig.md#autorise) option to false, it is possible to suppress unintended automatic pop-ups of the portal screen when connecting to an ESP module SSID.  
    To implement this, execute AutoConnect::config within the **setup()** function as usual, and handle AutoConnect::begin inside the **loop()** function.

    ```cpp hl_lines="9"
    #define TRIGGER_PIN 5     // Trigger switch should be LOW active.
    #define HOLD_TIMER  3000

    AutoConnect       Portal;
    AutoConnectConfig Config;

    void setup() {
      pinMode(5, INPUT_PULLUP);
      Config.immediateStart = true;
      // Config.autoRise = false;   // If you don't need to automatically pop-up the portal when connected to the ESP module's SSID.
      Portal.config(Config);
    }

    void loop() {
      if (digitalRead(TRIGGER_PIN) == LOW) {
        unsigned long tm = millis();
        while (digitalRead(TRIGGER_PIN) == LOW) {
          yield();
        }
        // Hold the switch while HOLD_TIMER time to start connect.
        if (millis() - tm > HOLD_TIMER)
          Portal.begin();
      }

      if (WiFi.status() == WL_CONNECTED) {
        // Here, what to do if the module has connected to a WiFi access point
      }

      // Main process of your sketch

      Portal.handleClient();  // If WiFi is not connected, handleClient will do nothing.
    }
    ```

    !!! info "It will not be automatic reconnect"
        The above example does not connect to WiFi until TRIGGER\_PIN goes LOW. When TRIGGER\_PIN goes LOW, the captive portal starts and you can connect to WiFi. Even if you reset the module, it will not automatically reconnect.

2. Register new access points on demand

    The following template is useful for controlling the registration of unknown access points. In this case, the ESP module establishes a WiFi connection using WiFi.begin natively without relying on AutoConnect.  
    Known access point credentials are saved by AutoConnect, to the ESP module can use the saved credentials to handle WiFi.begin natively. This means that you can explicitly register available access points when needed, and the ESP module will not use unknown access points under normal situations.

    ```cpp
    AutoConnect* portal = nullptr;

    bool detectSwitch() {
      /*
      Returns true if an external switch to configure is active.
      */
    }

    bool connectWiFi(const char* ssid, const char* password, unsigned long timeout) {
      WiFi.mode(WIFI_STA);
      delay(100);
      WiFi.begin(ssid, password);
      unsigned long tm = millis();
      while (WiFi.status() != WL_CONNECTED) {
        if (millis() - tm > timeout)
          return false;
      }
      return true;
    }

    void setup() {
      AutoConnectCredential credt;
      station_config_t  config;
      for (int8_t e = 0; e < credt.entries(); e++) {
        credt.load(e, &config);
        if (connectWiFi(config.ssid, config.password, 30000))
          break;
      }
      if (WiFi.status() != WL_CONNECTED) {
        // Here, do something when WiFi cannot reach.
      }
    }

    void loop() {
      if (detectSwitch()) {
        AutoConnectConfig config;
        config.immediateStart= true;
        if (!portal) {
          portal = new AutoConnect;
        }
        portal->config(config);
        if (portal->begin()) {
          portal->end();
          delete portal;
          portal = nullptr;
        }
      }
      // Here, ordinary sketch logic.
    }
    ```

## Launch the captive portal on-demand at losing WiFi

If the ESP module loses the established WiFi connection during the loop of [**handleClient**](api.md#handleclient), you can prevent the ESP module from going absolutely standalone by launching the captive portal on demand.

When [**retainPortal**](apiconfig.md#retainportal) and [**autoRise**](apiconfig.md#autorise) settings are enabled, AutoConnect will launch SoftAP and start DNS when it detects a WiFi disconnect with the router during a handleClient loop. This behavior will occur caused by a WiFi disconnect detection even if the WiFi mode is STA.

**Since AutoConnect v1.2.0**, An improved [**retainPortal**](apiconfig.md#retainportal) option allows the captive portal to be restarted during a handleClient loop even if it is closed once in [AutoConnect::begin](api.md#begin). In this case, the Sketch execution stage has already transitioned into the loop function, so the Sketch process seems running concurrently with the captive portal loop. Its captive portal launched from inside handleClient does not block the execution of the Sketch, unlike that launched from [AutoConnect::begin](api.md#begin).

```cpp hl_lines="5 6"
AutoConnect       Portal;
AutoConnectConfig Config;

void setup() {
  Config.autoRise = true; // It's the default, no setting is needed explicitly.
  Config.retainPortal = true;
  Portal.config(Config);
  Portal.begin();
}

void loop() {
  Portal.handleClient();
}
```

!!! note "Need autoRise enabled"
    Need [*AutoConnectConfig::autoRise*](apiconfig.md#autorise) setting enabled to start the captive portal on demand during a handleClient loop.

Although the Sketch above specifies the [**retainPortal**](apiconfig.md#retainportal), it does not instruct starts the captive portal always. AutoConnect will try WiFi.begin once in [AutoConnect::begin](api.md#begin) unless the [**immediateStart**](apiconfig.md#immediatestart) option is specified. If AutoConnect fails the *1st-WiFi.begin*, the captive portal will be launched at that point and the Sketch execution will stay within the [AutoConnect::begin](api.md#begin) function.

There is also a way to avoid starting the captive portal inside [AutoConnect::begin](api.md#begin) and start the captive portal according to the WiFi connection status after the Sketch execution transitions to a **handleClient loop**. Adjusting the timing of [**autoRise**](apiconfig.md#autorise) activation will allow the captive portal to start only from inside [AutoConnect::handleClient](api.md#handleclient) function.

```cpp hl_lines="5 6 9"
AutoConnect       Portal;
AutoConnectConfig Config;

void setup() {
  Config.retainPortal = true;
  Config.autoRise = false;  // Suppresses the launch of the captive portal from AutoConnect::begin.
  Portal.config(Config);    // Don't forget it.
  Portal.begin();
  Config.autoRise = true;   // Enable the launch of the captive portal.
  Portal.config(Config);    // Don't forget it.
}

void loop() {
  Portal.handleClient();
}
```

The [*retainPortal*](apiconfig.md#retainportal) option will keep SoftAP even if WiFi has established a connection as a client with the router.  Since it has the effect of stopping the DNS server, the phenomenon that the portal screen will not pop up automatically even though SoftAP is in action occur.<br>
This is a legacy behavior to ensure backward compatibility with up to AutoConnect v1.1.7. To stop SoftAP on escape from the on-demand captive portal, you need to explicitly call **[`WiFi.softAPdisconnect(true)`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html#softapdisconnect)** and **[`WiFi.enableAP(false)`](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html?highlight=enableAP#other-function-calls)** in the Sketch.

```cpp hl_lines="25 26 27 28 29 30"
AutoConnect       Portal;
AutoConnectConfig Config;

bool  Connected;
unsigned long Elapsed;

void onConnect(IPAddress& clientIP) {
  Connected = true;
  Elapsed = millis();
}

void setup() {
  Config.retainPortal = true;
  Portal.config(Config);
  Portal.onConnect(onConnect);
  Portal.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connected = false;
    Elapsed = millis();
  }

  if ((WiFi.getMode() & WIFI_AP) && Connected) {
    if (millis() - Elapsed > 30000) {
      WiFi.softAPdisconnect(true);
      WiFi.enableAP(false);
    }
  }

  // Actual sketch process is here.

  Portal.handleClient();
}
```

The above sketch will shutdown the SoftAP after elapsed time exceeds 30 seconds since the connection was re-established. Its logic is a bit tricky and does not stop SoftAP immediately after the connection established, which has several seconds delay. Doing it ensures that AutoConnect can send the HTML response.

!!!info "Stopped SoftAP is still displayed"
    After SoftAP stopped, there is a time lag before it disappears from the detected access points list on the client device.

## Shutdown the captive portal

There is some complexity in the conditions under which AutoConnect shuts down the captive portal. Making a sketch that activates SoftAP only when needed can seem tedious.  
But there is a reason why. Even if AutoConnect could establish a connection using a captive portal, your cell phone as a client device would still have to keep connected to the ESP module-generated SoftAP in order to send the page for notifying the connection successful to a user. At that point, your client device that opened the captive portal still needs a connection with SoftAP.

What happens, after all, is as follows:

1. You made a connection to the access point such as WiFi router using the captive portal and took a successful page.
2. Your sketch will rush into the loop function and starts to works well, hooray!
3. Oops. Don't celebrate yet. I can see SoftAP ID on my cell phone. But the AutoConnect page never pops up automatically. Why?

Because, for the above reasons, we can not promptly shut down the SoftAP. (However, DNS will stop)

So, If you want to stop SoftAP after connecting to the access point using the captive portal, you need to implement the shutdown process with Sketch explicitly. A template of the basic sketch that can stop the SoftAP after the connection is the following:

```cpp hl_lines="5 6 7 8"
AutoConnect Portal;

void setup() {
  if (Portal.begin()) {
    if (WiFi.getMode() & WIFI_AP) {
      WiFi.softAPdisconnect(true);
      WiFi.enableAP(false);
    }
  }
}

void loop() {
  Portal.handleClient();
}
```

!!! note "If you stop SoftAP, the connection will be lost"
    If you stop SoftAP immediately after the [AutoConnect::begin](api.md#begin) successful, will part with the connection and cannot see the result notifying on your client device.  
    You can expect to receive result notifications if you run [**handleClient**](api.md#handleclient) before stopping SoftAP. (although you may not always succeed; it will not work if the WiFi radio signal is weak)

## Sketch execution during the captive portal loop

With [AutoConnect::begin](api.md#begin), once the captive portal is started without being able to connect to a known WiFi access point, control will not return to sketch until the WiFi connection is established or times out. This behavior helps to pin the ESP module's network participation as a WiFi client (that is, AutoConnect::begin is an alternative to WiFi.begin) but it can not rush into the loop function of the Sketch. Therefore, while the ESP module is in the captive portal state and waiting for a connection operation to the access point, the behavior of the Sketch will be restrained by the [escape conditions from AutoConnect::begin](lsbegin.md).

The **whileCaptivePortal exit** allows the Sketch to continue the process temporarily while the ESP module remains standalone and the captive portal is open. [AutConnect::whileCaptivePortal](api.md#whilecaptiveportal) function registers the user's sketch function to be called by [AutoConnect::begin](api.md#begin) or [AutoConnect::handleClient](api.md#handleclient) during the execution of the captive portal session loop.

The whileCaptivePortal exit can be registered by following: 

```cpp hl_lines="13"
AutoConnect portal;

bool whileCP(void) {
  bool  rc;
  // Here, something to process while the captive portal is open.
  // To escape from the captive portal loop, this exit function returns false.
  // rc = true;, or rc = false;
  return rc;
}

void setup() {
  ...
  portal.whileCaptivePortal(whileCP);
  portal.begin();
  ...
}
```

AutoConnect will open the captive portal in the [AutoConnect::begin](api.md#bdgin) and [AutoConnect::handleClient](api.md#handleclient) scenes, but the whileCaptive portal exit will be called repeatedly from [AutoConnect::begin](api.md#begin) until exits from it. The whileCaptivePortal exit will be called repeatedly while the captive portal is open until WiFi connected or times out occurs. In the Sketch, returning a **FALSE** value from the whileCaptivePortal function allows the control to escape from the captive portal loop even before the session elapsed time exceeds the limits.
