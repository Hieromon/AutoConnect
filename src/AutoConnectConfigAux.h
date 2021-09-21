/**
 * Declaration of AutoConnectConfigAux class.
 * AutoConnectConfigAux persists the AutoConnectConfig settings to
 * which it belongs and provides the ability to manipulate these
 * settings through a web interface.
 * The AutoConnectConfigAux class is derived from the AutoConnectAux
 * class, and it allows the sketch to operate AutoConnectConfigAux
 * itself and AutoConnectElements by using the functions that
 * AutoConnectAux has.
 * @file AutoConnectConfigAux.h
 * @author hieromon@gmail.com
 * @version  1.3.0
 * @date 2021-05-27
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTCONFIGAUX_H_
#define _AUTOCONNECTCONFIGAUX_H_

#include "AutoConnect.h"
#include "AutoConnectAux.h"

#ifdef AUTOCONNECT_USE_CONFIGAUX
// AutoConnectConfigAux is an experimental feature that allows 
// AutoConnectConfig to be persisted and transported. It has been
// implemented by a custom Web page as AutoConnectAux with a handler
// that is independent of the existing components of AutoConnect.
// You can access the AutoConnectConfigAux custom web page via the 
// Config item from the AutoConnect menu, then this page will load the
// current settings and allows you to change any item. The applied 
// settings will be reflected immediately in AutoConnect without calling
// AutoConnect::config.
// The AutoConnectConfigAux handler will check if acconfig.json exists 
// on the default file system when you visit its custom web page. The
// acconfig.json file is the JSON-represented persistence data for the 
// AutoConnectConfig instance saved by the APPLY button on the 
// AutoConnectConfigAux page.
// To activate the AutoConnectConfigAux, follow the next procedure with
// your sketch:
// 1. Uncommect #define AC_USE_CONFIGAUX in AutoConnectDefs.h
// 2. Include AutoConnectConfigAux.h header file with your Sketch
// 3. Declare an AutoConnectConfigAux object globally with your Sketch
// 4. Join the AutoConnectConfigAux object to AutoConnect using the join function
//
// #include <AutoConnect.h>
// #include <AutoConnectConfigAux.h>
// AutoConnect portal;
// AutoConnectConfigAux configUX;
// void setup() {
//   portal.join(configUX);
//   portal.begin();
// }
// void loop() {
//   portal.handleClient();
// }

#include <type_traits>

class AutoConnectConfigAux : public AutoConnectAux {
 public:
  explicit AutoConnectConfigAux(const char* uri = AUTOCONNECT_URI_CONFIGAUX, const char* title = AUTOCONNECT_MENULABEL_ACCONFIG, const char* fileName = AUTOCONNECT_CONFIGAUX_FILE) : AutoConnectAux(uri, title), _fn(String(fileName)) {
    static_assert(std::is_same<AutoConnectElement, AutoConnectElementJson>::value, "AUTOCONNECT_USE_JSON must be defined to activate AutoConnectConfigAux");
  }
  ~AutoConnectConfigAux() {}

 protected:
  void  _join(AutoConnect& ac) override;
  void  _loadSettings(void);
  void  _retrieveSettings(AutoConnectConfigAux& me);
  void  _restoreSettings(AutoConnectConfigAux& me);
  void  _saveSettings(AutoConnectAux& me);
  String  _settings(AutoConnectAux& me, PageArgument& arg);

  String  _fn;              /**< AutoConnectConfig setting file name */

 private:
  // The names of the AutoConnectElements placed on the Web page that
  // implements the operation page of AutoConnectConfigAux.
  static const char _ELM_APGATEWAY[] PROGMEM;
  static const char _ELM_APIP[] PROGMEM;
  static const char _ELM_APNETMASK[] PROGMEM;
  static const char _ELM_AUTH[] PROGMEM;
  static const char _ELM_AUTHSCOPE[] PROGMEM;
  static const char _ELM_AUTORECONNECT[] PROGMEM;
  static const char _ELM_AUTORESTART[] PROGMEM;
  static const char _ELM_AUTORISE[] PROGMEM;
  static const char _ELM_AUTOSAVE[] PROGMEM;
  static const char _ELM_BEGINTIMEOUT[] PROGMEM;
  static const char _ELM_BOOTURI[] PROGMEM;
  static const char _ELM_BOUNDARYOFFSET[] PROGMEM;
  static const char _ELM_BUILTINOTA[] PROGMEM;
  static const char _ELM_CHANNEL[] PROGMEM;
  static const char _ELM_ENABLEDHCP[] PROGMEM;
  static const char _ELM_HIDDEN[] PROGMEM;
  static const char _ELM_HOMEURI[] PROGMEM;
  static const char _ELM_HOSTNAME[] PROGMEM;
  static const char _ELM_IMMEDIATESTART[] PROGMEM;
  static const char _ELM_MENU[] PROGMEM;
  static const char _ELM_MENUCONFIGNEW[] PROGMEM;
  static const char _ELM_MENUDISCONNECT[] PROGMEM;
  static const char _ELM_MENUHOME[] PROGMEM;
  static const char _ELM_MENUOPENSSIDS[] PROGMEM;
  static const char _ELM_MENURESET[] PROGMEM;
  static const char _ELM_MENUTITLE[] PROGMEM;
  static const char _ELM_MENUUPDATE[] PROGMEM;
  static const char _ELM_MINRSSI[] PROGMEM;
  static const char _ELM_PASSWORD[] PROGMEM;
  static const char _ELM_PORTALTIMEOUT[] PROGMEM;
  static const char _ELM_PRESERVEAPMODE[] PROGMEM;
  static const char _ELM_PRIMARYDNS[] PROGMEM;
  static const char _ELM_PRINCIPLE[] PROGMEM;
  static const char _ELM_PSK[] PROGMEM;
  static const char _ELM_RECONNECTINT[] PROGMEM;
  static const char _ELM_RETAINPORTAL[] PROGMEM;
  static const char _ELM_SECONDARYDNS[] PROGMEM;
  static const char _ELM_SSID[] PROGMEM;
  static const char _ELM_STAGATEWAY[] PROGMEM;
  static const char _ELM_STAIP[] PROGMEM;
  static const char _ELM_STANETMASK[] PROGMEM;
  static const char _ELM_TICKER[] PROGMEM;
  static const char _ELM_TICKERON[] PROGMEM;
  static const char _ELM_TICKERPORT[] PROGMEM;
  static const char _ELM_USER[] PROGMEM;

  // AutoConnectConfigAux page definitions
  static const char* const _elmNames[] PROGMEM; /**< A collection of the AutoConnectElements placed on the AutoConnectConfigAux page. */
  static const char _ui[] PROGMEM;              /**< Json Document of AutoConnectConfigAux page  */
  static const char _rdlg[] PROGMEM;            /**< Script to prompt the reset dialog */
};

#endif // !AUTOCONNECT_USE_CONFIGAUX

#endif // !_AUTOCONNECTCONFIGAUX_H_
