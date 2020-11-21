## Summary

In order for your sketches combined with AutoConnect to work as you intended, make them based on the right understanding of configuration settings and their effectiveness. AutoConnect includes the configuration settings using [AutoConnectConfig](apiconfig.md) class that adjusts controlling the behaviors for WiFi connection and the captive portal.  
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
