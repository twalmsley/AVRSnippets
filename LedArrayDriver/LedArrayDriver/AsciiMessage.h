#ifndef ASCII_MESSAGE
#define ASCII_MESSAGE

#include <stdint.h>

class AsciiMessage {
	public:
		AsciiMessage(char *message);
		~AsciiMessage();
		void buffer(uint8_t *ptr);
		uint16_t getLength();
	private:
		char *_message;
	};
#endif