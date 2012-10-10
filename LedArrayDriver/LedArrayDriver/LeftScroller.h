#ifndef LEFTSCROLLER_H
#define LEFTSCROLLER_H

#include <stddef.h>
#include <stdlib.h>
#include "Animation.h"
#include "Message.h"

class LeftScroller : public Animation {
	public:
		LeftScroller(LedArrayDriver &ledArray, Message &message, uint8_t repeats);
		~LeftScroller();

		virtual void init();
		virtual uint8_t animate();

        void* operator new(size_t size);
        void operator delete(void*);
	private:
		Message &_message;
	};
#endif