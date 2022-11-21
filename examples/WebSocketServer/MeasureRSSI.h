#ifndef __MEASURERSSI_H_
#define __MEASURERSSI_H_

// RSSI measurement class
class MeasureRSSI {
 public:
  explicit MeasureRSSI(const unsigned long period = 30000, const unsigned int cycle = 20, const uint8_t points = 7);
  ~MeasureRSSI();

  bool  measure(void);

  unsigned long period;
  uint8_t  points;
  int rssi = 0;

 private:
  unsigned int  _cycle;
  unsigned long _tm;
  unsigned long _tmCycle;
  uint8_t  _n;
  int8_t*  _rssi = nullptr;
};

#endif // !__MEASURERSSI_H_
