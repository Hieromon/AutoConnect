/**
 * AutoConnectExt class owns members that extend AutoConnectCore to
 * implement the full components of AutoConnect. Its class declaration
 * is instantiated by a template whose argument is AutoConnectConfigExt.
 * @file AutoConnectExt.hpp
 * @author hieromon@gmail.com
 * @version 1.4.1
 * @date 2022-12-07
 * @copyright MIT license.
 */

#ifndef _AUTOCONNECTEXT_HPP_
#define _AUTOCONNECTEXT_HPP_

#include "AutoConnectCoreImpl.hpp"
#include "AutoConnectAux.h"

// The realization of AutoConnectOTA is effective only by the explicit
#include "AutoConnectOTA.h"
class AutoConnectOTA;  // Reference to avoid circular

// The realization of AutoConnectUpdate is effective only by the explicit
// definition of AUTOCONNECT_USE_UPDATE
#include "AutoConnectUpdate.h"
class AutoConnectUpdate;  // Reference to avoid circular

typedef std::vector<std::reference_wrapper<AutoConnectAux>> AutoConnectAuxVT;

template<typename T>
class AutoConnectExt : public AutoConnectCore<T> {
 public:
  using AutoConnectCore<T>::AutoConnectCore;
  virtual ~AutoConnectExt();
  void  end(void) override;
  String where(void) const { return _auxUri; }

  AutoConnectAux* aux(const String& uri) const;
  AutoConnectAux* append(const String& uri, const String& title);
  AutoConnectAux* append(const String& uri, const String& title, WebServer::THandlerFunction handler);
  bool  detach(const String& uri);
  void  join(AutoConnectAux& aux);
  void  join(AutoConnectAuxVT auxVector);
  AutoConnectAux& locate(const String& uri) const { return *aux(uri); }
  bool  on(const String& uri, const AuxHandlerFunctionT handler, AutoConnectExitOrder_t order = AC_EXIT_AHEAD);

  /** For AutoConnectAux described in JSON */
#ifdef AUTOCONNECT_USE_JSON
  bool  load(PGM_P aux);
  bool  load(const __FlashStringHelper* aux);
  bool  load(const String& aux);
  bool  load(Stream& aux);
#endif // !AUTOCONNECT_USE_JSON

  typedef std::function<void(void)>       OTAStartExit_ft;
  typedef std::function<void(void)>       OTAEndExit_ft;
  typedef std::function<void(uint8_t)>    OTAErrorExit_ft;
  typedef std::function<void(unsigned int, unsigned int)> OTAProgressExit_ft;
  void  onOTAStart(OTAStartExit_ft fn);
  void  onOTAEnd(OTAEndExit_ft fn);
  void  onOTAError(OTAErrorExit_ft fn);
  void  onOTAProgress(OTAProgressExit_ft fn);

 protected:
  void  _handleUpload(const String& requestUri, const HTTPUpload& upload);
  PageElement*  _setupFetch(const String& uri);

#ifdef AUTOCONNECT_USE_JSON
  template<typename U>
  bool  _parseJson(U in);
  bool  _load(JsonVariant& aux);
#endif // !AUTOCONNECT_USE_JSON

  /** Utilities */
  OTAStartExit_ft     _onOTAStartExit;
  OTAEndExit_ft       _onOTAEndExit;
  OTAErrorExit_ft     _onOTAErrorExit;
  OTAProgressExit_ft  _onOTAProgressExit;
  size_t              _freeHeapSize;

  /** Extended pages made up with AutoConnectAux */
  AutoConnectAux* _aux = nullptr; /**< A top of registered AutoConnectAux */
  String        _auxUri;        /**< Last accessed AutoConnectAux */
  String        _prevUri;       /**< Previous generated page uri */
  /** Available updater, only reset by AutoConnectUpdate::attach is valid */
  std::unique_ptr<AutoConnectUpdate>  _update;
  /** OTA updater */
  std::unique_ptr<AutoConnectOTA>     _ota;

 private:
  // The following members are used to separate AutoConnectAux-dependent
  // processes from AutoConnectCore that were mingled with AutoConnect.
  // These are treated as dummy calls in AutoConnectCore.
  inline void _attachOTA(void) override;
  inline void _enableUpdate(void) override;
  inline bool _handleOTA(void) override;
  inline bool _handleUpdate(void) override;
  inline void _registerOnUpload(PageBuilder* page) override;
  inline void _releaseAux(const String& uri) override;
  inline void _saveCurrentUri(const String& uri) override;
  inline String _mold_MENU_AUX(PageArgument& args) override;

  friend class AutoConnectAux;
  friend class AutoConnectUpdate;
};

/**
 * Returns AutoConnectAux instance of specified.
 * @param  uri  An uri string.
 * @return A pointer of AutoConnectAux instance.
 */
template<typename T>
AutoConnectAux* AutoConnectExt<T>::aux(const String& uri) const {
  AutoConnectAux* aux_p = _aux;
  while (aux_p) {
    if (!strcmp(aux_p->uri(), uri.c_str()))
      break;
    aux_p = aux_p->_next;
  }
  if (!aux_p) {
    AC_DBG("'%s' not found in auxiliaries", uri.c_str());
    if (uri[0] != '/') {
      AC_DBG_DUMB(", path may be missing '/'");
    }
    AC_DBG_DUMB("\n");
  }
  return aux_p;
}

/**
 * Append auxiliary pages made up with AutoConnectAux.
 * @param  aux  A reference to AutoConnectAux that made up
 * the auxiliary page to be added.
 */
template<typename T>
void AutoConnectExt<T>::join(AutoConnectAux& aux) {
  if (_aux)
    _aux->_concat(aux);
  else
    _aux = &aux;
  aux._join(*this);
  AC_DBG("%s on hands\n", aux.uri());
}

#endif  // _AUTOCONNECTEXT_HPP_
