## OTA Updates with AutoConnect using an updateserver.py

Since AutoConnect v1.0.0 release provides a new function for updating the sketch firmware of ESP8266 or ESP32 module via OTA assisted with AutoConnectUpdate class. The [AutoConnectUpdate](https://hieromon.github.io/AutoConnect/apiupdate.html) class is an implementation of the binary sketch updater using the HTTP server mentioned in the OTA Updates of the [ESP8266 Arduino Core documentation](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html#ota-updates), which inherits from the ESP8266 HTTPUpdate class (HTTPUpdate class in the case of ESP32). It acts as a client agent for a series of update operations.

<img src="../../mkdocs/images/updatemodel.png" width="540" />

## A simple update server for the AutoConnectUpdate class

The [updateserver.py](https://hieromon.github.io/AutoConnect/otaserver.html#update-server-for-the-autoconnectupdate-class) script is a simple server implemented in Python for OTA updates communicated with the AutoConnectUpdate class and can serve in Python 2 or 3 environment.

### Supported Python environment

* Python 2.7 for [python2/updateserver.py](./python2/updateserver.py)
* Python 3.6 or higher for [python3/updateserver.py](./python3/updateserver.py)

### updateserver.py command line options

```bash
updateserver.py [-h] [--port PORT] [--bind IP_ADDRESS] [--catalog CATALOG] [--log LOG_LEVEL]
```
<dl>
  <dt>--help | -h</dt>
  <dd>Show help message and exit.</dd>
  <dt>--port | -p</dt><dd>Specifies **PORT** number (Default: 8000)</dd>
  <dt>--bind | -b</dt><dd>Specifies the IP address to which the update server binds. Usually, it is the host address of the update server. When multiple NICs configured, specify one of the IP addresses. (Default: HOST IP or 127.0.0.0)</dd>
  <dt>--catalog | -d</dt>
  <dd>Specifies the directory path on the update server that contains the binary sketch files. (Default: The current directory)</dd>
  <dt>--log | -l</dt>
  <dd>Specifies the level of logging output. It accepts the <a href="https://docs.python.org/3/library/logging.html?highlight=logging#logging-levels">Logging Levels</a> specified in the Python logging module.</dd>
</dl>

### Usage updateserver.py

1. Python  
   First, prepare a Python environment. It is also possible with a tiny single-board computer like the [raspberry pi](https://www.raspberrypi.org/). Popular distributions such as Ubuntu for Linux include Python. You can easily set up a Python 2 or 3 environment. If you are using a Mac, you already have the Python 2 environment. macOS is equipped with Python 2.7 by default. In the case of Windows OS, it is necessary to install the Python environment intentionally. Please refer to the [Python official page](https://wiki.python.org/moin/BeginnersGuide/Download) to install Python in your environment.

2. Deploy the binary sketch files  
   Use the Arduino IDE to output a binary file of sketches and deploy it under the update server. The path which specifies for the **--catalog** option of updateServer.py is the path of the binary sketch files you deployed.

3. Start updateserver.py  
   For example, to start the update server on the host with IP address 172.16.1.10 using 8080 port, execute the following command:
   ```bash
   python updateserver.py --port 8080 --bind 172.16.1.10 --catalog bin --log debug
   ```  
   In this example assumes that the binary sketch files are deployed under the path `bin` from the current directory.

Details for the [AutoConnect documentation](https://hieromon.github.io/AutoConnect/otaserver.html).
