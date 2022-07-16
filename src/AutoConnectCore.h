/**
 * Define a core component of AutoConnect that implements the minimum
 * functionality of a decoupled AutoConnectAux structure.
 * @file AutoConnectCore.h
 * @author hieromon@gmail.com
 * @version 1.4.0
 * @date 2022-07-14
 * @copyright MIT license.
 */

#ifndef _AUTOCONNECTCORE_H_
#define _AUTOCONNECTCORE_H_

#include "AutoConnectCoreImpl.hpp"

using AutoConnectConfig = AutoConnectConfigBase;
using AutoConnect = AutoConnectCore<AutoConnectConfig>;

#endif // !_AUTOCONNECTCORE_H_
