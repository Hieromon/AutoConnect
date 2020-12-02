/**
 *  AutoConnectTicker class implementation.
 *  Provides a service that shows the flicker signal according to WiFi
 *  connection status. 
 *  @file   AutoConnectTicker.cpp
 *  @author hieromon@gmail.com
 *  @version    0.9.11
 *  @date   2019-07-09
 *  @copyright  MIT license.
 */

#include "AutoConnectTicker.h"

/**
 * Start ticker cycle
 * @param cycle Cycle time in [ms]
 * @param duty  Duty cycle in [ms]
 */
void AutoConnectTicker::start(const uint32_t cycle, const uint32_t duty) {
  _cycle = cycle;
  if (duty <= _cycle)
    _duty = duty;
  start();
}

/**
 * Start ticker cycle
 */
void AutoConnectTicker::start(void) {
  pinMode(_port, OUTPUT);
  _pulse.detach();
  _period.attach_ms<AutoConnectTicker*>(_cycle, AutoConnectTicker::_onPeriod, this);
}

/**
 * Turn on the flicker signal and reserves a ticker to turn off the
 * signal. This behavior will perform every cycle to generate the
 * pseudo-PWM signal.
 * If the function is registered, call the callback function at the
 * end of one cycle.
 * @param  t  Its own address
 */
void AutoConnectTicker::_onPeriod(AutoConnectTicker* t) {
  digitalWrite(t->_port, t->_turnOn);
  t->_pulse.once_ms<AutoConnectTicker*>(t->_duty, AutoConnectTicker::_onPulse, t);
  if (t->_callback)
    t->_callback();
}

/**
 * Turn off the flicker signal
 * @param  t  Its own address
 */
void AutoConnectTicker::_onPulse(AutoConnectTicker* t) {
  digitalWrite(t->_port, !(t->_turnOn));
}
