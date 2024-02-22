#pragma once
#include <stdexcept>

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

typedef unsigned char       BYTE;
typedef int                 INT;
typedef unsigned int        UINT;

class JBuffer
{
private:
	UINT		enqOffset;
	UINT		deqOffset;
	BYTE* buffer;
	UINT		capacity;

public:
	JBuffer(UINT capacity);
	~JBuffer();

	inline void Resize(UINT size) {
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

	inline UINT	GetBufferSize(void) const {
		return capacity;
	}
	// 현재 사용중인 용량 얻기.
	inline UINT	GetUseSize(void) const {
		if (enqOffset >= deqOffset) {
			return enqOffset - deqOffset;
		}
		else {
			return (enqOffset)+(capacity - deqOffset);
		}
	}

	// 현재 버퍼에 남은 용량 얻기. 
	inline UINT	GetFreeSize(void) const {
		return (capacity - 1) - GetUseSize();
	}

	// WritePos 에 데이타 넣음.
	// Parameters: (BYTE *)데이타 포인터. (UINT)크기. 
	// Return: (UINT)넣은 크기.
	inline UINT	Enqueue(const BYTE* data, UINT uiSize) {
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


	// ReadPos 에서 데이타 가져옴. ReadPos 이동.
	// Parameters: (BYTE *)데이타 포인터. (UINT)크기.
	// Return: (UINT)가져온 크기.
	inline UINT	Dequeue(BYTE* dest, UINT uiSize) {
		UINT useSize = GetUseSize();
		UINT dequeueSize = (useSize >= uiSize) ? uiSize : useSize;
		UINT dirDequeueSize = GetDirectDequeueSize();

		if (dirDequeueSize >= dequeueSize) {
			memcpy(dest, GetDequeueBufferPtr(), dequeueSize);
			//#ifdef _DEBUG
			//			memset(GetDequeueBufferPtr(), 0xff, dequeueSize);
			//#endif // DEBUG
			deqOffset = (deqOffset + dequeueSize) % capacity;
		}
		else {
			memcpy(dest, GetDequeueBufferPtr(), dirDequeueSize);
			//#ifdef _DEBUG
			//			memset(GetDequeueBufferPtr(), 0xff, dirDequeueSize);
			//#endif // DEBUG
			memcpy(&dest[dirDequeueSize], buffer, dequeueSize - dirDequeueSize);
			//#ifdef _DEBUG
			//			memset(buffer, 0xff, dequeueSize - dirDequeueSize);
			//#endif // DEBUG
			deqOffset = dequeueSize - dirDequeueSize;
		}

#if !defined(JBUFF_DIRPTR_MANUAL_RESET)
		if (deqOffset == enqOffset) {
			deqOffset = enqOffset = 0;
		}
#endif

		return dequeueSize;
	}

	// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
	// Parameters: (void *)데이타 포인터. (uUINT32_t)크기.
	// Return: (UINT)가져온 크기.
	inline UINT	Peek(OUT BYTE* dest, UINT uiSize) {
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
	template<typename T>
	inline UINT	Peek(OUT T* dest) { return Peek(reinterpret_cast<BYTE*>(dest), sizeof(T)); }
	inline bool	Peek(UINT offset, OUT BYTE* dest, UINT uiSize) {
		UINT tempSize = offset + uiSize;
		BYTE* temp = new BYTE[tempSize];
		UINT useSize = GetUseSize();
		if (useSize < tempSize) {
			return false;
		}

		UINT dirDequeueSize = GetDirectDequeueSize();

		if (dirDequeueSize >= tempSize) {
			memcpy(temp, GetDequeueBufferPtr(), tempSize);
		}
		else {
			memcpy(temp, GetDequeueBufferPtr(), dirDequeueSize);
			memcpy(&temp[dirDequeueSize], buffer, tempSize - dirDequeueSize);
		}

		memcpy(dest, &temp[offset], uiSize);

		delete temp;

		return true;
	}


	// 버퍼의 모든 데이타 삭제.
	// Parameters: 없음.
	// Return: 없음.
	inline void	ClearBuffer(void) {
		deqOffset = enqOffset = 0;
	}

	// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이. (끊기지 않은 길이)
	// 원형 큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로 돌아가서
	// 2번에 데이터를 얻거나 넣을 수 있음. 이 부분에서 끊어지지 않은 길이를 의미
	// Parameters: 없음.
	// Return: (UINT)사용가능 용량.
	inline UINT	GetDirectEnqueueSize(void) {
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
	inline UINT	GetDirectDequeueSize(void) {
		if (enqOffset >= deqOffset) {
			return enqOffset - deqOffset;
		}
		else {
			return capacity - deqOffset;
		}
	}
	// 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	// Parameters: 없음.
	// Return: (UINT)이동크기
	inline UINT	DirectMoveEnqueueOffset(UINT uiSize) {
		UINT moveCnt = uiSize;
		UINT freeSize = GetFreeSize();
		moveCnt = (freeSize < moveCnt) ? freeSize : moveCnt;
		enqOffset = (enqOffset + moveCnt) % capacity;

#if !defined(JBUFF_DIRPTR_MANUAL_RESET)
		if (deqOffset == enqOffset) {
			deqOffset = enqOffset = 0;
		}
#else
		if (enqOffset == capacity - 1) {
			enqOffset = 0;
		}
#endif
		return moveCnt;
	}
	inline UINT	DirectMoveDequeueOffset(UINT uiSize) {
		UINT moveCnt = uiSize;
		UINT useSize = GetUseSize();
		moveCnt = (useSize < moveCnt) ? useSize : moveCnt;
		deqOffset = (deqOffset + moveCnt) % capacity;

#if !defined(JBUFF_DIRPTR_MANUAL_RESET)
		if (deqOffset == enqOffset) {
			deqOffset = enqOffset = 0;
		}
#else
		if (deqOffset == capacity - 1) {
			deqOffset = 0;
		}
#endif

		return moveCnt;
	}

	inline BYTE* GetBeginBufferPtr(void) {
		return buffer;
	}

	// 버퍼의 Front 포인터 얻음.
	// Parameters: 없음.
	// Return: (BYTE *) 버퍼 포인터.
	inline BYTE* GetEnqueueBufferPtr(void) {
		return &buffer[enqOffset];
	}

	inline void* GetEnqueueBufferVoidPtr(void) {
		return reinterpret_cast<void*>(&buffer[enqOffset]);
	}

	// 버퍼의 RearPos 포인터 얻음.
	// Parameters: 없음.
	// Return: (BYTE *) 버퍼 포인터.
	inline BYTE* GetDequeueBufferPtr(void) {
		return &buffer[deqOffset];
	}

	inline void* GetDequeueBufferVoidPtr(void) {
		return reinterpret_cast<void*>(&buffer[deqOffset]);
	}


	// [직렬화]
	// 버퍼 디큐 (>>)
	template<typename T>
	inline JBuffer& operator>>(OUT T& dest) {
		if (GetUseSize() >= sizeof(T)) {
			Dequeue(reinterpret_cast<BYTE*>(&dest), sizeof(T));
			return *this;
		}
		else {
			throw std::runtime_error("[ERR] Serialization error : Buffer contains data size smaller than the size of dest type!");
		}
	}
	// 버퍼 인큐 (<<)
	//JBuffer& operator<<(BYTE src);
	//template<typename T>
	//JBuffer& operator<<(T src) {
	//	if (GetFreeSize() >= sizeof(T)) {
	//		Enqueue(reinterpret_cast<const BYTE*>(&src), sizeof(T));
	//	}
	//	else {
	//		throw std::runtime_error("[ERR] Serialization error : Buffer is fulled!");
	//	}
	//}
	template<typename T>
	inline JBuffer& operator<<(const T& src) {
		if (GetFreeSize() >= sizeof(T)) {
			Enqueue(reinterpret_cast<const BYTE*>(&src), sizeof(T));
		}
		else {
			throw std::runtime_error("[ERR] Serialization error : Buffer is fulled!");
		}
	}
	// 버퍼 공간 예약
	// => 링 버퍼 특성상 연속적인 메모리 주소 공간 반환을 보장할 수 없다.
	// => 따라서 DirectReserve 라는 함수를 제공한다. 사용하는 측은 DirectEnqueueSize() 함수를 통해
	//    연속된 예약 공간이 있는지 확인한다.
	template<typename T>
	inline T* DirectReserve() {
		T* ret = nullptr;
		if (GetDirectEnqueueSize() >= sizeof(T)) {
			ret = reinterpret_cast<T*>(GetEnqueueBufferPtr());
			DirectMoveEnqueueOffset(sizeof(T));
		}
		return ret;
	}
};
