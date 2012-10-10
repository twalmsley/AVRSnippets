#include "LeftScroller.h"

LeftScroller::LeftScroller(LedArrayDriver &ledArray, Message &message, uint8_t repeats):Animation(ledArray, repeats), _message(message) {
	init();
}

LeftScroller::~LeftScroller() {

}

void LeftScroller::init() {
	_message.buffer(_displayBuffer);
}

uint8_t LeftScroller::animate() {
	//
	// Set up and clock-in the first shift register bit
	//
	_ledArray.begin();
	//
	// Process each column in the buffer
	//
	uint16_t count = 0;
	while(count <= 80) {
		uint8_t pattern = _displayBuffer[count+_displayIndex];
		_ledArray.show(pattern);
		_ledArray.nextColumn();
		count++;
	}
	if(_repeatCount++ == _repeats) {
		_repeatCount = 0;
		_displayIndex++;
		if(_displayIndex > _message.getLength()) {
			_displayIndex = 0;
			return (uint8_t)0;
		}
	}
	return (uint8_t)1;
}

void* LeftScroller::operator new(size_t size) {
	return calloc(size, (size_t)1);
}

void LeftScroller::operator delete(void* ptr) {
	free(ptr);
}
