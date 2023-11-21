#include "RingBuffer.h"

RingBuffer::RingBuffer(int capacity)
{
	frontOffset = endOffset = 0;
	this->capacity = capacity + 1;
	buffer = new char[this->capacity];
#ifdef _DEBUG
	memset(buffer, 0xff, this->capacity);
#endif // DEBUG

}

RingBuffer::~RingBuffer()
{
	delete[] buffer;
}

void RingBuffer::Resize(int size)
{
	if (capacity - 1 < size) {
		char* newBuffer = new char[size + 1];

		if (endOffset >= frontOffset) {
			memcpy(newBuffer, &buffer[frontOffset], endOffset - frontOffset);
		}
		else {
			memcpy(newBuffer, &buffer[frontOffset], capacity - frontOffset);
			memcpy(&newBuffer[capacity - frontOffset], buffer, endOffset);
		}

		int useSize = GetUseSize();
		frontOffset = 0;
		endOffset = useSize;
		capacity = size + 1;
		delete[] buffer;
		buffer = newBuffer;
	}
}

int RingBuffer::GetBufferSize(void)
{
	return capacity;
}

int RingBuffer::GetUseSize(void)
{
	if (endOffset >= frontOffset) {
		return endOffset - frontOffset;
	}
	else {
		return (endOffset) + (capacity - frontOffset);
	}
}

int RingBuffer::GetFreeSize(void)
{
	return (capacity - 1) - GetUseSize();
}

int RingBuffer::Enqueue(char* chpData, int iSize)
{
	int freeSize = GetFreeSize();
	int enqueueSize = (freeSize >= iSize) ? iSize : freeSize;
	int dirEnqueueSize = DirectEnqueueSize();

	if (dirEnqueueSize >= enqueueSize) {
		memcpy(GetEnqueueBufferPtr(), chpData, enqueueSize);
		endOffset = (endOffset + enqueueSize) % capacity;
	}
	else {
		memcpy(GetEnqueueBufferPtr(), chpData, dirEnqueueSize);
		memcpy(buffer, &chpData[dirEnqueueSize], enqueueSize - dirEnqueueSize);
		endOffset = enqueueSize - dirEnqueueSize;
	}

	return enqueueSize;
}

int RingBuffer::Dequeue(char* chpDest, int iSize)
{
	int useSize = GetUseSize();
	int dequeueSize = (useSize >= iSize) ? iSize : useSize;
	int dirDequeueSize = DirectDequeueSize();

	if (dirDequeueSize >= dequeueSize) {
		memcpy(chpDest, GetDequeueBufferPtr(), dequeueSize);
#ifdef _DEBUG
		memset(GetDequeueBufferPtr(), 0xff, dequeueSize);
#endif // DEBUG
		frontOffset = (frontOffset + dequeueSize) % capacity;
	}
	else {
		memcpy(chpDest, GetDequeueBufferPtr(), dirDequeueSize);
#ifdef _DEBUG
		memset(GetDequeueBufferPtr(), 0xff, dirDequeueSize);
#endif // DEBUG
		memcpy(&chpDest[dirDequeueSize], buffer, dequeueSize - dirDequeueSize);
#ifdef _DEBUG
		memset(buffer, 0xff, dequeueSize - dirDequeueSize);
#endif // DEBUG
		frontOffset = dequeueSize - dirDequeueSize;
	}

	if (frontOffset == endOffset) {
		frontOffset = endOffset = 0;
	}

	return dequeueSize;
}

int RingBuffer::Peek(char* chpDest, int iSize)
{
	int useSize = GetUseSize();
	int peekSize = (useSize < iSize) ? useSize : iSize;
	int dirDequeueSize = DirectDequeueSize();

	if (dirDequeueSize >= peekSize) {
		memcpy(chpDest, GetDequeueBufferPtr(), peekSize);
	}
	else {
		memcpy(chpDest, GetDequeueBufferPtr(), dirDequeueSize);
		memcpy(&chpDest[dirDequeueSize], buffer, peekSize - dirDequeueSize);
	}

	return peekSize;
}

void RingBuffer::ClearBuffer(void)
{
	frontOffset = endOffset = 0;
}

int RingBuffer::DirectEnqueueSize(void)
{
	if (endOffset >= frontOffset) {
		if (frontOffset == 0) {
			return (capacity - 1) - endOffset;
		}
		else {
			return (capacity - 1) - endOffset + 1;
		}
	}
	else {
		return frontOffset - endOffset;
	}
}

int RingBuffer::DirectDequeueSize(void)
{
	if (endOffset >= frontOffset) {
		return endOffset - frontOffset;
	}
	else {
		return capacity - frontOffset;
	}
}

int RingBuffer::DirectMoveEnd(int iSize)
{
	int moveCnt = iSize;
	int freeSize = GetFreeSize();
	moveCnt = (freeSize < moveCnt) ? freeSize : moveCnt;
	endOffset = (endOffset + moveCnt) % capacity;

	if (frontOffset == endOffset) {
		frontOffset = endOffset = 0;
	}

	return moveCnt;
}

int RingBuffer::DirectMoveFront(int iSize)
{
	int moveCnt = iSize;
	int useSize = GetUseSize();
	moveCnt = (useSize < moveCnt) ? useSize : moveCnt;
	frontOffset = (frontOffset + moveCnt) % capacity;

	if (frontOffset == endOffset) {
		frontOffset = endOffset = 0;
	}

	return moveCnt;
}

char* RingBuffer::GetEnqueueBufferPtr(void)
{
	return &buffer[endOffset];
}

char* RingBuffer::GetDequeueBufferPtr(void)
{
	return &buffer[frontOffset];
}

