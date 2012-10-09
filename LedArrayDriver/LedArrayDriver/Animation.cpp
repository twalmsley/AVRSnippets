#include "Animation.h"

Animation::Animation(LedArrayDriver &ledArray, uint8_t repeats) 
	:	_ledArray(ledArray), 
		_displayIndex(0), 
		_repeats(repeats),
		_repeatCount(0) {

}

Animation::~Animation() {

}
