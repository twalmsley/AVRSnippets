#ifndef ASCII_MESSAGE
#define ASCII_MESSAGE
#include <stddef.h>
#include <stdint.h>
#include "Message.h"

class AsciiMessage : public Message {
	public:
		AsciiMessage(char *message);
		~AsciiMessage();
		void buffer(uint8_t *ptr);
		uint16_t getLength();
        void* operator new(size_t size);
        void operator delete(void*);
	private:
		char *_message;
	};
#endif