#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif
#include "MeasureRSSI.h"

MeasureRSSI::MeasureRSSI(const unsigned long period, const unsigned int cycle, const uint8_t points) :
  period(period),
  points(points),
  _cycle(cycle)
{
  _n = 0;
  _tm = millis();
  _tmCycle = _tm;
}

MeasureRSSI::~MeasureRSSI() {
  if (_rssi)
    delete[] _rssi;
}

/**
 * If the real-time measurement is required, the MeasreRSSI::measure function
 * should always be called within the loop function. A sketch does not have to
 * intentionally create a measurement period. In particular, don't use delay.
 * It just adds unnecessary delays.
 * @return  true  The measurement is completed. MeasureRSSI::rssi has the
 * average value of RSSI measured for the specified number of times.
 * @return  false Measurement in progress.
 */
bool MeasureRSSI::measure(void) {
  unsigned long ct = millis();

  if (ct - _tm < period)
    return false;

  if (ct - _tmCycle < _cycle)
    return false;

  _tmCycle = ct;

  if (!_rssi)
    _rssi = new int8_t[points];

  _rssi[_n++] = WiFi.RSSI();
  
  if (_n >= points) {
    rssi = 0;
    do {
      rssi += _rssi[--_n];
    } while (_n);
    rssi = rssi / points;
    delete[] _rssi;
    _rssi = nullptr;
    _tm = _tmCycle;
    return true;
  }

  return false;
}
