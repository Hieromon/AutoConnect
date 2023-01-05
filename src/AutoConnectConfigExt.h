/**
 * AutoConnect configuration settings extended structure. It has the
 * necessary configuration settings for all AutoConnect components.
 * @file AutoConnectConfigExt.h
 * @author hieromon@gmail.com
 * @version 1.4.1
 * @date 2023-01-05
 * @copyright MIT license.
 */

#ifndef _AUTOCONNECTCONFIGEXT_H_
#define _AUTOCONNECTCONFIGEXT_H_

#include "AutoConnectConfigBase.h"

class AutoConnectConfigExt : public AutoConnectConfigBase {
 public:
  using AutoConnectConfigBase::AutoConnectConfigBase;
  explicit AutoConnectConfigExt() :
    AutoConnectConfigBase(),
    ota(AC_OTA_EXTRA),
    otaExtraCaption(nullptr) {
      menuItems |= AC_MENUITEM_UPDATE;
    }

  explicit AutoConnectConfigExt(const char* ap, const char* password, const unsigned long portalTimeout = 0, const uint8_t channel = AUTOCONNECT_AP_CH) :
    AutoConnectConfigBase(ap, password, portalTimeout, channel),
    ota(AC_OTA_EXTRA),
    otaExtraCaption(nullptr) {
      menuItems |= AC_MENUITEM_UPDATE;
    }

  ~AutoConnectConfigExt() {}

  AutoConnectConfigExt& operator=(const AutoConnectConfigExt& o) {
    AutoConnectConfigBase::operator=(o);
    ota = o.ota;
    otaExtraCaption = o.otaExtraCaption;
    return *this;
  }

  AC_OTA_t  ota;                /**< Attach built-in OTA */
  const char* otaExtraCaption;  /**< Extra caption of OTA Updating Firmware screen */
};

#endif // !_AUTOCONNECTCONFIGEXT_H_
