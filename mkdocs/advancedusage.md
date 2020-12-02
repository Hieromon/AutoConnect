## Summary

To make sketches work as you intended with AutoConnect, make sure you understand the implications of the setting parameters and configure AutoConnect. [AutoConnectConfig](apiconfig.md) allows you to incorporate settings into AutoConnect that coordinate control over WiFi connectivity and captive portal behavior.  
For advanced usages, the configuration settings and the Sketch examples are followings:

- [AutoConnect WiFi connection control](adconnection.md)
- [Captive portal control](adcpcontrol.md)
- [Authentication settings](adauthentication.md)
- [Credential accesses](adcredential.md)
- [Settings for customizing the page exterior](adexterior.md)
- [Settings and controls for network and WiFi](adnetwork.md)
- [Other operation settings and controls](adothers.md)

!!! note "Don't forget [**AutoConnect::config**](api.md#config)"
    The configuration cannot be reflected by only changing the member variables of [AutoConnectConfig](apiconfig.md) settings. It will be reflected in the actual ones by [AutoConnect::config](api.md#config) function. Don't forget to run the [AutoConnect::config](api.md#config) after changing the AutoConnectConfig member variables.
