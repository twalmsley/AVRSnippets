/*
TTYSerial.cpp (formerly NewSoftSerial.cpp) - 
*/

// 
// Includes
// 
#include "Arduino.h"
#include "TTYSerial.h"
//
// Lookup table
//
#if F_CPU == 16000000

const int XMIT_START_ADJUSTMENT = 5;

#else

#error This version of TTYSerial supports only 16MHz processors

#endif

//
// Private methods
//

/* static */ 
inline void TTYSerial::tunedDelay(uint16_t delay) { 
  uint8_t tmp=0;

  asm volatile("sbiw    %0, 0x01 \n\t"
    "ldi %1, 0xFF \n\t"
    "cpi %A0, 0xFF \n\t"
    "cpc %B0, %1 \n\t"
    "brne .-10 \n\t"
    : "+r" (delay), "+a" (tmp)
    : "0" (delay)
    );
}

void TTYSerial::tx_pin_write(uint8_t pin_state)
{
  if (pin_state == LOW)
    *_transmitPortRegister &= ~_transmitBitMask;
  else
    *_transmitPortRegister |= _transmitBitMask;
}

//
// Constructor
//
TTYSerial::TTYSerial(uint8_t transmitPin, bool inverse_logic /* = false */) : 
  _tx_delay(0),
  _inverse_logic(inverse_logic)
{
  setTX(transmitPin);
}

//
// Destructor
//
TTYSerial::~TTYSerial()
{
  end();
}

void TTYSerial::setTX(uint8_t tx)
{
  pinMode(tx, OUTPUT);
  digitalWrite(tx, HIGH);
  _transmitBitMask = digitalPinToBitMask(tx);
  uint8_t port = digitalPinToPort(tx);
  _transmitPortRegister = portOutputRegister(port);
}

//
// Public methods
//

void TTYSerial::begin(long adjust)
{
      _tx_delay = adjust;
}

void TTYSerial::end()
{
}

size_t TTYSerial::write(uint8_t b)
{
  if (_tx_delay == 0) {
    return 0;
  }

  uint8_t oldSREG = SREG;
  cli();  // turn off interrupts for a clean txmit

  // Write the start bit
  tx_pin_write(_inverse_logic ? HIGH : LOW);
  tunedDelay(_tx_delay + XMIT_START_ADJUSTMENT);

  // Write each of the 8 bits
  if (_inverse_logic)
  {
    for (byte mask = 0x01; mask; mask <<= 1)
    {
      if (b & mask) // choose bit
        tx_pin_write(LOW); // send 1
      else
        tx_pin_write(HIGH); // send 0
    
      tunedDelay(_tx_delay);
    }

	tx_pin_write(LOW);
    tx_pin_write(LOW); // restore pin to natural state
  }
  else
  {
    for (byte mask = 0x01; mask; mask <<= 1)
    {
      if (b & mask) // choose bit
        tx_pin_write(HIGH); // send 1
      else
        tx_pin_write(LOW); // send 0
    
      tunedDelay(_tx_delay);
    }

    tx_pin_write(HIGH); // restore pin to natural state
    tx_pin_write(HIGH); // restore pin to natural state
  }

  SREG = oldSREG; // turn interrupts back on
  tunedDelay(_tx_delay);
  
  return 1;
}
