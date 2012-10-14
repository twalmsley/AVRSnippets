#ifndef REPEATEDGRAPHIC
#define REPEATEDGRAPHIC
#include <stddef.h>
#include <stdint.h>
#include "GraphicMessage.h"

class RepeatedGraphic : public GraphicMessage {
	public:
	RepeatedGraphic(uint8_t *message, uint16_t messageLen, uint8_t repetitions);
	~RepeatedGraphic();
	virtual void buffer(uint8_t *ptr);
	virtual uint16_t getLength();
	void* operator new(size_t size);
	void operator delete(void*);
	private:
	uint8_t _repetitions;
};
#endif