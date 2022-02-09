/**
 *  AutoConnect quoted type declarations.
 *  @file AutoConnectTypes.h
 *  @author hieromon@gmail.com
 *  @version  1.3.4
 *  @date 2022-02-09
 *  @copyright  MIT license.
 */

#ifndef _AUTOCONNECTTYPES_H_
#define _AUTOCONNECTTYPES_H_

/**< A type to save established credential at WiFi.begin automatically. */
typedef enum AC_SAVECREDENTIAL {
  AC_SAVECREDENTIAL_NEVER,
  AC_SAVECREDENTIAL_AUTO,
  AC_SAVECREDENTIAL_ALWAYS
} AC_SAVECREDENTIAL_t;

/**< URI that can be specified to AutoConnectConfig::bootUri. */
typedef enum AC_ONBOOTURI {
  AC_ONBOOTURI_ROOT,
  AC_ONBOOTURI_HOME
} AC_ONBOOTURI_t;

/** WiFi connection principle, it specifies the order of WiFi connecting with saved credentials. */
typedef enum AC_PRINCIPLE {
  AC_PRINCIPLE_RECENT,
  AC_PRINCIPLE_RSSI
} AC_PRINCIPLE_t;

/**< An enumerated type of the designated menu items. */
typedef enum AC_MENUITEM {
  AC_MENUITEM_NONE       = 0x0000,
  AC_MENUITEM_CONFIGNEW  = 0x0001,
  AC_MENUITEM_OPENSSIDS  = 0x0002,
  AC_MENUITEM_DISCONNECT = 0x0004,
  AC_MENUITEM_RESET      = 0x0008,
  AC_MENUITEM_HOME       = 0x0010,
  AC_MENUITEM_UPDATE     = 0x0020,
  AC_MENUITEM_DEVINFO    = 0x0040,
  AC_MENUITEM_DELETESSID = 0x0080
} AC_MENUITEM_t;

/**< Specifier for using built-in OTA */
typedef enum AC_OTA {
  AC_OTA_EXTRA,
  AC_OTA_BUILTIN
} AC_OTA_t;

/**< Scope of certification influence */
typedef enum AC_AUTHSCOPE {
  AC_AUTHSCOPE_PARTIAL  = 0x0001, // Available for particular AUX-pages.
  AC_AUTHSCOPE_AUX      = 0x0002, // All AUX-pages are affected by an authentication.
  AC_AUTHSCOPE_AC       = 0x0004, // Allow authentication to AutoConnect pages.
  AC_AUTHSCOPE_PORTAL   = AC_AUTHSCOPE_AC | AC_AUTHSCOPE_AUX, // All AutoConnect pages are affected by an authentication.
  AC_AUTHSCOPE_WITHCP   = 0x8000  // Allows authenticating in the standalone state.
} AC_AUTHSCOPE_t;

/**< A type to enable authentication. */
typedef enum AC_AUTH {
  AC_AUTH_NONE,
  AC_AUTH_DIGEST,
  AC_AUTH_BASIC
} AC_AUTH_t;

#endif // !_AUTOCONNECTTYPES_H_
