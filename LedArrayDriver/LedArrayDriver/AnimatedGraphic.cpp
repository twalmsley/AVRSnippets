#include <string.h>
#include <stdlib.h>
#include "AnimatedGraphic.h"

AnimatedGraphic::AnimatedGraphic(uint8_t *message, uint16_t messageLen, uint8_t numberOfFrames)
	:_message(message),_messageLen( messageLen), _numberOfFrames(numberOfFrames),_frame(0){}

void AnimatedGraphic::buffer(uint8_t *dest) {
	uint16_t start = (_frame*_messageLen);
	
	for(uint16_t i = start;i< (start+_messageLen) ;i++) {
		uint8_t letter = _message[i];
		*dest++ = letter;
	}
	_frame++;
	if(_frame == _numberOfFrames) {
		_frame = 0;
	}
}

uint16_t AnimatedGraphic::getLength() {
	return _messageLen;
}

void* AnimatedGraphic::operator new(size_t size) {
	return calloc(size, (size_t)1);
}

void AnimatedGraphic::operator delete(void* ptr) {
	free(ptr);
}
