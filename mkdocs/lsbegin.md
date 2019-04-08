## AutoConnect::begin logic sequence

Several parameters as follows of [AutoConnectConfig](apiconfig.md) affect the behavior of [AutoConnect::begin](api.md#begin) function. Each parameter affects the behaves in interacted order with the priority and apply to the logic sequence of [AutoConnect::begin](api.md#begin).

- [immediateStart](apiconfig.md#immediatestart) : The captive portal start immediately, without first WiFi.begin.
- [autoReconenct](apiconfig.md#autoreconnect) : Attempt re-connect with past SSID by saved credential.
- [portalTimeout](apiconfig.md#portaltimeout) : Time out limit for the portal.
- [retainPortal](apiconfig.md#retainportal) : Keep DNS server functioning for the captive portal.

You can use these parameters in combination with sketch requirements and need to understand correctly the behavior caused by the parameters. The following chart shows the AutoConnect::begin logic sequence including the effect of these parameters.

<img align="center" src="images/process_begin.svg">

For example, [AutoConnect::begin](api.md#begin) will not exits without the [**portalTimeout**](apiconfig.md#portaltimeout) while the connection not establishes, but WebServer will start to work. A DNS server that detects the probe of the captive portal is also effective. So, your sketch may work seemingly, but it will close with inside a loop of the [AutoConnect::begin](api.md#begin) function. Especially when invoking [AutoConnect::begin](api.md#begin) in the **setup()**, execution control does not pass to the **loop()**.

As different scenes, you may use the [**immediateStart**](apiconfig.md#immediatestart) effectively. Equipped the external switch to activate the captive portal with the ESP module, combined with the [**portalTime**](apiconfig.md#portaltimeout) and the [**retainPortal**](apiconfig.md#retainportal) it will become WiFi active connection feature. You can start [AutoConnect::begin](api.md#begin) at any point in the **loop()**, which allows your sketch can behave both the offline mode and the online mode.

The [**retainPortal**](apiconfig.md#retainportal) option allows the DNS server to continue operation after exiting from [AutoConnect::begin](api.md#begin). AutoConnect traps captive portal detection from the client and redirects it to the AutoConnect menu. That trap will answer all unresolved addresses with SoftAP's IP address. If the URI handler for the source request is undefined, it returns a 302 response with `SoftAPIP/_ac` to the client. This is the mechanism of AutoConnect's captive portal. Captive portal probes will frequently occur while you are attempting on the client device's WiFi connection Apps and these implementations are varied each OS, so it not realistic to identify all probing URIs. Therefore, while retainPortal is enabled, it is not preferable to executing the sketch under the WiFi connection Apps on the client device. (Probably not work correctly) You need to exit from the WiFi connection Apps once.

Please consider these kinds of influence when you make sketches.

!!! info "The AutoConnect::begin 3rd parameter"
    Another parameter as the [3rd parameter](api.md#begin) of AutoConnect::begin related to timeout constrains the connection wait time after WiFi.begin. It is the **CONNECTED** judgment of the above chart that it has an effect.
