#include <string.h>
#include <stdlib.h>
#include "GraphicMessage.h"

GraphicMessage::GraphicMessage(uint8_t *message, uint16_t messageLen):_message(message),_messageLen(messageLen){}

void GraphicMessage::buffer(uint8_t *dest) {
	for(uint16_t i = 0;i<_messageLen;i++) {
		uint8_t letter = _message[i];
		*dest++ = letter;
	}
}

uint16_t GraphicMessage::getLength() {
	return _messageLen;
}

void* GraphicMessage::operator new(size_t size) {
	return calloc(size, (size_t)1);
}

void GraphicMessage::operator delete(void* ptr) {
	free(ptr);
}
