#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

class Message {
	public:
		virtual void buffer(uint8_t *ptr) = 0;
		virtual uint16_t getLength() = 0;
	};
#endif