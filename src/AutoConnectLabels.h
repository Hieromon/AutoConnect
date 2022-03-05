/**
 *  AutoConnect proper menu label constant definition.
 *  @file   AutoConnectLabels.h
 *  @author hieromon@gmail.com
 *  @version    1.3.4
 *  @date   2022-02-09
 *  @copyright  MIT license.
 */

#ifndef _AUTOCONNECTLABELS_H_
#define _AUTOCONNECTLABELS_H_

/**
 * The upper row of each of the following definition items is the
 * default value. The lower row is an alternative label string as
 * a sample and can be changed you wish.
 * 
 * Notes:
 * You can find some tokens for the PageBuilder if you parse
 * AutoConnectPage.cpp. Some of the tokens are valid at the time
 * the AutoConnect menu will generate. For example, the token
 * `{{CUR_SSID}}` returns the SSID of the currently participating
 * AP by AutoConnect::_token_CURRENT_SSID function.
 * You can use these tokens to display dynamic menus showing the
 * current situation, but you need to know the internal structure
 * of AutoConnect in order to display the appropriate menu.
 */

// Menu item: Configure new AP
#ifndef AUTOCONNECT_MENULABEL_CONFIGNEW
#define AUTOCONNECT_MENULABEL_CONFIGNEW   "Configure new AP"
//#define AUTOCONNECT_MENULABEL_CONFIGNEW   "Establish a new AP"
#endif // !AUTOCONNECT_MENULABEL_CONFIGNEW

// Menu item: Open SSIDs
#ifndef AUTOCONNECT_MENULABEL_OPENSSIDS
#define AUTOCONNECT_MENULABEL_OPENSSIDS   "Open SSIDs"
//#define AUTOCONNECT_MENULABEL_OPENSSIDS   "Open credentials"
#endif // !AUTOCONNECT_MENULABEL_OPENSSIDS

// Menu item: Disconnect
#ifndef AUTOCONNECT_MENULABEL_DISCONNECT
#define AUTOCONNECT_MENULABEL_DISCONNECT  "Disconnect"
//#define AUTOCONNECT_MENULABEL_DISCONNECT  "Leave {{CUR_SSID}}"
#endif // !AUTOCONNECT_MENULABEL_DISCONNECT

// Menu item: Reset...
#ifndef AUTOCONNECT_MENULABEL_RESET
#define AUTOCONNECT_MENULABEL_RESET       "Reset..."
//#define AUTOCONNECT_MENULABEL_RESET       "Reboot..."
#endif // !AUTOCONNECT_MENULABEL_RESET

// Menu item: HOME
#ifndef AUTOCONNECT_MENULABEL_HOME
#define AUTOCONNECT_MENULABEL_HOME        "HOME"
//#define AUTOCONNECT_MENULABEL_HOME        "Main"
#endif // !AUTOCONNECT_MENULABEL_HOME

// Menu item: Update
#ifndef AUTOCONNECT_MENULABEL_UPDATE
#define AUTOCONNECT_MENULABEL_UPDATE      "Update"
#endif // !AUTOCONNECT_MENULABEL_UPDATE

// Menu item: Device Info
#ifndef AUTOCONNECT_MENULABEL_DEVINFO
#define AUTOCONNECT_MENULABEL_DEVINFO     "Device info"
#endif // !AUTOCONNECT_MENULABEL_DEVINFO

// Menu item: Config
#ifndef AUTOCONNECT_MENULABEL_ACCONFIG
#define AUTOCONNECT_MENULABEL_ACCONFIG    "Config"
#endif // !AUTOCONNECT_MENULABEL_ACCONFIG

// Button label: RESET
#ifndef AUTOCONNECT_BUTTONLABEL_RESET
#define AUTOCONNECT_BUTTONLABEL_RESET     "RESET"
//#define AUTOCONNECT_BUTTONLABEL_RESET     "Reboot"
#endif // !AUTOCONNECT_BUTTONLABEL_RESET

// Button label: UPDATE
#ifndef AUTOCONNECT_BUTTONLABEL_UPDATE
#define AUTOCONNECT_BUTTONLABEL_UPDATE     "UPDATE"
#endif // !AUTOCONNECT_BUTTONLABEL_UPDATE

// Page title: Page not found
#ifndef AUTOCONNECT_PAGETITLE_NOTFOUND
#define AUTOCONNECT_PAGETITLE_NOTFOUND "Page not found"
#endif // !AUTOCONNECT_PAGETITLE_NOTFOUND

// Page title: AutoConnect resetting
#ifndef AUTOCONNECT_PAGETITLE_RESETTING
#define AUTOCONNECT_PAGETITLE_RESETTING "AutoConnect resetting"
#endif // !AUTOCONNECT_PAGETITLE_RESETTING

// Page title: AutoConnect statistics
#ifndef AUTOCONNECT_PAGETITLE_STATISTICS
#define AUTOCONNECT_PAGETITLE_STATISTICS "AutoConnect statistics"
#endif // !AUTOCONNECT_PAGETITLE_STATISTICS

// Page statistics row: Established connection
#ifndef AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION
#define AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION "Established connection"
#endif // !AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION

// Page statistics row: Mode
#ifndef AUTOCONNECT_PAGESTATS_MODE
#define AUTOCONNECT_PAGESTATS_MODE "Mode"
#endif // !AUTOCONNECT_PAGESTATS_MODE

// Page statistics row: IP
#ifndef AUTOCONNECT_PAGESTATS_IP
#define AUTOCONNECT_PAGESTATS_IP "IP"
#endif // !AUTOCONNECT_PAGESTATS_IP

// Page statistics row: GW
#ifndef AUTOCONNECT_PAGESTATS_GATEWAY
#define AUTOCONNECT_PAGESTATS_GATEWAY "GW"
#endif // !AUTOCONNECT_PAGESTATS_GATEWAY

// Page statistics row: Subnet mask
#ifndef AUTOCONNECT_PAGESTATS_SUBNETMASK
#define AUTOCONNECT_PAGESTATS_SUBNETMASK "Subnet mask"
#endif // !AUTOCONNECT_PAGESTATS_SUBNETMASK

// Page statistics row: SoftAP IP
#ifndef AUTOCONNECT_PAGESTATS_SOFTAPIP
#define AUTOCONNECT_PAGESTATS_SOFTAPIP "SoftAP IP"
#endif // !AUTOCONNECT_PAGESTATS_SOFTAPIP

// Page statistics row: AP MAC
#ifndef AUTOCONNECT_PAGESTATS_APMAC
#define AUTOCONNECT_PAGESTATS_APMAC "AP MAC"
#endif // !AUTOCONNECT_PAGESTATS_APMAC

// Page statistics row: STA MAC
#ifndef AUTOCONNECT_PAGESTATS_STAMAC
#define AUTOCONNECT_PAGESTATS_STAMAC "STA MAC"
#endif // !AUTOCONNECT_PAGESTATS_STAMAC

// Page statistics row: Channel
#ifndef AUTOCONNECT_PAGESTATS_CHANNEL
#define AUTOCONNECT_PAGESTATS_CHANNEL "Channel"
#endif // !AUTOCONNECT_PAGESTATS_CHANNEL

// Page statistics row: dBm
#ifndef AUTOCONNECT_PAGESTATS_DBM
#define AUTOCONNECT_PAGESTATS_DBM "dBm"
#endif // !AUTOCONNECT_PAGESTATS_DBM

// Page statistics row: Chip ID
#ifndef AUTOCONNECT_PAGESTATS_CHIPID
#define AUTOCONNECT_PAGESTATS_CHIPID "Chip ID"
#endif // !AUTOCONNECT_PAGESTATS_CHIPID

// Page statistics row: CPU Freq.
#ifndef AUTOCONNECT_PAGESTATS_CPUFREQ
#define AUTOCONNECT_PAGESTATS_CPUFREQ "CPU Freq."
#endif // !AUTOCONNECT_PAGESTATS_CPUFREQ

// Page statistics row: Flash size
#ifndef AUTOCONNECT_PAGESTATS_FLASHSIZE
#define AUTOCONNECT_PAGESTATS_FLASHSIZE "Flash size"
#endif // !AUTOCONNECT_PAGESTATS_FLASHSIZE

// Page statistics row: Free memory
#ifndef AUTOCONNECT_PAGESTATS_FREEMEM
#define AUTOCONNECT_PAGESTATS_FREEMEM "Free memory"
#endif // !AUTOCONNECT_PAGESTATS_FREEMEM

// Page title: AutoConnect config
#ifndef AUTOCONNECT_PAGETITLE_CONFIG
#define AUTOCONNECT_PAGETITLE_CONFIG "AutoConnect config"
#endif // !AUTOCONNECT_PAGETITLE_CONFIG

// Page AutoConnectConfigAux section label: SoftAP settings
#ifndef AUTOCONNECT_PAGELABEL_SOFTAPSETTINGS
#define AUTOCONNECT_PAGELABEL_SOFTAPSETTINGS "SoftAP settings"
#endif // !AUTOCONNECT_PAGELABEL_SOFTAPSETTINGS

// Page AutoConnectConfigAux section label: Captive portal control
#ifndef AUTOCONNECT_PAGELABEL_CPCONTROL
#define AUTOCONNECT_PAGELABEL_CPCONTROL "Captive portal control"
#endif // !AUTOCONNECT_PAGELABEL_CPCONTROL

// Page AutoConnectConfigAux section label: Connection control
#ifndef AUTOCONNECT_PAGELABEL_CONNECTIONCONTROL
#define AUTOCONNECT_PAGELABEL_CONNECTIONCONTROL "Connection control"
#endif // !AUTOCONNECT_PAGELABEL_CONNECTIONCONTROL

// Page AutoConnectConfigAux section label: Station settings
#ifndef AUTOCONNECT_PAGELABEL_STATIONSETTINGS
#define AUTOCONNECT_PAGELABEL_STATIONSETTINGS "Station settings"
#endif // !AUTOCONNECT_PAGELABEL_STATIONSETTINGS

// Page AutoConnectConfigAux section label: Authentication settings
#ifndef AUTOCONNECT_PAGELABEL_AUTHSETTINGS
#define AUTOCONNECT_PAGELABEL_AUTHSETTINGS "Authentication settings"
#endif // !AUTOCONNECT_PAGELABEL_AUTHSETTINGS

// Page AutoConnectConfigAux section label: Miscellaneous
#ifndef AUTOCONNECT_PAGELABEL_MISCELLANEOUS
#define AUTOCONNECT_PAGELABEL_MISCELLANEOUS "Miscellaneous"
#endif // !AUTOCONNECT_PAGELABEL_MISCELLANEOUS

// Page config text: Total:
#ifndef AUTOCONNECT_PAGECONFIG_TOTAL
#define AUTOCONNECT_PAGECONFIG_TOTAL "Total:"
#endif // !AUTOCONNECT_PAGECONFIG_TOTAL

// Page config text: Hidden:
#ifndef AUTOCONNECT_PAGECONFIG_HIDDEN
#define AUTOCONNECT_PAGECONFIG_HIDDEN "Hidden:"
#endif // !AUTOCONNECT_PAGECONFIG_HIDDEN

// Page config text: Hidden SSID
#ifndef AUTOCONNECT_PAGECONFIG_HIDDENSSID
#define AUTOCONNECT_PAGECONFIG_HIDDENSSID "Hidden SSID"
#endif // !AUTOCONNECT_PAGECONFIG_HIDDENSSID

// Page config text: SSID
#ifndef AUTOCONNECT_PAGECONFIG_SSID
#define AUTOCONNECT_PAGECONFIG_SSID "SSID"
#endif // !AUTOCONNECT_PAGECONFIG_SSID

// Page config text: Passphrase
#ifndef AUTOCONNECT_PAGECONFIG_PASSPHRASE
#define AUTOCONNECT_PAGECONFIG_PASSPHRASE "Passphrase"
#endif // !AUTOCONNECT_PAGECONFIG_PASSPHRASE

// Page AutoConnectConfigAux text: IP Address
#ifndef AUTOCONNECT_PAGECONFIG_IPADDRESS
#define AUTOCONNECT_PAGECONFIG_IPADDRESS "IP Address"
#endif // !AUTOCONNECT_PAGECONFIG_IPADDRESS

// Page AutoConnectConfigAux text: Gateway
#ifndef AUTOCONNECT_PAGECONFIG_GATEWAY
#define AUTOCONNECT_PAGECONFIG_GATEWAY "Gateway"
#endif // !AUTOCONNECT_PAGECONFIG_GATEWAY

// Page config text: Netmask
#ifndef AUTOCONNECT_PAGECONFIG_NETMASK
#define AUTOCONNECT_PAGECONFIG_NETMASK "Netmask"
#endif // !AUTOCONNECT_PAGECONFIG_NETMASK

// Page config text: DNS1
#ifndef AUTOCONNECT_PAGECONFIG_DNS1
#define AUTOCONNECT_PAGECONFIG_DNS1 "DNS1"
#endif // !AUTOCONNECT_PAGECONFIG_DNS1

// Page config text: DNS2
#ifndef AUTOCONNECT_PAGECONFIG_DNS2
#define AUTOCONNECT_PAGECONFIG_DNS2 "DNS2"
#endif // !AUTOCONNECT_PAGECONFIG_DNS2

// Page AutoConnectConfigAux text: Primary DNS
#ifndef AUTOCONNECT_PAGECONFIG_PRIMARYDNS
#define AUTOCONNECT_PAGECONFIG_PRIMARYDNS "Primary DNS"
#endif // !AUTOCONNECT_PAGECONFIG_PRIMARYDNS

// Page AutoConnectConfigAux text: Primary DNS
#ifndef AUTOCONNECT_PAGECONFIG_SECONDARYDNS
#define AUTOCONNECT_PAGECONFIG_SECONDARYDNS "Secondary DNS"
#endif // !AUTOCONNECT_PAGECONFIG_SECONDARYDNS

// Page AutoConnectConfigAux text: Auto popup
#ifndef AUTOCONNECT_PAGECONFIG_AUTOPOPUP
#define AUTOCONNECT_PAGECONFIG_AUTOPOPUP "Auto popup"
#endif // !AUTOCONNECT_PAGECONFIG_AUTOPOPUP

// Page AutoConnectConfigAux text: Immediate start
#ifndef AUTOCONNECT_PAGECONFIG_IMMEDIATESTART
#define AUTOCONNECT_PAGECONFIG_IMMEDIATESTART "Immediate start"
#endif // !AUTOCONNECT_PAGECONFIG_IMMEDIATESTART

// Page AutoConnectConfigAux text: Retain portal
#ifndef AUTOCONNECT_PAGECONFIG_RETAINPORTAL
#define AUTOCONNECT_PAGECONFIG_RETAINPORTAL "Retain portal"
#endif // !AUTOCONNECT_PAGECONFIG_RETAINPORTAL

// Page AutoConnectConfigAux text: Captive portal timeout
#ifndef AUTOCONNECT_PAGECONFIG_CPTIMEOUT
#define AUTOCONNECT_PAGECONFIG_CPTIMEOUT "Captive portal timeout"
#endif // !AUTOCONNECT_PAGECONFIG_CPTIMEOUT

// Page AutoConnectConfigAux text: Principle
#ifndef AUTOCONNECT_PAGECONFIG_PRINCIPLE
#define AUTOCONNECT_PAGECONFIG_PRINCIPLE "Principle"
#endif // !AUTOCONNECT_PAGECONFIG_PRINCIPLE

// Page AutoConnectConfigAux text: Min RSSI
#ifndef AUTOCONNECT_PAGECONFIG_MINRSSI
#define AUTOCONNECT_PAGECONFIG_MINRSSI "Min RSSI"
#endif // !AUTOCONNECT_PAGECONFIG_MINRSSI

// Page AutoConnectConfigAux text: BEGIN timeout
#ifndef AUTOCONNECT_PAGECONFIG_BEGINTIMEOUT
#define AUTOCONNECT_PAGECONFIG_BEGINTIMEOUT "BEGIN Timeout"
#endif // !AUTOCONNECT_PAGECONFIG_BEGINTIMEOUT

// Page AutoConnectConfigAux text: Auto reconnect
#ifndef AUTOCONNECT_PAGECONFIG_AUTORECONNECT
#define AUTOCONNECT_PAGECONFIG_AUTORECONNECT "Auto reconnect"
#endif // !AUTOCONNECT_PAGECONFIG_AUTORECONNECT

// Page AutoConnectConfigAux text: Reconnect interval
#ifndef AUTOCONNECT_PAGECONFIG_RECONNECTINTERVAL
#define AUTOCONNECT_PAGECONFIG_RECONNECTINTERVAL "Reconnect interval"
#endif // !AUTOCONNECT_PAGECONFIG_RECONNECTINTERVAL

// Page AutoConnectConfigAux text: Save credential
#ifndef AUTOCONNECT_PAGECONFIG_SAVECREDENTIAL
#define AUTOCONNECT_PAGECONFIG_SAVECREDENTIAL "Save credential"
#endif // !AUTOCONNECT_PAGECONFIG_SAVECREDENTIAL

// Page AutoConnectConfigAux text: Auto restart
#ifndef AUTOCONNECT_PAGECONFIG_AUTORESTART
#define AUTOCONNECT_PAGECONFIG_AUTORESTART "Auto restart"
#endif // !AUTOCONNECT_PAGECONFIG_AUTORESTART

// Page AutoConnectConfigAux text: Preserve AP Mode
#ifndef AUTOCONNECT_PAGECONFIG_PRESERVEAPMODE
#define AUTOCONNECT_PAGECONFIG_PRESERVEAPMODE "Preserve AP Mode"
#endif // !AUTOCONNECT_PAGECONFIG_PRESERVEAPMODE

// Page config text: Enable DHCP
#ifndef AUTOCONNECT_PAGECONFIG_ENABLEDHCP
#define AUTOCONNECT_PAGECONFIG_ENABLEDHCP "Enable DHCP"
#endif // !AUTOCONNECT_PAGECONFIG_ENABLEDHCP

// Page AutoConnectConfigAux text: Host name
#ifndef AUTOCONNECT_PAGECONFIG_HOSTNAME
#define AUTOCONNECT_PAGECONFIG_HOSTNAME "Host name"
#endif // !AUTOCONNECT_PAGECONFIG_HOSTNAME

// Page AutoConnectConfigAux text: Home URI
#ifndef AUTOCONNECT_PAGECONFIG_HOMEURI
#define AUTOCONNECT_PAGECONFIG_HOMEURI "Home URI"
#endif // !AUTOCONNECT_PAGECONFIG_HOMEURI

// Page AutoConnectConfigAux text: On Boot URI
#ifndef AUTOCONNECT_PAGECONFIG_ONBOOTURI
#define AUTOCONNECT_PAGECONFIG_ONBOOTURI "On Boot URI"
#endif // !AUTOCONNECT_PAGECONFIG_ONBOOTURI

// Page AutoConnectConfigAux text: Authentication
#ifndef AUTOCONNECT_PAGECONFIG_AUTHENTICATION
#define AUTOCONNECT_PAGECONFIG_AUTHENTICATION "Authentication"
#endif // !AUTOCONNECT_PAGECONFIG_AUTHENTICATION

// Page AutoConnectConfigAux text: Scope
#ifndef AUTOCONNECT_PAGECONFIG_AUTHSCOPE
#define AUTOCONNECT_PAGECONFIG_AUTHSCOPE "Scope"
#endif // !AUTOCONNECT_PAGECONFIG_AUTHSCOPE

// Page AutoConnectConfigAux text: User name
#ifndef AUTOCONNECT_PAGECONFIG_USERNAME
#define AUTOCONNECT_PAGECONFIG_USERNAME "User name"
#endif // !AUTOCONNECT_PAGECONFIG_USERNAME

// Page AutoConnectConfigAux text: Password
#ifndef AUTOCONNECT_PAGECONFIG_PASSWORD
#define AUTOCONNECT_PAGECONFIG_PASSWORD "Password"
#endif // !AUTOCONNECT_PAGECONFIG_PASSWORD

// Page AutoConnectConfigAux text: Menu title
#ifndef AUTOCONNECT_PAGECONFIG_MENUTITLE
#define AUTOCONNECT_PAGECONFIG_MENUTITLE "Menu title"
#endif // !AUTOCONNECT_PAGECONFIG_MENUTITLE

// Page AutoConnectConfigAux text: Menu to use
#ifndef AUTOCONNECT_PAGECONFIG_MENUSUSE
#define AUTOCONNECT_PAGECONFIG_MENUSUSE "Menu to use"
#endif // !AUTOCONNECT_PAGECONFIG_MENUSUSE

// Page AutoConnectConfigAux text: Ticker
#ifndef AUTOCONNECT_PAGECONFIG_TICKER
#define AUTOCONNECT_PAGECONFIG_TICKER "Ticker"
#endif // !AUTOCONNECT_PAGECONFIG_TICKER

// Page AutoConnectConfigAux text: ON
#ifndef AUTOCONNECT_PAGECONFIG_TICKERON
#define AUTOCONNECT_PAGECONFIG_TICKERON "ON"
#endif // !AUTOCONNECT_PAGECONFIG_TICKERON

// Page AutoConnectConfigAux text: Active LOW
#ifndef AUTOCONNECT_PAGECONFIG_TICKERACTIVE
#define AUTOCONNECT_PAGECONFIG_TICKERACTIVE "Active LOW"
#endif // !AUTOCONNECT_PAGECONFIG_TICKERACTIVE

// Page AutoConnectConfigAux text: GPIO
#ifndef AUTOCONNECT_PAGECONFIG_TICKERPORT
#define AUTOCONNECT_PAGECONFIG_TICKERPORT "GPIO"
#endif // !AUTOCONNECT_PAGECONFIG_TICKERPORT

// Page AutoConnectConfigAux text: Built-in OTA
#ifndef AUTOCONNECT_PAGECONFIG_BUILTINOTA
#define AUTOCONNECT_PAGECONFIG_BUILTINOTA "Import Built-in OTA"
#endif // !AUTOCONNECT_PAGECONFIG_BUILTINOTA

// Page AutoConnectConfigAux text: Boundary offset
#ifndef AUTOCONNECT_PAGECONFIG_BOUNDARY
#define AUTOCONNECT_PAGECONFIG_BOUNDARY "Boundary offset for credentials store"
#endif // !AUTOCONNECT_PAGECONFIG_BOUNDARY

// Page config text: Apply
#ifndef AUTOCONNECT_PAGECONFIG_APPLY
#define AUTOCONNECT_PAGECONFIG_APPLY "Apply"
#endif // !AUTOCONNECT_PAGECONFIG_APPLY

// Page title: AutoConnect credentials
#ifndef AUTOCONNECT_PAGETITLE_CREDENTIALS
#define AUTOCONNECT_PAGETITLE_CREDENTIALS "AutoConnect credentials"
#endif // !AUTOCONNECT_PAGETITLE_CREDENTIALS

// Page title: AutoConnect connecting
#ifndef AUTOCONNECT_PAGETITLE_CONNECTING
#define AUTOCONNECT_PAGETITLE_CONNECTING "AutoConnect connecting"
#endif // !AUTOCONNECT_PAGETITLE_CONNECTING

// Page title: AutoConnect connection failed
#ifndef AUTOCONNECT_PAGETITLE_CONNECTIONFAILED
#define AUTOCONNECT_PAGETITLE_CONNECTIONFAILED "AutoConnect connection failed"
#endif // !AUTOCONNECT_PAGETITLE_CONNECTIONFAILED

// Page connection failed: Connection Failed
#ifndef AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED
#define AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED "Connection Failed"
#endif // !AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED

// Page title: AutoConnect disconnected
#ifndef AUTOCONNECT_PAGETITLE_DISCONNECTED
#define AUTOCONNECT_PAGETITLE_DISCONNECTED "AutoConnect disconnected"
#endif // !AUTOCONNECT_PAGETITLE_DISCONNECTED

// Text: Delete a credential.
#ifndef AUTOCONNECT_TEXT_DELETECREDENTIAL
#define AUTOCONNECT_TEXT_DELETECREDENTIAL "Delete a credential?"
#endif // !AUTOCONNECT_TEXT_DELETECREDENTIAL

// Text: Could not deleted a credential.
#ifndef AUTOCONNECT_TEXT_COULDNOTDELETED
#define AUTOCONNECT_TEXT_COULDNOTDELETED "could not deleted"
#endif // !AUTOCONNECT_TEXT_COULDNOTDELETED

// Text: No saved credentials.
#ifndef AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS
#define AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS "No saved credentials."
#endif // !AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS

// Text: The update page caption
#ifndef AUTOCONNECT_TEXT_UPDATINGFIRMWARE
#define AUTOCONNECT_TEXT_UPDATINGFIRMWARE "Updating firmware"
#endif // !AUTOCONNECT_TEXT_UPDATINGFIRMWARE

// Text: The update page's file selection button label
#ifndef AUTOCONNECT_TEXT_SELECTFIRMWARE
#define AUTOCONNECT_TEXT_SELECTFIRMWARE "Select file: "
#endif // !AUTOCONNECT_TEXT_SELECTFIRMWARE

// Text: OTA success
#ifndef AUTOCONNECT_TEXT_OTASUCCESS
#define AUTOCONNECT_TEXT_OTASUCCESS "Successfully updated, rebooting..."
#endif // !AUTOCONNECT_TEXT_OTASUCCESS

// Text: OTA success
#ifndef AUTOCONNECT_TEXT_OTAUPLOADED
#define AUTOCONNECT_TEXT_OTAUPLOADED "Successfully uploaded"
#endif // !AUTOCONNECT_TEXT_OTAUPLOADED

// Text: OTA failure
#ifndef AUTOCONNECT_TEXT_OTAFAILURE
#define AUTOCONNECT_TEXT_OTAFAILURE "Failed to update: "
#endif // !AUTOCONNECT_TEXT_OTAFAILURE

// Text: Authenticaton failed
#ifndef AUTOCONNECT_TEXT_AUTHFAILED
#define AUTOCONNECT_TEXT_AUTHFAILED "Authenticaton failed"
#endif  // !AUTOCONNECT_TEXT_AUTHFAILED

// Menu Text: Connecting
#ifndef AUTOCONNECT_MENUTEXT_CONNECTING
#define AUTOCONNECT_MENUTEXT_CONNECTING "Connecting"
#endif // !AUTOCONNECT_MENUTEXT_CONNECTING

// Menu Text: Disconnect
#ifndef AUTOCONNECT_MENUTEXT_DISCONNECT
#define AUTOCONNECT_MENUTEXT_DISCONNECT "Disconnect"
#endif // !AUTOCONNECT_MENUTEXT_DISCONNECT

// Menu Text: Failed
#ifndef AUTOCONNECT_MENUTEXT_FAILED
#define AUTOCONNECT_MENUTEXT_FAILED "Failed"
#endif // !AUTOCONNECT_MENUTEXT_FAILED

// Menu colors
// The following three color code items determine the color scheme of
// the menu. In addition to hexadecimal color values, you can specify
// 140 standard color names that apply to CSS.
// To be no sense of discomfort, select the same series of values for
// the background and the active item.

// Menu text foreground color
#ifndef AUTOCONNECT_MENUCOLOR_TEXT
#define AUTOCONNECT_MENUCOLOR_TEXT        "#fff"
//#define AUTOCONNECT_MENUCOLOR_TEXT        "#fffacd"
#endif // !AUTOCONNECT_MENUCOLOR_TEXT

// Menu background color
#ifndef AUTOCONNECT_MENUCOLOR_BACKGROUND
#define AUTOCONNECT_MENUCOLOR_BACKGROUND  "#263238"
// #define AUTOCONNECT_MENUCOLOR_BACKGROUND  "#696969"
#endif // !AUTOCONNECT_MENUCOLOR_BACKGROUND

// Background color with active menu items and mouse hover
#ifndef AUTOCONNECT_MENUCOLOR_ACTIVE
#define AUTOCONNECT_MENUCOLOR_ACTIVE      "#37474f"
// #define AUTOCONNECT_MENUCOLOR_ACTIVE      "#808080"
#endif

/**< Override the hardcoded strings contained in the AutoConnect pages.    */
/**< e.g. for PlatformIO, you can add your environment in platformio.ini   */
/**< along with AC_LABLES macro which specifies the user-defined label     */
/**< constants as follows:                                                 */
/**<                                                                       */
/**< build_flags =                                                         */
/**<   -DAC_LABELS='"${PROJECT_SRC_DIR}/mylabels.h"'                       */
/**<                                                                       */
/**< And places mylabels.h, it needs a structure of the define directive   */
/**< provided per label string definition for the change your wants.       */
/**<                                                                       */
/**< #ifdef [ID YOU WANT TO CHANGE]                                        */
/**< #undef [ID YOU WANT TO CHANGE]                                        */
/**< #define [ID YOU WANT TO CHANGE] "NEW_STRING_FOR_THISONE"              */
/**< #endif                                                                */
/**<                                                                       */
/**< example:
#ifdef AUTOCONNECT_MENULABEL_CONFIGNEW
#undef AUTOCONNECT_MENULABEL_CONFIGNEW
#define AUTOCONNECT_MENULABEL_CONFIGNEW   "NEW_STRING_FOR_THISONE"
#endif
*/
#ifdef AC_LABELS
#include AC_LABELS
#endif

#endif
