#include "JBuffer.h"

#include <memory>

JBuffer::JBuffer() 
{
	JBuffer(JBUFFER_DEFAULT_CAPACITY);
}
JBuffer::JBuffer(UINT _capacity)
	: deqOffset(0), enqOffset(0), capacity(_capacity), isExternalBuffer(false), internalCapacity(capacity + 1)
{
	//buffer = new BYTE[capacity + 1];
	buffer = new BYTE[internalCapacity];
}
JBuffer::JBuffer(UINT _capacity, BYTE* _buffer) 
	: deqOffset(0), enqOffset(0), capacity(_capacity - 1), buffer(_buffer), isExternalBuffer(true)
{}

JBuffer::~JBuffer()
{
	if (!isExternalBuffer) {
		delete[] buffer;
	}	
}

bool JBuffer::Resize(UINT _size) {
	if (!isExternalBuffer && capacity < _size) {
		capacity = _size;
		BYTE* newBuffer = new BYTE[_size + 1];

		if (GetUseSize() > 0) {
			if (GetUseSize() > GetDirectDequeueSize()) {
				memcpy(newBuffer, GetDequeueBufferPtr(), GetDirectDequeueSize());
				memcpy(&newBuffer[GetDirectDequeueSize()], buffer, GetUseSize() - GetDirectDequeueSize());
			}
			else {
				memcpy(newBuffer, GetDequeueBufferPtr(), GetUseSize());
			}
		}

		deqOffset = 0;
		enqOffset = GetUseSize();
		capacity = _size;
		delete[] buffer;

		buffer = newBuffer;

		return true;
	}
	else {
		return false;
	}
}