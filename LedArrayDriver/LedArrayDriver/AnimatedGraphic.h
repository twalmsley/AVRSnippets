#ifndef ANIMATEDGRAPHIC
#define ANIMATEDGRAPHIC
#include <stddef.h>
#include <stdint.h>
#include "Message.h"

class AnimatedGraphic: public Message {
	public:
	AnimatedGraphic(uint8_t *message, uint16_t messageLen, uint8_t numberOfFrames);
	~AnimatedGraphic();
	virtual void buffer(uint8_t *ptr);
	virtual uint16_t getLength();
	void* operator new(size_t size);
	void operator delete(void*);

	protected:

	uint8_t *_message;
	uint16_t _messageLen;
	uint8_t _numberOfFrames;
	uint8_t _frame;// the current frame
};
#endif