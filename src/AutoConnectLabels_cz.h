/**
 *  AutoConnect proper menu label constant definition.
 *  Czech language.
 *  @file   AutoConnectLabels_cz.h
 *  @author dmaixner@gmail.com
 *  @version    1.0.0
 *  @date   2020-02-01
 *  @copyright  MIT license.
 */

#ifndef _AUTOCONNECTLABELS_H_
#define _AUTOCONNECTLABELS_H_

/**
 * This file serves as and alternative translation of
 * AutoConnectLabels.h.
 * To use it, you should add directive to building process.
 * E.g. for PlatformIO you can add to your environment in
 * platformio.ini this build_flag:
 * ```
 * build_flags = 
 *      '-D_AUTOCONNECTLABELSLANG_="AutoConnectLabels_cz.h"'
 * ```
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
#define AUTOCONNECT_MENULABEL_CONFIGNEW "Nastavení nové WiFi"
#endif // !AUTOCONNECT_MENULABEL_CONFIGNEW

// Menu item: Open SSIDs
#ifndef AUTOCONNECT_MENULABEL_OPENSSIDS
#define AUTOCONNECT_MENULABEL_OPENSSIDS "Uložené WiFi"
#endif // !AUTOCONNECT_MENULABEL_OPENSSIDS

// Menu item: Disconnect
#ifndef AUTOCONNECT_MENULABEL_DISCONNECT
#define AUTOCONNECT_MENULABEL_DISCONNECT "Odpojit"
#endif // !AUTOCONNECT_MENULABEL_DISCONNECT

// Menu item: Reset...
#ifndef AUTOCONNECT_MENULABEL_RESET
#define AUTOCONNECT_MENULABEL_RESET "Restart zařízení"
#endif // !AUTOCONNECT_MENULABEL_RESET

// Menu item: HOME
#ifndef AUTOCONNECT_MENULABEL_HOME
#define AUTOCONNECT_MENULABEL_HOME "Informace o zařízení"
#endif // !AUTOCONNECT_MENULABEL_HOME

// Menu item: Update
#ifndef AUTOCONNECT_MENULABEL_UPDATE
#define AUTOCONNECT_MENULABEL_UPDATE "Přehrát firmware"
#endif // !AUTOCONNECT_MENULABEL_UPDATE

// Button label: RESET
#ifndef AUTOCONNECT_BUTTONLABEL_RESET
#define AUTOCONNECT_BUTTONLABEL_RESET "Reset"
#endif // !AUTOCONNECT_BUTTONLABEL_RESET

// Page title: Page not found
#ifndef AUTOCONNECT_PAGETITLE_NOTFOUND
#define AUTOCONNECT_PAGETITLE_NOTFOUND "Stránka nenalezena"
#endif // !AUTOCONNECT_PAGETITLE_NOTFOUND

// Page title: AutoConnect resetting
#ifndef AUTOCONNECT_PAGETITLE_RESETTING
#define AUTOCONNECT_PAGETITLE_RESETTING "Restartuji"
#endif // !AUTOCONNECT_PAGETITLE_RESETTING

// Page title: AutoConnect statistics
#ifndef AUTOCONNECT_PAGETITLE_STATISTICS
#define AUTOCONNECT_PAGETITLE_STATISTICS "Statistika"
#endif // !AUTOCONNECT_PAGETITLE_STATISTICS

// Page statistics row: Established connection
#ifndef AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION
#define AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION "Navázané spojení"
#endif // !AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION

// Page statistics row: Mode
#ifndef AUTOCONNECT_PAGESTATS_MODE
#define AUTOCONNECT_PAGESTATS_MODE "Mód"
#endif // !AUTOCONNECT_PAGESTATS_MODE

// Page statistics row: IP
#ifndef AUTOCONNECT_PAGESTATS_IP
#define AUTOCONNECT_PAGESTATS_IP "IP adresa"
#endif // !AUTOCONNECT_PAGESTATS_IP

// Page statistics row: GW
#ifndef AUTOCONNECT_PAGESTATS_GATEWAY
#define AUTOCONNECT_PAGESTATS_GATEWAY "Brána"
#endif // !AUTOCONNECT_PAGESTATS_GATEWAY

// Page statistics row: Subnet mask
#ifndef AUTOCONNECT_PAGESTATS_SUBNETMASK
#define AUTOCONNECT_PAGESTATS_SUBNETMASK "Maska podsítě"
#endif // !AUTOCONNECT_PAGESTATS_SUBNETMASK

// Page statistics row: SoftAP IP
#ifndef AUTOCONNECT_PAGESTATS_SOFTAPIP
#define AUTOCONNECT_PAGESTATS_SOFTAPIP "AP IP adresa"
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
#define AUTOCONNECT_PAGESTATS_CHANNEL "Kanál"
#endif // !AUTOCONNECT_PAGESTATS_CHANNEL

// Page statistics row: dBm
#ifndef AUTOCONNECT_PAGESTATS_DBM
#define AUTOCONNECT_PAGESTATS_DBM "dBm"
#endif // !AUTOCONNECT_PAGESTATS_DBM

// Page statistics row: Chip ID
#ifndef AUTOCONNECT_PAGESTATS_CHIPID
#define AUTOCONNECT_PAGESTATS_CHIPID "ID čipu"
#endif // !AUTOCONNECT_PAGESTATS_CHIPID

// Page statistics row: CPU Freq.
#ifndef AUTOCONNECT_PAGESTATS_CPUFREQ
#define AUTOCONNECT_PAGESTATS_CPUFREQ "Frekvence CPU"
#endif // !AUTOCONNECT_PAGESTATS_CPUFREQ

// Page statistics row: Flash size
#ifndef AUTOCONNECT_PAGESTATS_FLASHSIZE
#define AUTOCONNECT_PAGESTATS_FLASHSIZE "Velikost paměti"
#endif // !AUTOCONNECT_PAGESTATS_FLASHSIZE

// Page statistics row: Free memory
#ifndef AUTOCONNECT_PAGESTATS_FREEMEM
#define AUTOCONNECT_PAGESTATS_FREEMEM "Volné paměti"
#endif // !AUTOCONNECT_PAGESTATS_FREEMEM

// Page title: AutoConnect config
#ifndef AUTOCONNECT_PAGETITLE_CONFIG
#define AUTOCONNECT_PAGETITLE_CONFIG "Konfigurace"
#endif // !AUTOCONNECT_PAGETITLE_CONFIG

// Page config text: Total:
#ifndef AUTOCONNECT_PAGECONFIG_TOTAL
#define AUTOCONNECT_PAGECONFIG_TOTAL "Celkem: "
#endif // !AUTOCONNECT_PAGECONFIG_TOTAL

// Page config text: Hidden:
#ifndef AUTOCONNECT_PAGECONFIG_HIDDEN
#define AUTOCONNECT_PAGECONFIG_HIDDEN "Skryté: "
#endif // !AUTOCONNECT_PAGECONFIG_HIDDEN

// Page config text: SSID
#ifndef AUTOCONNECT_PAGECONFIG_SSID
#define AUTOCONNECT_PAGECONFIG_SSID "SSID"
#endif // !AUTOCONNECT_PAGECONFIG_SSID

// Page config text: Passphrase
#ifndef AUTOCONNECT_PAGECONFIG_PASSPHRASE
#define AUTOCONNECT_PAGECONFIG_PASSPHRASE "Heslo"
#endif // !AUTOCONNECT_PAGECONFIG_PASSPHRASE

// Page config text: Enable DHCP
#ifndef AUTOCONNECT_PAGECONFIG_ENABLEDHCP
#define AUTOCONNECT_PAGECONFIG_ENABLEDHCP "Zapnout DHCP"
#endif // !AUTOCONNECT_PAGECONFIG_ENABLEDHCP

// Page config text: Apply
#ifndef AUTOCONNECT_PAGECONFIG_APPLY
#define AUTOCONNECT_PAGECONFIG_APPLY "Uložit"
#endif // !AUTOCONNECT_PAGECONFIG_APPLY

// Page title: AutoConnect credentials
#ifndef AUTOCONNECT_PAGETITLE_CREDENTIALS
#define AUTOCONNECT_PAGETITLE_CREDENTIALS "Pověření"
#endif // !AUTOCONNECT_PAGETITLE_CREDENTIALS

// Page title: AutoConnect connecting
#ifndef AUTOCONNECT_PAGETITLE_CONNECTING
#define AUTOCONNECT_PAGETITLE_CONNECTING "Připojuji"
#endif // !AUTOCONNECT_PAGETITLE_CONNECTING

// Page title: AutoConnect connection failed
#ifndef AUTOCONNECT_PAGETITLE_CONNECTIONFAILED
#define AUTOCONNECT_PAGETITLE_CONNECTIONFAILED "Připojení selhalo"
#endif // !AUTOCONNECT_PAGETITLE_CONNECTIONFAILED

// Page connection failed: Connection Failed
#ifndef AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED
#define AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED "Připojení selhalo"
#endif // !AUTOCONNECT_PAGECONNECTIONFAILED_CONNECTIONFAILED

// Page title: AutoConnect disconnected
#ifndef AUTOCONNECT_PAGETITLE_DISCONNECTED
#define AUTOCONNECT_PAGETITLE_DISCONNECTED "Odpojeno"
#endif // !AUTOCONNECT_PAGETITLE_DISCONNECTED

// Text: No saved credentials.
#ifndef AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS
#define AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS "Žádné uložené WiFi sítě."
#endif // !AUTOCONNECT_TEXT_NOSAVEDCREDENTIALS

// Menu Text: Connecting
#ifndef AUTOCONNECT_MENUTEXT_CONNECTING
#define AUTOCONNECT_MENUTEXT_CONNECTING "Připojuji"
#endif // !AUTOCONNECT_MENUTEXT_CONNECTING

// Menu Text: Disconnect
#ifndef AUTOCONNECT_MENUTEXT_DISCONNECT
#define AUTOCONNECT_MENUTEXT_DISCONNECT "Odpojit"
#endif // !AUTOCONNECT_MENUTEXT_DISCONNECT

// Menu Text: Failed
#ifndef AUTOCONNECT_MENUTEXT_FAILED
#define AUTOCONNECT_MENUTEXT_FAILED "Připojení selhalo"
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
#endif
