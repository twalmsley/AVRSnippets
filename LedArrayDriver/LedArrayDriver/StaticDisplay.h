#ifndef STATIC_DISPLAY_H
#define STATIC_DISPLAY_H


#include <stddef.h>
#include <stdlib.h>
#include "Animation.h"
#include "Message.h"

class StaticDisplay : public Animation {
	public:
	StaticDisplay(LedArrayDriver &ledArray, Message &message, uint8_t repeats);
	~StaticDisplay();

	virtual void init();
	virtual uint8_t animate();

	void* operator new(size_t size);
	void operator delete(void*);
	private:
	Message &_message;
};

#endif