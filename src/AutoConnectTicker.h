/**
 *  Declaration of AutoConnectTicker class.
 *  @file   AutoConnectTicker.h
 *  @author hieromon@gmail.com
 *  @version    0.9.11
 *  @date   2019-07-09
 *  @copyright  MIT license.
 */

#ifndef _AUTOCONNECTTICKER_H_
#define _AUTOCONNECTTICKER_H_

#include <functional>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include <Ticker.h>
#include "AutoConnectDefs.h"

class AutoConnectTicker {
 public:
	explicit AutoConnectTicker(const uint8_t port = AUTOCONNECT_TICKER_PORT, const uint8_t active = LOW, const uint32_t cycle = 0, uint32_t duty = 0) : _cycle(cycle), _duty(duty), _port(port), _turnOn(active), _callback(nullptr) {
    if (_duty > _cycle)
      _duty = _cycle;
  }
  ~AutoConnectTicker() { stop(); }

  typedef std::function<void(void)> Callback_ft;
  void setCycle(const uint32_t cycle) { _cycle = cycle; }
  void setDuty(const uint32_t duty) { _duty = duty <= _cycle ? duty : _duty; }
  void start(const uint32_t cycle, const uint32_t duty);
  void start(const uint32_t cycle, const uint8_t width) { start(cycle, (uint32_t)((cycle * width) >> 8)); }
  void start(void);
  void stop(void) { _period.detach(); _pulse.detach(); digitalWrite(_port, !_turnOn); }
  void onPeriod(Callback_ft cb) { _callback = cb ;}

 protected:
  Ticker    _period;        /**< Ticker for flicking cycle */
  Ticker    _pulse;         /**< Ticker for pulse width generating */
  uint32_t  _cycle;         /**< Cycle time in [ms] */
  uint32_t  _duty;          /**< Pulse width in [ms] */

 private:
  static void _onPeriod(AutoConnectTicker* t);
  static void _onPulse(AutoConnectTicker* t);
  uint8_t     _port;        /**< Port to output signal */
  uint8_t     _turnOn;      /**< Signal to turn on */
  Callback_ft _callback;    /**< An exit by every cycle */
};

#endif // !_AUTOCONNECTTICKER_H_
