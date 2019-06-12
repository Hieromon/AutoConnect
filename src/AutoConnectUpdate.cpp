/**
 * AutoConnectUpdate class implementation.
 * @file   AutoConnectUpdate.cpp
 * @author hieromon@gmail.com
 * @version    0.9.11
 * @date   2019-05-14
 * @copyright  MIT license.
 */

#include "AutoConnectDefs.h"

#ifdef AUTOCONNECT_USE_UPDATE

#include <functional>
#include <type_traits>
#include "AutoConnectUpdate.h"
#include "AutoConnectUpdatePage.h"
#include "AutoConnectJsonDefs.h"

/**
 * The AutoConnectUpdateAct class inherits from the HTTPupdate class. The
 * update server corresponding to this class needs a simple script
 * based on an HTTP server. It is somewhat different from the advanced
 * updater script offered by Arduino core of ESP8266.
 * The equipment required for the update server for the
 * AutoConnectUpdateAct class is as follows:
 *
 * The catalog script:
 *   The update server URI /catalog is a script process that responds to
 *   queries from the AutoConnectUpdateAct class. The catalog script accepts
 *   the queries such as '/catalog?op=list&path='.
 *   - op:
 *     An op parameter speicies the query operation. In the current
 *     version, available query operation is a list only.
 *   - list:
 *     The query operation list responds with a list of available sketch
 *     binaries. The response of list is a directory list of file paths
 *     on the server specified by the path parameter and describes as a
 *     JSON document. Its JSON document is an array of JSON objects with
 *     the name and the type keys. For example, it is the following
 *     description:
 *     [
 *       {
 *         "name": "somefoler",
 *         "type": "directory"
 *       },
 *       {
 *         "name": "update.ino",
 *         "type": "file"
 *       },
 *       {
 *         "name": "update.bin",
 *         "type": "bin"
 *       }
 *     ]
 *     - name:
 *       Name of file entry on the path.
 *     - type:
 *       The type of the file. It defines either directory, file or bin.
 *       The bin means that the file is a sketch binary, and the
 *       AutoConnectUpdateAct class recognizes only files type bin as
 *       available update files.
 *   - path:
 *     A path parameter specifies the path on the server storing
 *     available sketch binaries.
 *
 * Access to the path on the server:
 *   It should have access to the bin file. The update server needs to
 *   send a bin file with a content type of application/octet-stream via
 *   HTTP and also needs to attach an MD5 hash value to the x-MD5 header.
 */

/**
 * The following two templates absorb callbacks that are enabled/disabled
 * by the Update library version in the core.
 * The old UpdateClass of the ESP8266/ESP32 arduino core does not have
 * the onProgress function for registering callbacks. These templates
 * avoid the onProgress calls on older library versions.
 * In versions where the update function callback is disabled, the
 * dialog on the client browser does not show the progress of the update.
 */
#if defined(ARDUINO_ARCH_ESP8266)
using UpdateVariedClass = UpdaterClass;
#elif defined(ARDUINO_ARCH_ESP32)
using UpdateVariedClass = UpdateClass;
#endif

namespace AutoConnectUtil {
AC_HAS_FUNC(onProgress);

template<typename T>
typename std::enable_if<AutoConnectUtil::has_func_onProgress<T>::value, AutoConnectUpdateAct::AC_UPDATEDIALOG_t>::type onProgress(T& updater, std::function<void(size_t, size_t)> fn) {
  updater.onProgress(fn);
  AC_DBG("Updater keeps callback\n");
  return AutoConnectUpdateAct::UPDATEDIALOG_METER;
}

template<typename T>
typename std::enable_if<!AutoConnectUtil::has_func_onProgress<T>::value, AutoConnectUpdateAct::AC_UPDATEDIALOG_t>::type onProgress(T& updater, std::function<void(size_t, size_t)> fn) {
  (void)(updater);
  (void)(fn);
  return AutoConnectUpdateAct::UPDATEDIALOG_LOADER;
}
}

/**
 * A destructor. Release the update processing dialogue page generated
 * as AutoConnectAux.
 */
AutoConnectUpdateAct::~AutoConnectUpdateAct() {
  _catalog.reset(nullptr);
  _progress.reset(nullptr);
  _result.reset(nullptr);
  _WiFiClient.reset(nullptr);
  _ws.reset(nullptr);
}

/**
 * Attach the AutoConnectUpdateAct to the AutoConnect which constitutes
 * the bedrock of the update process. This function creates dialog pages
 * for update operation as an instance of AutoConnectAux and joins to
 * the AutoConnect which is the bedrock of the process.
 * @param  portal   A reference of AutoConnect
 */
void AutoConnectUpdateAct::attach(AutoConnect& portal) {
  AutoConnectAux* updatePage;

  updatePage = new AutoConnectAux(String(FPSTR(_auxCatalog.uri)), String(FPSTR(_auxCatalog.title)), _auxCatalog.menu);
  _buildAux(updatePage, &_auxCatalog, lengthOf(_elmCatalog));
  _catalog.reset(updatePage);

  updatePage = new AutoConnectAux(String(FPSTR(_auxProgress.uri)), String(FPSTR(_auxProgress.title)), _auxProgress.menu);
  _buildAux(updatePage, &_auxProgress, lengthOf(_elmProgress));
  _progress.reset(updatePage);

  updatePage = new AutoConnectAux(String(FPSTR(_auxResult.uri)), String(FPSTR(_auxResult.title)), _auxResult.menu);
  _buildAux(updatePage, &_auxResult, lengthOf(_elmResult));
  _result.reset(updatePage);
  _result->chunk = PB_ByteStream;
  _catalog->on(std::bind(&AutoConnectUpdateAct::_onCatalog, this, std::placeholders::_1, std::placeholders::_2), AC_EXIT_AHEAD);
  _progress->on(std::bind(&AutoConnectUpdateAct::_onUpdate, this, std::placeholders::_1, std::placeholders::_2), AC_EXIT_AHEAD);
  _result->on(std::bind(&AutoConnectUpdateAct::_onResult, this, std::placeholders::_1, std::placeholders::_2), AC_EXIT_AHEAD);

  portal.join(*_catalog.get());
  portal.join(*_progress.get());
  portal.join(*_result.get());

  _status = UPDATE_IDLE;

  // Attach this to the AutoConnectUpdateAct
  portal._update.reset(this);
  AC_DBG("AutoConnectUpdate attached\n");
  if (WiFi.status() == WL_CONNECTED)
    enable();

  // Register the callback to inform the update progress
  _dialog = AutoConnectUtil::onProgress<UpdateVariedClass>(Update, std::bind(&AutoConnectUpdateAct::_inProgress, this, std::placeholders::_1, std::placeholders::_2));
  // Adjust the client dialog pattern according to the callback validity
  // of the UpdateClass.
  AutoConnectElement* loader = _progress->getElement(String(F("loader")));
  AutoConnectElement* enable_loader = _progress->getElement(String(F("enable_loader")));
  AutoConnectElement* enable_loader_post = _progress->getElement(String(F("enable_loader_post")));
  AutoConnectElement* progress_meter = _progress->getElement(String(F("progress_meter")));
  AutoConnectElement* inprogress_meter = _progress->getElement(String(F("inprogress_meter")));
  AutoConnectElement* progress_loader = _progress->getElement(String(F("progress_loader")));
  AutoConnectElement* inprogress_loader = _progress->getElement(String(F("inprogress_loader")));
  switch (_dialog) {
  case UPDATEDIALOG_LOADER:
    progress_meter->enable =false;
    inprogress_meter->enable = false;
    break;
  case UPDATEDIALOG_METER:
    loader->enable = false;
    enable_loader->enable =false;
    enable_loader_post->enable =false;
    progress_loader->enable =false;
    inprogress_loader->enable = false;
    break;
  }
}

/**
 * Detach the update item from the current AutoConnect menu.
 * AutoConnectUpdateAct still active.
 */
void AutoConnectUpdateAct::disable(void) {
  if (_catalog) {
    _catalog->menu(false);
    if (_WiFiClient)
      _WiFiClient.reset(nullptr);
    AC_DBG("AutoConnectUpdate disabled\n");
  }
}

/**
 * Make AutoConnectUpdateAct class available by incorporating the update
 * function into the menu.
 */
void AutoConnectUpdateAct::enable(void) {
  if (_catalog) {
    _catalog->menu(true);
    _status = UPDATE_IDLE;
    _period = 0;
    AC_DBG("AutoConnectUpdate enabled\n");
  }
}

void AutoConnectUpdateAct::handleUpdate(void) {
  // Purge WiFiClient instances that have exceeded their retention
  // period to avoid running out of memory.
  if (_WiFiClient) {
    if (millis() - _period > AUTOCONNECT_UPDATE_DURATION)
      if (_status != UPDATE_START) {
        _WiFiClient.reset(nullptr);
        AC_DBG("Purged WifiClient due to duration expired.\n");
      }
  }

  if (isEnable()) {
    if (WiFi.status() == WL_CONNECTED) {
      // Evaluate the processing status of AutoConnectUpdateAct and
      // execute it accordingly. It is only this process point that
      // requests update processing.
      if (_status == UPDATE_START) {
        unsigned long tm = millis();
        while (!_wsConnected) {
          if (millis() - tm > AUTOCONNECT_TIMEOUT) {
            AC_DBG("WebSocket client connection timeout, update ignored\n");
            break;
          }
          _ws->loop();  // Crawl the connection request.
        }
        // Launch the update
        if (_wsConnected)
          update();
        else
          _status = UPDATE_IDLE;
      }
      else if (_status == UPDATE_RESET) {
        AC_DBG("Restart on %s updated...\n", _binName.c_str());
        ESP.restart();
      }
    }
    // If WiFi is not connected, disables the update menu.
    // However, the AutoConnectUpdateAct class stills active.
    else
      disable();
  }
}

/**
 * Run the update function of HTTPUpdate that is the base class and
 * fetch the result.
 * @return  AC_UPDATESTATUS_t
 */
AC_UPDATESTATUS_t AutoConnectUpdateAct::update(void) {
  // Start update
  String  uriBin = uri + '/' + _binName;
  if (_binName.length()) {
    AC_DBG("%s:%d/%s update in progress...", host.c_str(), port, uriBin.c_str());
    if (!_WiFiClient) {
      _WiFiClient.reset(new WiFiClient);
      _period = millis();
    }
    t_httpUpdate_return ret = HTTPUpdateClass::update(*_WiFiClient, host, port, uriBin);
    switch (ret) {
    case HTTP_UPDATE_FAILED:
      _status = UPDATE_FAIL;
      AC_DBG_DUMB(" %s\n", getLastErrorString().c_str());
      AC_DBG("update returns HTTP_UPDATE_FAILED\n");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      _status = UPDATE_IDLE;
      AC_DBG_DUMB(" No available update\n");
      break;
    case HTTP_UPDATE_OK:
      _status = UPDATE_SUCCESS;
      AC_DBG_DUMB(" completed\n");
      break;
    }
    _WiFiClient.reset(nullptr);
    // Request the client to close the WebSocket.
    _ws->sendTXT(_wsClient, "#e");
  }
  else {
    AC_DBG("An update has not specified");
    _status = UPDATE_NOAVAIL;
  }
  return _status;
}

/**
 * Create the update operation pages using a predefined page structure
 * with two structures as ACPage_t and ACElementProp_t which describe
 * for AutoConnectAux configuration.
 * This function receives instantiated AutoConnectAux, instantiates
 * defined AutoConnectElements by ACPage_t, and configures it into
 * received AutoConnectAux.
 * @param  aux        An instantiated AutoConnectAux that will configure according to ACPage_t.
 * @param  page       Pre-defined ACPage_t
 * @param  elementNum Number of AutoConnectElements to configure.  
 */
void AutoConnectUpdateAct::_buildAux(AutoConnectAux* aux, const AutoConnectUpdateAct::ACPage_t* page, const size_t elementNum) {
  for (size_t n = 0; n < elementNum; n++) {
    if (page->element[n].type == AC_Element) {
      AutoConnectElement* element = new AutoConnectElement;
      element->name = String(FPSTR(page->element[n].name));
      if (page->element[n].value)
        element->value = String(FPSTR(page->element[n].value));
      aux->add(reinterpret_cast<AutoConnectElement&>(*element));
    }
    else if (page->element[n].type == AC_Radio) {
      AutoConnectRadio* element = new AutoConnectRadio;
      element->name = String(FPSTR(page->element[n].name));
      aux->add(reinterpret_cast<AutoConnectElement&>(*element));
    }
    else if (page->element[n].type == AC_Submit) {
      AutoConnectSubmit* element = new AutoConnectSubmit;
      element->name = String(FPSTR(page->element[n].name));
      if (page->element[n].value)
        element->value = String(FPSTR(page->element[n].value));
      if (page->element[n].peculiar)
        element->uri = String(FPSTR(page->element[n].peculiar));
      aux->add(reinterpret_cast<AutoConnectElement&>(*element));
    }
    else if (page->element[n].type == AC_Text) {
      AutoConnectText* element = new AutoConnectText;
      element->name = String(FPSTR(page->element[n].name));
      if (page->element[n].value)
        element->value = String(FPSTR(page->element[n].value));
      if (page->element[n].peculiar)
        element->format = String(FPSTR(page->element[n].peculiar));
      aux->add(reinterpret_cast<AutoConnectText&>(*element));
    }
  }
}

/**
 * AUTOCONNECT_URI_UPDATE page handler.
 * It queries the update server for cataloged sketch binary and
 * displays the result on the page as an available updater list.
 * The update execution button held by this page will be enabled only
 * when there are any available updaters.
 * @param  catalog A reference of the AutoConnectAux as AUTOCONNECT_URI_UPDATE
 * @param  args    A reference of the PageArgument of the PageBuilder
 * @return         Additional string to the page but it always null.
 */
String AutoConnectUpdateAct::_onCatalog(AutoConnectAux& catalog, PageArgument& args) {
  AC_UNUSED(args);
  HTTPClient httpClient;

  // Reallocate WiFiClient if it is not existence.
  if (!_WiFiClient) {
    _WiFiClient.reset(new WiFiClient);
    _period = millis();
  }

  AutoConnectText&  caption = catalog.getElement<AutoConnectText>(String(F("caption")));
  AutoConnectRadio& firmwares = catalog.getElement<AutoConnectRadio>(String(F("firmwares")));
  AutoConnectSubmit&  submit = catalog.getElement<AutoConnectSubmit>(String(F("update")));
  firmwares.empty();
  firmwares.tags.clear();
  submit.enable = false;
  _binName = String("");

  String  qs = String(F(AUTOCONNECT_UPDATE_CATALOG)) + '?' + String(F("op=list&path=")) + uri;
  AC_DBG("Query %s:%d%s\n", host.c_str(), port, qs.c_str());

  // Throw a query to the update server and parse the response JSON
  // document. After that, display the bin type file name contained in
  // its JSON document as available updaters to the page.
  if (httpClient.begin(*_WiFiClient.get(), host, port, qs, false)) {
    int responseCode = httpClient.GET();
    if (responseCode == HTTP_CODE_OK) {

      JsonVariant jb;
      bool  parse;
      char  beginOfList[] = "[";
      char  endOfEntry[] = ",";
      char  endOfList[] = "]";
      Stream& responseBody = httpClient.getStream();

      // Read partially and repeatedly the responded http stream that is
      // including the JSON array to reduce the buffer size for parsing
      // of the firmware catalog list.
      AC_DBG("Update server responded:");
      responseBody.find(beginOfList);
      do {
        // The size of the JSON buffer is a fixed. It can be a problem
        // when parsing with ArduinoJson V6. If memory insufficient has
        // occurred during the parsing, increase this buffer size.
        ArduinoJsonStaticBuffer<AUTOCONNECT_UPDATE_CATALOG_JSONBUFFER_SIZE> jb;

#if ARDUINOJSON_VERSION_MAJOR<=5
        ArduinoJsonObject json = jb.parseObject(responseBody);
        parse = jb.success();
#else
        DeserializationError err = deserializeJson(jb, responseBody);
        ArduinoJsonObject json = jb.as<JsonObject>();
        parse = (err == DeserializationError::Ok);
#endif
        if (parse) {
#ifdef AC_DEBUG
          AC_DBG_DUMB("\n");
          ARDUINOJSON_PRINT(jb, AC_DEBUG_PORT);
#endif
          // Register only bin type file name as available sketch binary to
          // AutoConnectRadio value based on the response from the update server.
          firmwares.order = AC_Horizontal;
          if (json["type"].as<String>().equalsIgnoreCase("bin")) {
            firmwares.add(json[F("name")].as<String>());
            String  attr = String(F("<span>")) + json[F("date")].as<String>() + String(F("</span><span>")) + json[F("time")].as<String>().substring(0, 5) + String(F("</span><span>")) + String(json[F("size")].as<int>()) + String(F("</span>"));
            firmwares.tags.push_back(attr);
          }
        }
        else {
          caption.value = String(F("Invalid catalog list:")) + String(err.c_str());
          AC_DBG("JSON:%s\n", err.c_str());
          break;
        }
      } while (responseBody.findUntil(endOfEntry, endOfList));

      AC_DBG_DUMB("\n");
      if (parse) {
        if (firmwares.size()) {
          caption.value = String(F("<h4>Available firmwares</h4>"));
          submit.enable = true;
        }
        else
          caption.value = String(F("<h4>No available firmwares</h4>"));
      }
    }
    else {
      caption.value = String(F("Update server responds (")) + String(responseCode) + String("):");
      caption.value += HTTPClient::errorToString(responseCode);
      AC_DBG("%s\n", caption.value.c_str());
    }
    httpClient.end();
  }
  else {
    caption.value = String(F("http failed connect to ")) + host + String(':') + String(port);
    AC_DBG("%s\n", caption.value.c_str());
  }

  return String("");
}

/**
 * AUTOCONNECT_URI_UPDATE_ACT page handler
 * Only display a dialog indicating that the update is in progress.
 * @param  progress A reference of the AutoConnectAux as AUTOCONNECT_URI_UPDATE_ACT
 * @param  args     A reference of the PageArgument of the PageBuilder
 * @return          Additional string to the page but it always null.
 */
String AutoConnectUpdateAct::_onUpdate(AutoConnectAux& progress, PageArgument& args) {
  AC_UNUSED(args);
  // launch the WebSocket server
  _ws.reset(new WebSocketsServer(AUTOCONNECT_WEBSOCKETPORT));
  if (_ws) {
    _ws->begin();
    _ws->onEvent(std::bind(&AutoConnectUpdateAct::_wsEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
  }
  else
    AC_DBG("WebSocketsServer allocation failed\n");

  // Constructs the dialog page.
  AutoConnectElement* binName = progress.getElement(String(F("binname")));
  _binName = _catalog->getElement<AutoConnectRadio>(String(F("firmwares"))).value();
  binName->value = _binName;
  AutoConnectElement* url = progress.getElement(String("url"));
  url->value = host + ':' + port;
  AutoConnectElement* wsurl = progress.getElement(String(F("wsurl")));
  wsurl->value = "ws://" + WiFi.localIP().toString() + ':' + AUTOCONNECT_WEBSOCKETPORT;
  AC_DBG("Cast WS %s\n", wsurl->value.c_str());
  _wsConnected = false;
  _status = UPDATE_START;
  return String("");
}

/**
 * AUTOCONNECT_URI_UPDATE_RESULT page handler
 * Display the result of the update function of HTTPUpdate class.
 * @param  result A reference of the AutoConnectAux as AUTOCONNECT_URI_UPDATE_RESULT
 * @param  args   A reference of the PageArgument of the PageBuilder
 * @return        Additional string to the page but it always null.
 */
String AutoConnectUpdateAct::_onResult(AutoConnectAux& result, PageArgument& args) {
  AC_UNUSED(args);
  String  resForm;
  String  resColor;
  bool    restart = false;

  if (_ws) {
    _ws->close();
    while (_wsConnected)
      _ws->loop();
    _ws.reset(nullptr);
  }

  switch (_status) {
  case UPDATE_SUCCESS:
    resForm = String(F(" sucessfully updated. restart..."));
    resColor = String(F("blue"));
    restart = true;
    break;
  case UPDATE_FAIL:
    resForm = String(F(" failed."));
    resForm += String(F("<br>")) + getLastErrorString();
    resColor = String(F("red"));
    break;
  default:
    resForm = String(F("<br>No available update."));
    resColor = String(F("red"));
    break;
  }
  AutoConnectText& resultElm = result.getElement<AutoConnectText>(String(F("status")));
  resultElm.value = _binName + resForm;
  resultElm.style = String(F("font-size:120%;color:")) + resColor;
  result.getElement<AutoConnectElement>(String(F("restart"))).enable = restart;
  if (restart)
    _status = UPDATE_RESET;
  return String("");
}

void AutoConnectUpdateAct::_inProgress(size_t amount, size_t size) {
  if (_ws) {
    _amount = amount;
    _binSize = size;
    String  payload = "#p," + String(_amount) + ':' + String(_binSize); 
    _ws->sendTXT(_wsClient, payload);
  }
}

void AutoConnectUpdateAct::_wsEvent(uint8_t client, WStype_t event, uint8_t* payload, size_t length) {
  AC_DBG("WS client:%d event(%d)\n", client, event);
  if (event == WStype_CONNECTED) {
    _wsConnected = true;
    _wsClient = client;
    _ws->sendTXT(_wsClient, "#s");
  }
  else if (event == WStype_DISCONNECTED)
    _wsConnected = false;
}

#endif // !AUTOCONNECT_USE_UPDATE
