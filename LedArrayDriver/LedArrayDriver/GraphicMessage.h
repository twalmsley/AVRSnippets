#ifndef GRAPHIC_MESSAGE
#define GRAPHIC_MESSAGE
#include <stddef.h>
#include <stdint.h>
#include "Message.h"

class GraphicMessage : public Message {
	public:
	GraphicMessage(uint8_t *message, uint16_t messageLen);
	~GraphicMessage();
	virtual void buffer(uint8_t *ptr);
	virtual uint16_t getLength();
	void* operator new(size_t size);
	void operator delete(void*);
	protected:
	uint8_t *_message;
	uint16_t _messageLen;
};
#endif