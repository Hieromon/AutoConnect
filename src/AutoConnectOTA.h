/**
 * Declaration of AutoConnectOTA class.
 * The AutoConnecOTA class is a class for web updating a Sketch binary
 * via OTA and implements with an AutoConnectAux page handler that
 * inherits from AutoConnectUploadHandler.
 * By overriding the _write function of AutoConnectUploadHandler to
 * write the executable binary using the Update class, it can update
 * the module firmware in synchronization with the upload of the sketch
 * binary file.
 * @file AutoConnectOTA.h
 * @author hieromon@gmail.com
 * @version  1.2.2
 * @date 2020-12-11
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTOTA_H_
#define _AUTOCONNECTOTA_H_

#include <memory>
#include "AutoConnect.h"
#include "AutoConnectUpload.h"

#include <FS.h>
#if defined(ARDUINO_ARCH_ESP8266)
#ifndef AUTOCONNECT_USE_SPIFFS
#include <LittleFS.h>
#endif
typedef fs::FS    SPIFFST;
#elif defined(ARDUINO_ARCH_ESP32)
#include <SPIFFS.h>
typedef fs::SPIFFSFS  SPIFFST;
#endif

class AutoConnectOTA : public AutoConnectUploadHandler {
public:
  // Updating process status
  typedef enum  {
    OTA_IDLE,               /**< Update process has not started */
    OTA_START,              /**< Update process has started */
    OTA_PROGRESS,           /**< Update process in progress */
    OTA_SUCCESS,            /**< A binary updater has uploaded fine */
    OTA_RIP,                /**< Ready for module restart */
    OTA_FAIL                /**< Failed to save binary updater by Update class */
  } AC_OTAStatus_t;

  // The treating destination of OTA transferred data
  typedef enum {
    OTA_DEST_FILE, /**< To be upload the file */
    OTA_DEST_FIRM  /**< To update the firmware */
  } AC_OTADest_t;

  AutoConnectOTA() : _dest(OTA_DEST_FIRM), _status(OTA_IDLE), _tickerPort(-1), _tickerOn(LOW) {};
  ~AutoConnectOTA();
  void  attach(AutoConnect& portal);                        /**< Attach itself to AutoConnect */
  void  authentication(const AC_AUTH_t auth);               /**< Set certain page authentication */
  String  error(void) const { return _err; }                /**< Returns current error string */
  void  menu(const bool post) { _auxUpdate->menu(post); }   /**< Enabel or disable arranging a created AutoConnectOTA page in the menu. */
  void  reset(void) { _status = OTA_IDLE; }                 /**< Reset the status */
  AC_OTAStatus_t  status(void) const { return _status; }    /**< Return a current error status of the Update class */ 
  AC_OTADest_t  dest(void) const { return _dest; }          /**< Return a current uploading destination */
  void  setTicker(int8_t pin, uint8_t on) { _tickerPort = pin, _tickerOn = on; }  /**< Set ticker LED port */

protected:
  // Attribute definition of the element to be placed on the update page.
  typedef struct {
    const ACElement_t type;
    const char*  name;      /**< Name to assign to AutoConnectElement */
    const char*  value;     /**< Value owned by an element */
    const char*  peculiar;  /**< Specific ornamentation for the element */
  } ACElementProp_t;

  // Attributes to treat included update pages as AutoConnectAux.
  typedef struct {
    const char*  uri;       /**< URI for the page */
    const char*  title;     /**< Menu title of update page */
    const bool   menu;      /**< Whether to display in menu */
    const ACElementProp_t* element;
  } ACPage_t;

  template <typename T, size_t N> constexpr size_t lengthOf(T (&)[N]) noexcept {
    return N;
  }
  void    _buildAux(AutoConnectAux* aux, const AutoConnectOTA::ACPage_t* page, const size_t elementNum);
  bool    _open(const char* filename, const char* mode) override;
  size_t  _write(const uint8_t *buf, const size_t size) override;
  void    _close(const HTTPUploadStatus status) override;
  String  _updated(AutoConnectAux& result, PageArgument& args);

  std::unique_ptr<AutoConnectAux> _auxUpdate;   /**< An update operation page */
  std::unique_ptr<AutoConnectAux> _auxResult;   /**< An update result page */

 private:
  void  _setError(void);
  void  _setError(const char* err);

  AC_OTADest_t _dest;           /**< Destination of OTA transferred data */
  AC_OTAStatus_t  _status;      /**< Status for update progress */
  int8_t  _tickerPort;          /**< GPIO for flicker */
  uint8_t _tickerOn;            /**< A signal for flicker turn on */
  String  _binName;             /**< An updater file name */
  String  _err;                 /**< Occurred error stamp */

  SPIFFST*  _fs;                /**< Filesystem for the native file uploading */
  fs::File  _file;              /**< File handler for the native file uploading */

  static const ACPage_t         _pageUpdate  PROGMEM;
  static const ACElementProp_t  _elmUpdate[] PROGMEM;
  static const ACPage_t         _pageResult  PROGMEM;
  static const ACElementProp_t  _elmResult[] PROGMEM;
};

#endif // !_AUTOCONNECTOTA_H_
