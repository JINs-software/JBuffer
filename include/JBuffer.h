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
	// ���� ������� �뷮 ���.
	inline UINT	GetUseSize(void) const {
		if (enqOffset >= deqOffset) {
			return enqOffset - deqOffset;
		}
		else {
			return (enqOffset)+(capacity - deqOffset);
		}
	}

	// ���� ���ۿ� ���� �뷮 ���. 
	inline UINT	GetFreeSize(void) const {
		return (capacity - 1) - GetUseSize();
	}

	// WritePos �� ����Ÿ ����.
	// Parameters: (BYTE *)����Ÿ ������. (UINT)ũ��. 
	// Return: (UINT)���� ũ��.
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


	// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
	// Parameters: (BYTE *)����Ÿ ������. (UINT)ũ��.
	// Return: (UINT)������ ũ��.
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

	// ReadPos ���� ����Ÿ �о��. ReadPos ����.
	// Parameters: (void *)����Ÿ ������. (uUINT32_t)ũ��.
	// Return: (UINT)������ ũ��.
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


	// ������ ��� ����Ÿ ����.
	// Parameters: ����.
	// Return: ����.
	inline void	ClearBuffer(void) {
		deqOffset = enqOffset = 0;
	}

	// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����. (������ ���� ����)
	// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
	// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
	// Parameters: ����.
	// Return: (UINT)��밡�� �뷮.
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
	// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
	// Parameters: ����.
	// Return: (UINT)�̵�ũ��
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

	// ������ Front ������ ����.
	// Parameters: ����.
	// Return: (BYTE *) ���� ������.
	inline BYTE* GetEnqueueBufferPtr(void) {
		return &buffer[enqOffset];
	}

	inline void* GetEnqueueBufferVoidPtr(void) {
		return reinterpret_cast<void*>(&buffer[enqOffset]);
	}

	// ������ RearPos ������ ����.
	// Parameters: ����.
	// Return: (BYTE *) ���� ������.
	inline BYTE* GetDequeueBufferPtr(void) {
		return &buffer[deqOffset];
	}

	inline void* GetDequeueBufferVoidPtr(void) {
		return reinterpret_cast<void*>(&buffer[deqOffset]);
	}


	// [����ȭ]
	// ���� ��ť (>>)
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
	// ���� ��ť (<<)
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
	// ���� ���� ����
	// => �� ���� Ư���� �������� �޸� �ּ� ���� ��ȯ�� ������ �� ����.
	// => ���� DirectReserve ��� �Լ��� �����Ѵ�. ����ϴ� ���� DirectEnqueueSize() �Լ��� ����
	//    ���ӵ� ���� ������ �ִ��� Ȯ���Ѵ�.
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
