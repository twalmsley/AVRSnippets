#ifndef ANIMATION_H
#define ANIMATION_H

#include "LedArrayDriver.h"

#define BUFFERSIZE 1000

class Animation {
	public:
		Animation(LedArrayDriver &ledArray, uint8_t repeats);
		~Animation();
		virtual void init() = 0;
		virtual void animate() = 0;
	protected:
		LedArrayDriver &_ledArray;
		uint8_t _displayBuffer[BUFFERSIZE];
		uint16_t _displayIndex;
		uint8_t _repeats;
		uint8_t _repeatCount;
	};


#endif