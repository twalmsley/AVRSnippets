/*
TTYSerial.h (formerly NewSoftSerial.h) - 
*/

#ifndef TTYSerial_h
#define TTYSerial_h

#include <inttypes.h>
#include <Stream.h>

/******************************************************************************
* Definitions
******************************************************************************/

#define _SS_MAX_RX_BUFF 64 // RX buffer size
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

class TTYSerial //: public Stream
{
private:
  // per object data
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;

  uint16_t _tx_delay;

  uint16_t _inverse_logic:1;

  // private methods
  void tx_pin_write(uint8_t pin_state);
  void setTX(uint8_t transmitPin);

  // private static method for timing
  static inline void tunedDelay(uint16_t delay);

public:
  // public methods
  TTYSerial(uint8_t transmitPin, bool inverse_logic = false);
  ~TTYSerial();
  void begin(long adjust);
  void end();

  virtual size_t write(uint8_t byte);
 
  //using Print::write;

};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round

#endif
