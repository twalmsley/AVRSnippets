#include "StaticDisplay.h"

StaticDisplay::StaticDisplay(LedArrayDriver &ledArray, Message &message, uint8_t repeats):Animation(ledArray, repeats), _message(message) {
	init();
}

StaticDisplay::~StaticDisplay() {

}

void StaticDisplay::init() {
	_message.buffer(_displayBuffer);
}

uint8_t StaticDisplay::animate() {
	//
	// Set up and clock-in the first shift register bit
	//
	_ledArray.begin();
	//
	// Process each column in the buffer
	//
	uint16_t count = 0;
	while(count <= _ledArray.numberOfColumns()) {
		uint8_t pattern = _displayBuffer[count];
		_ledArray.show(pattern);
		_ledArray.nextColumn();
		count++;
	}
	if(_repeatCount++ == _repeats) {
		init();
		_repeatCount = 0;
		return (uint8_t)0;
	}
	return (uint8_t)1;
}

void* StaticDisplay::operator new(size_t size) {
	return calloc(size, (size_t)1);
}

void StaticDisplay::operator delete(void* ptr) {
	free(ptr);
}
