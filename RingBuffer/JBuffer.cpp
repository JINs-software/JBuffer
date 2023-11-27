#include "JBuffer.h"

#include <memory>

JBuffer::JBuffer(UINT capacity)
{
	deqOffset = enqOffset = 0;
	this->capacity = capacity + 1;
	buffer = new BYTE[this->capacity];
#ifdef _DEBUG
	memset(buffer, 0xff, this->capacity);
#endif // DEBUG

}

JBuffer::~JBuffer()
{
	delete[] buffer;
}

void JBuffer::Resize(UINT size)
{
	if (capacity - 1 < size) {
		BYTE* newBuffer = new BYTE[size + 1];

		if (enqOffset >= deqOffset) {
			memcpy(newBuffer, &buffer[deqOffset], enqOffset - deqOffset);
		}
		else {
			memcpy(newBuffer, &buffer[deqOffset], capacity - deqOffset);
			memcpy(&newBuffer[capacity - deqOffset], buffer, enqOffset);
		}

		UINT useSize = GetUseSize();
		deqOffset = 0;
		enqOffset = useSize;
		capacity = size + 1;
		delete[] buffer;
		buffer = newBuffer;
	}
}

UINT JBuffer::GetBufferSize(void)
{
	return capacity;
}

UINT JBuffer::GetUseSize(void)
{
	if (enqOffset >= deqOffset) {
		return enqOffset - deqOffset;
	}
	else {
		return (enqOffset)+(capacity - deqOffset);
	}
}

UINT JBuffer::GetFreeSize(void)
{
	return (capacity - 1) - GetUseSize();
}

UINT JBuffer::Enqueue(const BYTE* data, UINT uiSize)
{
	UINT freeSize = GetFreeSize();
	UINT enqueueSize = (freeSize >= uiSize) ? uiSize : freeSize;
	UINT dirEnqueueSize = GetDirectEnqueueSize();

	if (dirEnqueueSize >= enqueueSize) {
		memcpy(GetEnqueueBufferPtr(), data, enqueueSize);
		enqOffset = (enqOffset + enqueueSize) % capacity;
	}
	else {
		memcpy(GetEnqueueBufferPtr(), data, dirEnqueueSize);
		memcpy(buffer, &data[dirEnqueueSize], enqueueSize - dirEnqueueSize);
		enqOffset = enqueueSize - dirEnqueueSize;
	}

	return enqueueSize;
}

UINT JBuffer::Dequeue(BYTE* dest, UINT uiSize)
{
	UINT useSize = GetUseSize();
	UINT dequeueSize = (useSize >= uiSize) ? uiSize : useSize;
	UINT dirDequeueSize = GetDirectDequeueSize();

	if (dirDequeueSize >= dequeueSize) {
		memcpy(dest, GetDequeueBufferPtr(), dequeueSize);
#ifdef _DEBUG
		memset(GetDequeueBufferPtr(), 0xff, dequeueSize);
#endif // DEBUG
		deqOffset = (deqOffset + dequeueSize) % capacity;
	}
	else {
		memcpy(dest, GetDequeueBufferPtr(), dirDequeueSize);
#ifdef _DEBUG
		memset(GetDequeueBufferPtr(), 0xff, dirDequeueSize);
#endif // DEBUG
		memcpy(&dest[dirDequeueSize], buffer, dequeueSize - dirDequeueSize);
#ifdef _DEBUG
		memset(buffer, 0xff, dequeueSize - dirDequeueSize);
#endif // DEBUG
		deqOffset = dequeueSize - dirDequeueSize;
	}

	if (deqOffset == enqOffset) {
		deqOffset = enqOffset = 0;
	}

	return dequeueSize;
}

UINT JBuffer::Peek(OUT BYTE* dest, UINT uiSize)
{
	UINT useSize = GetUseSize();
	UINT peekSize = (useSize < uiSize) ? useSize : uiSize;
	UINT dirDequeueSize = GetDirectDequeueSize();

	if (dirDequeueSize >= peekSize) {
		memcpy(dest, GetDequeueBufferPtr(), peekSize);
	}
	else {
		memcpy(dest, GetDequeueBufferPtr(), dirDequeueSize);
		memcpy(&dest[dirDequeueSize], buffer, peekSize - dirDequeueSize);
	}

	return peekSize;
}

void JBuffer::ClearBuffer(void)
{
	deqOffset = enqOffset = 0;
}

UINT JBuffer::GetDirectEnqueueSize(void)
{
	if (enqOffset >= deqOffset) {
		if (deqOffset == 0) {
			return (capacity - 1) - enqOffset;
		}
		else {
			return (capacity - 1) - enqOffset + 1;
		}
	}
	else {
		return deqOffset - enqOffset;
	}
}

UINT JBuffer::GetDirectDequeueSize(void)
{
	if (enqOffset >= deqOffset) {
		return enqOffset - deqOffset;
	}
	else {
		return capacity - deqOffset;
	}
}

UINT JBuffer::DirectMoveEnqueueOffset(UINT uiSize)
{
	UINT moveCnt = uiSize;
	UINT freeSize = GetFreeSize();
	moveCnt = (freeSize < moveCnt) ? freeSize : moveCnt;
	enqOffset = (enqOffset + moveCnt) % capacity;

	if (deqOffset == enqOffset) {
		deqOffset = enqOffset = 0;
	}

	return moveCnt;
}

UINT JBuffer::DirectMoveDequeueOffset(UINT uiSize)
{
	UINT moveCnt = uiSize;
	UINT useSize = GetUseSize();
	moveCnt = (useSize < moveCnt) ? useSize : moveCnt;
	deqOffset = (deqOffset + moveCnt) % capacity;

	if (deqOffset == enqOffset) {
		deqOffset = enqOffset = 0;
	}

	return moveCnt;
}

BYTE* JBuffer::GetEnqueueBufferPtr(void)
{
	return &buffer[enqOffset];
}

void* JBuffer::GetEnqueueBufferVoidPtr(void)
{
	return reinterpret_cast<void*>(&buffer[enqOffset]);
}

BYTE* JBuffer::GetDequeueBufferPtr(void)
{
	return &buffer[deqOffset];
}

void* JBuffer::GetDequeueBufferVoidPtr(void)
{
	return reinterpret_cast<void*>(&buffer[deqOffset]);
}

