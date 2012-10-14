#include <string.h>
#include <stdlib.h>
#include "RepeatedGraphic.h"

RepeatedGraphic::RepeatedGraphic(uint8_t *message, uint16_t messageLen, uint8_t repetitions)
	:GraphicMessage(message,messageLen),_repetitions(repetitions){}

void RepeatedGraphic::buffer(uint8_t *dest) {
	for(uint8_t j = 0; j < _repetitions; j++) {
		for(uint16_t i = 0;i<_messageLen;i++) {
			uint8_t letter = _message[i];
			*dest++ = letter;
		}
		*dest++ = 0;
	}
}

uint16_t RepeatedGraphic::getLength() {
	return (_messageLen + 1) * _repetitions;
}

void* RepeatedGraphic::operator new(size_t size) {
	return calloc(size, (size_t)1);
}

void RepeatedGraphic::operator delete(void* ptr) {
	free(ptr);
}
