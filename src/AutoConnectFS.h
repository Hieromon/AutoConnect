/**
 * Predefined file system applied for AutoConnect usage.
 * @file AutoConnectFS.h
 * @author hieromon@gmail.com
 * @version  1.3.0
 * @date 2021-06-06
 * @copyright  MIT license.
 */

#ifndef _AUTOCONNECTFS_H_
#define _AUTOCONNECTFS_H_

// Globally deploy the applicable file system classes and instances.
// The type of the applicable file system to be used is switched
// according to the AC_USE_SPIFFS and AC_USE_LITTLEFS definition.
// The file system indicator to apply is expanded by the macro definition,
// AUTOCONNECT_APPLIED_FILECLASS is assigned the class name,
// and AUTOCONNECT_APPLIED_FILESYSTEM is assigned the global instance name.
#if defined(ARDUINO_ARCH_ESP8266)
#  define AC_DEFAULT_FILESYSTEM 2
#elif defined(ARDUINO_ARCH_ESP32)
#  define AC_DEFAULT_FILESYSTEM 1
#endif

#if !defined(AC_USE_SPIFFS) && !defined(AC_USE_LITTLEFS)
#  define AC_USE_FILESYSTEM AC_DEFAULT_FILESYSTEM
#elif defined(AC_USE_SPIFFS)
#  define AC_USE_FILESYSTEM 1
#elif defined(AC_USE_LITTLEFS)
#  define AC_USE_FILESYSTEM 2
#endif

#if AC_USE_FILESYSTEM == 1
#  include <FS.h>
#  ifdef ARDUINO_ARCH_ESP32
#    include <SPIFFS.h>
#  endif
#  define AUTOCONNECT_APPLIED_FILECLASS     fs::SPIFFSFS
#  define AUTOCONNECT_APPLIED_FILESYSTEM    SPIFFS
#elif AC_USE_FILESYSTEM == 2
#  if defined(ARDUINO_ARCH_ESP8266)
#    include <LittleFS.h>
#    define AUTOCONNECT_APPLIED_FILECLASS   fs::FS
#    define AUTOCONNECT_APPLIED_FILESYSTEM  LittleFS
#  elif defined(ARDUINO_ARCH_ESP32)
#    include <LITTLEFS.h>
#    define AUTOCONNECT_APPLIED_FILECLASS   fs::LITTLEFSFS
#    define AUTOCONNECT_APPLIED_FILESYSTEM  LITTLEFS
#  endif
#endif

// Deploy the file class for the AutoConnect scope.
namespace AutoConnectFS { using FS = AUTOCONNECT_APPLIED_FILECLASS; };

#endif // !_AUTOCONNECTFS_H_
