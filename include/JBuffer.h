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
	UINT	enqOffset;
	UINT	deqOffset;
	UINT	capacity;

	BYTE*	buffer;
	bool	isExternalBuffer = false;

public:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [JBuffer 생성자, 소멸자]
	// 
	// 생성자 호출 시 전달하는 _capacity 크기보다 내부 버퍼는 1 바이트 더 크게 생성된다. 
	// 이는 내부 오프셋 제어와 오프셋을 통한 버퍼 사용 크기, 여분 크기 산출의 편의성을 위해서이다.
	// 따라서 외부에서 버퍼를 주입하는 방식으로 생성하는 링버퍼의 실질적 용량은 1 바이트 더 작다.
	JBuffer(UINT _capacity);					// new 동적 할당을 통해 내부 버퍼 생성
	JBuffer(UINT _capacity, BYTE* _buffer);		// 외부에서 링버퍼가 랩핑할 내부 버퍼 주입, 기본 Resize 함수 사용 불가
	~JBuffer();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////
	// [Resize]
	// 
	// new 동적 할당을 통해 생성된 내부 버퍼를 가진 링퍼버의 경우 버퍼 크기 Resizing 가능
	// 기존 버퍼 크기보다 큰 크기로 사이즈 변경
	bool Resize(UINT _size);
	////////////////////////////////////////////////////////////////////////////////////////

	// 버퍼의 모든 데이타 삭제.
	// Parameters: 없음.
	// Return: 없음.
	inline void	ClearBuffer(void) {
		deqOffset = enqOffset = 0;
	}

	inline BYTE* GetBeginBufferPtr(void) {
		return buffer;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// [버퍼 용량, 사용중인 크기, 여분 크기 산출]
	//
	// - Return: capacity는 생성 시점에 사용자가 요청한 용량보다 1 바이트 큰 값이다.
	//			따라서 capacity - 1  을 반환한다.
	inline UINT	GetBufferSize(void) const {
		return capacity;
	}
	////////////////////////////////////////////////////////////////////////////////////////
	// - Desc: 현재 사용중인 용량 얻기.
	inline UINT	GetUseSize(void) const {
		if (enqOffset >= deqOffset) {
			return enqOffset - deqOffset;
		}
		else {
			return enqOffset + (capacity - deqOffset);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
	// 현재 버퍼에 남은 용량 얻기. 
	inline UINT	GetFreeSize(void) const {
		return GetBufferSize() - GetUseSize();
	}
	////////////////////////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////////////////////////////
	// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이. (끊기지 않은 길이)
	// 원형 큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로 돌아가서
	// 2번에 데이터를 얻거나 넣을 수 있음. 이 부분에서 끊어지지 않은 길이를 의미
	// 
	// - Parameters: 없음.
	// - Return: (UINT)사용가능 용량.
	inline UINT	GetDirectEnqueueSize(void) {
		if (enqOffset >= deqOffset) {
			return capacity - enqOffset;
		}
		else {
			return deqOffset - enqOffset;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
	inline UINT	GetDirectDequeueSize(void) {
		if (enqOffset >= deqOffset) {
			return enqOffset - deqOffset;
		}
		else {
			return capacity - deqOffset;
		}
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


	////////////////////////////////////////////////////////////////////////////////////////
	// [Enqueue]
	//
	// - Desc: enqueue 요청 크기가 링버퍼의 남은 여분 크기보다 크다면, 남은 여분 크기만 삽입한다.
	// - Parameters: (BYTE *)데이타 포인터. (UINT)크기. 
	// - Return: (UINT)넣은 크기.
	inline UINT	Enqueue(const BYTE* data, UINT uiSize) {
		UINT freeSize = GetFreeSize();
		UINT enqueueSize = (freeSize >= uiSize) ? uiSize : freeSize;	// 실 enqueue 크기
		UINT dirEnqueueSize = GetDirectEnqueueSize();

		if (dirEnqueueSize >= enqueueSize) {
			memcpy(GetEnqueueBufferPtr(), data, enqueueSize);
			enqOffset = (enqOffset + enqueueSize) % (capacity + 1);
			if (enqOffset == capacity && deqOffset != 0) {
				enqOffset = 0;
			}
		}
		else {
			memcpy(GetEnqueueBufferPtr(), data, dirEnqueueSize);
			memcpy(buffer, &data[dirEnqueueSize], enqueueSize - dirEnqueueSize);
			enqOffset = enqueueSize - dirEnqueueSize;
		}

		return enqueueSize;
	}
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// [Dequeue]
	// 
	// - Desc: dequeue 요청 크기가 링퍼버의 실제 사용 크기보다 크다면, 실제 사용 크기만큼만 반환한다.
	// - Parameters: (BYTE *)데이타 포인터. (UINT)크기.
	// - Return: (UINT)가져온 크기.
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
			if (deqOffset == 0 && enqOffset == capacity) {
				enqOffset = 0;
			}
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
#else
		//if (enqOffset == deqOffset && enqOffset == capacity) {
		//	enqOffset = deqOffset = 0;
		//}
#endif

		return dequeueSize;
	}
	////////////////////////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////////////////////////////
	// [Enqueue/Dequeue 포인터 직접 이동]
	//
	// - Desc: 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	// - Parameters: 없음.
	// - Return: (UINT)이동크기
	inline UINT	DirectMoveEnqueueOffset(UINT uiSize) {
		//UINT moveCnt = uiSize;
		UINT freeSize = GetFreeSize();
		UINT moveSize = (freeSize >= uiSize) ? uiSize : freeSize;
		UINT dirEnqueueSize = GetDirectEnqueueSize();

		if (dirEnqueueSize >= moveSize) {
			enqOffset = (enqOffset + moveSize) % (capacity + 1);
			if (enqOffset == capacity && deqOffset != 0) {
				enqOffset = 0;
			}
		}
		else {
			enqOffset = moveSize - dirEnqueueSize;
		}

		return moveSize;
	}
	inline UINT	DirectMoveDequeueOffset(UINT uiSize) {
		UINT useSize = GetUseSize();
		UINT moveSize = (useSize >= uiSize) ? uiSize : useSize;
		UINT dirDequeueSize = GetDirectDequeueSize();

		if (dirDequeueSize >= moveSize) {
			deqOffset = (deqOffset + moveSize) % capacity;
			if (deqOffset == 0 && enqOffset == capacity) {
				enqOffset = 0;
			}
		}
		else {
			deqOffset = moveSize - dirDequeueSize;
		}

#if !defined(JBUFF_DIRPTR_MANUAL_RESET)
		if (deqOffset == enqOffset) {
			deqOffset = enqOffset = 0;
		}
#else 
		//if (enqOffset == deqOffset && enqOffset == capacity) {
		//	enqOffset = deqOffset = 0;
		//}
#endif

		return moveSize;
	}
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// [Peek]
	// 
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
		UINT useSize = GetUseSize();
		UINT neededSize = offset + uiSize;
		if (useSize < neededSize) {
			return false;
		}

		UINT dirDequeueSize = GetDirectDequeueSize();
		if (dirDequeueSize < offset) {
			memcpy(dest, buffer + (offset - dirDequeueSize), uiSize);
		}
		else if(dirDequeueSize == offset) {
			memcpy(dest, buffer, uiSize);
		}
		else {
			BYTE* deqPtr = GetDequeueBufferPtr();
			if (dirDequeueSize >= neededSize) {
				memcpy(dest, deqPtr + offset, uiSize);
			}
			else {
				size_t copySize = dirDequeueSize - offset;
				memcpy(dest, deqPtr + offset, copySize);
				memcpy(dest + copySize, buffer, uiSize - copySize);
			}
		}

		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////////



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
