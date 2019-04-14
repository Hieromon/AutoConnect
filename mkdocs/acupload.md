## Uploading file from Web Browser

If you have to write some data individually to the ESP8266/ESP32 module for the sketch behavior, the [AutoConnectFile](acelements.md#autoconnectfile) element will assist with your wants implementation. The AutoConnectFile element produces an HTML `<input type="file">` tag and can save uploaded file to the flash or external SD of the ESP8266/ESP32 module. The handler for saving is built into AutoConnect. You can use it to inject any sketch data such as the initial values for the custom Web page into the ESP module via OTA without using the sketch data upload tool of Arduino-IDE.
<p style="display:block;margin-left:auto;margin-right:auto;width:603px;height:368px;border:1px solid lightgray;"><img data-gifffer="images/upload.gif" data-gifffer-width="601" data-gifffer-height="366""/></p>

## Basic steps of the file upload sketch

Here is the basic procedure of the sketch which can upload files from the client browser using AutoConnectFile:[^1]

[^1]:The AutoConnectFile element can be used with other AutoConnectElements on the same page.

1. Place AutoConnectFile on a custom Web page by writing JSON or constructor code directly with the sketch.
2. Place other AutoConnectElements as needed.
3. Place AutoConnectSubmit on the same custom Web page.
4. Perform the following process in the on-handler of submitting destination:
    - Retrieve the [AutoConnectFile instance](apielements.md#autoconnectfile) from the custom Web page where you placed the AutoConnectFile element using the [AutoConnectAux::getElement](apiaux.md#getelement) function or the [operator \[\]](apiaux.md#operator).
    - Start access to the device specified as the upload destination. In usually, it depends on the file system's begin function. For example, if you specified Flash's SPIFFS as the upload destination, invokes *SPIFFS.begin()*.
    - The [value member](acelements.md#value_3) of AutoConnectFile contains the file name of the upload file. Use its file name to access the uploaded file on the device.
    - Invokes the end function associated with the begin to close the device. It is the *SPIFFS.end()** if the flash on the ESP module has been begun for SPIFFS.

The following sketch is an example that implements the above basic steps. The *postUpload* function is the on-handler and retrieves the AutoConnectFile as named `upload_file`. You should note that this handler is **not** for a custom Web page placed with its AutoConnectFile element. The uploaded file should be processed by the handler for the transition destination page from the AutoConnectFile element placed page. AutoConnect built-in upload handler will save the uploaded file to the specified device before invoking the *postUpload* function.

However, If you use uploaded files in different situations, it may be more appropriate to place the actual handling process outside the handler. It applies for the parameter file, etc. The important thing is that you do not have to sketch file reception and storing logic by using the AutoConnectFile element and the upload handler built into the AutoConnect.

```cpp hl_lines="14 53 64 67 70 86"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <AutoConnect.h>

// Upload request custom Web page
static const char PAGE_UPLOAD[] PROGMEM = R"(
{
  "uri": "/",
  "title": "Upload",
  "menu": true,
  "element": [
    { "name":"caption", "type":"ACText", "value":"<h2>File uploading platform<h2>" },
    { "name":"upload_file", "type":"ACFile", "label":"Select file: ", "store":"fs" },
    { "name":"upload", "type":"ACSubmit", "value":"UPLOAD", "uri":"/upload" }
  ]
}
)";

// Upload result display
static const char PAGE_BROWSE[] PROGMEM = R"(
{
  "uri": "/upload",
  "title": "Upload",
  "menu": false,
  "element": [
    { "name":"caption", "type":"ACText", "value":"<h2>Uploading ended<h2>" },
    { "name":"filename", "type":"ACText" },
    { "name":"size", "type":"ACText", "format":"%s bytes uploaded" },
    { "name":"content_type", "type":"ACText", "format":"Content: %s" }
  ]
}
)";

ESP8266WebServer server;
AutoConnect portal(server);
// Declare AutoConnectAux separately as a custom web page to access
// easily for each page in the post-upload handler.
AutoConnectAux auxUpload;
AutoConnectAux auxBrowse;

/**
 * Post uploading, AutoConnectFile's built-in upload handler reads the
 * file saved in SPIFFS and displays the file contents on /upload custom
 * web page. However, only files with mime type uploaded as text are
 * displayed. A custom web page handler is called after upload.
 * @param  aux  AutoConnectAux(/upload)
 * @param  args PageArgument
 * @return Uploaded text content
 */
String postUpload(AutoConnectAux& aux, PageArgument& args) {
  String  content;
  AutoConnectFile&  upload = auxUpload["upload_file"].as<AutoConnectFile>();
  AutoConnectText&  aux_filename = aux["filename"].as<AutoConnectText>();
  AutoConnectText&  aux_size = aux["size"].as<AutoConnectText>();
  AutoConnectText&  aux_contentType = aux["content_type"].as<AutoConnectText>();
  // Assignment operator can be used for the element attribute.
  aux_filename.value = upload.value;
  aux_size.value = String(upload.size);
  aux_contentType.value = upload.mimeType;
  // The file saved by the AutoConnect upload handler is read from
  // the EEPROM and echoed to a custom web page.
  SPIFFS.begin();
  File uploadFile = SPIFFS.open(String("/" + upload.value).c_str(), "r");
  if (uploadFile) {
    while (uploadFile.available()) {
      char c = uploadFile.read();
      Serial.print(c);
    }
    uploadFile.close();
  }
  else
    content = "Not saved";
  SPIFFS.end();
  return String();
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  auxUpload.load(PAGE_UPLOAD);
  auxBrowse.load(PAGE_BROWSE);
  portal.join({ auxUpload, auxBrowse });
  auxBrowse.on(postUpload);
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

## Where will the file upload

The AutoConnect built-in upload handler can save the upload file to three locations:

1. Flash memory embedded in the ESP8266/ESP32 module
2. SD device externally connected to the ESP8266/ESP32 module
3. Other character devices

You can specify the device type to save with the [**store**](acelements.md#store) attribute of AutoConenctFile, and it accepts the following values:

- Flash : `AC_File_FS` for the API parameter or `fs` for the JSON document
- SD : `AC_File_SD` for the API parameter or `sd` for the JSON document
- Other : `AC_File_Extern` for the API parameter or `extern` for the JSON document

The substance of AC_File_FS (fs) is a SPIFFS file system implemented by the ESP8266/ESP32 core, and then AutoConnect uses the Global Instance **SPIFFS** to access SPIFFS.

Also, the substance of AC_File_SD (sd) is a FAT file of Arduino SD library ported to the ESP8266/ESP32 core, and then AutoConnect uses the Global Instance **SD** to access SD. When saving to an external SD device, there are additional required parameters for the connection interface and is defined as the macro in AutoConnectDefs.h.

```cpp
#define AUTOCONNECT_SD_CS       SS
#define AUTOCONNECT_SD_SPEED    4000000
```

`AUTOCONNECT_SD_CS` defines which GPIO for the CS (Chip Select, or SS as Slave Select) pin. This definition is derived from pins_arduino.h, which is included in the Arduino core distribution. If you want to assign the CS pin to another GPIO, you need to change the macro definition of AutoConnectDefs.h.

`AUTOCONNECT_SD_SPEED` defines SPI clock speed depending on the connected device.

!!! info "Involves both the begin() and the end()"
    The built-in uploader executes the begin and end functions regardless of the sketch whence the file system of the device will terminate with the uploader termination. Therefore, to use the device in the sketch after uploading, you need to **restart it with the begin** function.

## When it will be uploaded

Upload handler will be launched by ESP8266WebServer/WebServer(as ESP32) library which is triggered by receiving an HTTP stream of POST BODY including file content. Its launching occurs before invoking the page handler.

The following diagram illustrates the file uploading sequence:

<img src="images/ac_upload_flow.svg">

At the time of the page handler behaves, the uploaded file already saved to the device, and the [member variables](acelements.md#name_3) of AutoConnectFile reflects the file name and transfer size.

## The file name for the uploaded file

AutoConnetFile saves the uploaded file with the file name you selected by `<input type="file">` tag on the browser. The file name used for uploading is stored in the AutoConnetFile's value member, which you can access after uploading. (i.e. In the handler of the destination page by the AutoConnectSubmit element.) You can not save it with a different name. It can be renamed after upload if you need to change the name.

## Upload to a device other than Flash or SD

You can output the file to any device using a custom uploader by specifying [**extern**](acjson.md#acfile) with the [**store**](acjson.md#acfile) attribute of [AutoConnectFile](acjson.md#acfile) (or specifying [**AC_File_Extern**](acelements.md#store) for the [**store**](apielements.md#store) member variable) and can customize the uploader according to the need to upload files to other than Flash or SD. Implements your own uploader with inheriting the [**AutoConnectUploadHandler**](#upload-handler-base-class) class which is the base class of the upload handler.

!!! note "It's not so difficult"
    Implementing the custom uploader requires a little knowledge of the c++ language. If you are less attuned to programming c++, you may find it difficult. But don't worry. You can make it in various situations by just modifying the sketch skeleton that appears at the end of this page.

### <i class="fa fa-code"></i> Upload handler base class

AutoConnectUploadHandler is a base class of upload handler and It has one public member function and three protected functions. 

#### <i class="fa fa-caret-right"></i> Constructor

```cpp
AutoConnectUploadHandler()
```

#### <i class="fa fa-caret-right"></i> Member functions

The **upload** public function is an entry point, the ESP8266WebServer (WebServer as ESP32) library will invoke the upload with each time of uploading content divided into chunks. 

Also, the **\_open**, **\_write** and **\_close** protected functions are actually responsible for saving files and are declared as pure virtual functions. A custom uploader class that inherits from the AutoConnectUploadHandler class need to implement these functions.

The actual upload process is handled by the three private functions above, and then upload only invokes three functions according to the upload situation. In usually, there is no need to override the upload function in an inherited class.

```cpp
public virtual void upload(const String& requestUri, const HTTPUpload& upload)
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">requestUri</span><span class="apidesc">URI of upload request source.</span></dd>
    <dd><span class="apidef">upload</span><span class="apidesc">A data structure of the upload file as <b>HTTPUpload</b>. It is defined in the ESP8266WebServer (WebServer as ESP32) library as follows:

```cpp
typedef struct {
  HTTPUploadStatus status;
  String  filename;
  String  name;
  String  type;
  size_t  totalSize;
  size_t  currentSize;
  size_t  contentLength;
  uint8_t buf[HTTP_UPLOAD_BUFLEN];
} HTTPUpload;
```
    </span></dd>
</dl>

The upload handler needs to implement processing based on the enumeration value of HTTPUpload.status as **HTTPUploadStatus** enum type. HTTPUploadStatus enumeration is as follows:

- **`UPLOAD_FILE_START`** : Invokes to the \_open.
- **`UPLOAD_FILE_WRITE`** : Invokes to the \_write.
- **`UPLOAD_FILE_END`** : Invokes to the \_close.
- **`UPLOAD_FILE_ABORTED`** : Invokes to the \_close.

The \_open function will be invoked when HTTPUploadStatus is **UPLOAD_FILE_START**. Usually, the implementation of an inherited class will open the file.

```cpp
protected virtual bool _open(const char* filename, const char* mode) = 0
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">filename</span><span class="apidesc">Uploading file name.</span></dd>
    <dd><span class="apidef">mode</span><span class="apidesc">An indicator for the file access mode, a "w" for writing.</span></dd>
    <dt>**Return value**</dt>
    <dd><span class="apidef">true</span><span class="apidesc">File open successful.</span></dd>
    <dd><span class="apidef">false</span><span class="apidesc">Failed to open.</span></dd>
</dl>

The \_write function will be invoked when HTTPUploadStatus is **UPLOAD_FILE_WRITE**. The content of the upload file is divided and the \_write will be invoked in multiple times. Usually, the implementation of an inherited class will write data.

```cpp
protected virtual size_t _write(const uint8_t *buf, const size_t size))= 0
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">buf</span><span class="apidesc">File content block.</span></dd>
    <dd><span class="apidef">size</span><span class="apidesc">File block size to write.</span></dd>
    <dt>**Return value**</dt>
    <dd>Size written.</dd>
</dl>

The \_close function will be invoked when HTTPUploadStatus is **UPLOAD_FILE_END** or **UPLOAD_FILE_ABORTED**. Usually, the implementation of an inherited class will close the file.

```cpp
protected virtual void _close(void) = 0
```

For reference, the following AutoConnectUploadFS class is an implementation of AutoConnect built-in uploader and inherits from AutoConnectUploadHandler.

```cpp
class AutoConnectUploadFS : public AutoConnectUploadHandler {
 public:
  explicit AutoConnectUploadFS(SPIFFST& media) : _media(&media) {}
  ~AutoConnectUploadFS() { _close(); }

 protected:
  bool _open(const char* filename, const char* mode) override {
    if (_media->begin()) {
      _file = _media->open(filename, mode);
      return _file != false;      
    }
    return false;
  }

  size_t _write(const uint8_t* buf, const size_t size) override {
    if (_file)
      return _file.write(buf, size);
    else
      return -1;
  }

  void _close(void) override {
    if (_file)
      _file.close();
    _media->end();
  }

 private:
  SPIFFST*  _media;
  SPIFileT  _file; 
};
```

### <i class="fa fa-code"></i> Register custom upload handler

In order to upload a file by the custom uploader, it is necessary to register it to the custom Web page beforehand. To register a custom uploader, specify the custom uploader class name in the template argument of the  [AutoConnectAux::onUpload](apiaux.md#onupload) function and invokes it.

```cpp
void AutoConnectAux::onUpload<T>(T& uploadClass)
```
<dl class="apidl">
    <dt>**Parameters**</dt>
    <dd><span class="apidef">T</span><span class="apidesc">Specifies a class name of the custom uploader. This class name is a class that you implemented by inheriting AutoConnectUploadHandler for custom upload.</span></dd>
    <dd><span class="apidef">uploadClass</span><span class="apidesc">Specifies the custom upload class instance.</span></dd>
</dl>

The rough structure of the sketches that completed these implementations will be as follows:

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

static const char PAGE_UPLOAD[] PROGMEM = R"(
{
  "uri": "/",
  "title": "Upload",
  "menu": true,
  "element": [
    { "name":"caption", "type":"ACText", "value":"<h2>File uploading platform<h2>" },
    { "name":"upload_file", "type":"ACFile", "label":"Select file: ", "store":"extern" },
    { "name":"upload", "type":"ACSubmit", "value":"UPLOAD", "uri":"/upload" }
  ]
}
)";

static const char PAGE_RECEIVED[] PROGMEM = R"(
{
  "uri": "/upload",
  "title": "Upload ended",
  "menu": false,
  "element": [
    { "name":"caption", "type":"ACText", "value":"<h2>File uploading ended<h2>" }
  ]
}
)";

// Custom upload handler class
class CustomUploader : public AutoConnectUploadHandler {
public:
  CustomUploader() {}
  ~CustomUploader() {}

protected:
  bool   _open(const char* filename, const char* mode) override;
  size_t _write(const uint8_t *buf, const size_t size) override;
  void   _close(void) override;
};

// _open for custom open
bool CustomUploader::_open(const char* filename, const char* mode) {
  // Here, an implementation for the open file.
}

// _open for custom write
size_t CustomUploader::_write(const uint8_t *buf, const size_t size) {
  // Here, an implementation for the writing the file data.
}

// _open for custom close
void CustomUploader::_close(void) {
  // Here, an implementation for the close file.
}

AutoConnect     portal;
AutoConnectAux  uploadPage;
AutoConnectAux  receivePage;
CustomUploader  uploader;   // Declare the custom uploader

void setup() {
  uploadPage.load(PAGE_UPLOAD);
  receivePage.load(PAGE_RECEIVED);
  portal.join({ uploadPage, receivePage });
  receivePage.onUpload<CustomUploader>(uploader);  // Register the custom uploader
  portal.begin();
}

void loop() {
  portal.handleClient();
}
```

!!! note "Don't forget to specify the store"
    When using a custom uploader, remember to specify the **extern** for the store attribute of AutoConnectFile.

<script>
  window.onload = function() {
    Gifffer();
  };
</script>
