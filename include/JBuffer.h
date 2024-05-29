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

#define JBUFFER_DEFAULT_CAPACITY 10000

class JBuffer
{
protected:
	UINT	enqOffset;
	UINT	deqOffset;
	UINT	capacity;

	BYTE*	buffer;
	UINT	internalCapacity;
	bool	isExternalBuffer;

public:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [JBuffer ������, �Ҹ���]
	// 
	// ������ ȣ�� �� �����ϴ� _capacity ũ�⺸�� ���� ���۴� 1 ����Ʈ �� ũ�� �����ȴ�. 
	// �̴� ���� ������ ����� �������� ���� ���� ��� ũ��, ���� ũ�� ������ ���Ǽ��� ���ؼ��̴�.
	// ���� �ܺο��� ���۸� �����ϴ� ������� �����ϴ� �������� ������ �뷮�� 1 ����Ʈ �� �۴�.
	JBuffer();
	JBuffer(UINT _capacity);					// new ���� �Ҵ��� ���� ���� ���� ����
	JBuffer(UINT _capacity, BYTE* _buffer);		// �ܺο��� �����۰� ������ ���� ���� ����, �⺻ Resize �Լ� ��� �Ұ�
	JBuffer(JBuffer* jbuff);
	~JBuffer();

	void Init(UINT _capacity);
	void Init(UINT _capacity, BYTE* _buffer);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	UINT GetEnqOffset() {
		return enqOffset;
	}
	UINT GetDeqOffset() {
		return deqOffset;
	}
	bool SetEnqOffset(UINT offset) {
		if (offset > capacity) {
			return false;
		}

		enqOffset = offset;
		return true;
	}
	bool SetDeqOffset(UINT offset) {
		if (offset > capacity) {
			return false;
		}

		deqOffset = offset;
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// [Resize]
	// 
	// new ���� �Ҵ��� ���� ������ ���� ���۸� ���� ���۹��� ��� ���� ũ�� Resizing ����
	// ���� ���� ũ�⺸�� ū ũ��� ������ ����
	bool Resize(UINT _size);
	////////////////////////////////////////////////////////////////////////////////////////

	// ������ ��� ����Ÿ ����.
	// Parameters: ����.
	// Return: ����.
	inline void	ClearBuffer(void) {
		deqOffset = enqOffset = 0;
	}

	inline BYTE* GetBeginBufferPtr(void) {
		return buffer;
	}
	inline BYTE* GetBufferPtr(UINT offset) {
		return &buffer[offset];
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// [���� �뷮, ������� ũ��, ���� ũ�� ����]
	//
	// - Return: capacity�� ���� ������ ����ڰ� ��û�� �뷮���� 1 ����Ʈ ū ���̴�.
	//			���� capacity - 1  �� ��ȯ�Ѵ�.
	inline UINT	GetBufferSize(void) const {
		return capacity;
	}
	////////////////////////////////////////////////////////////////////////////////////////
	// - Desc: ���� ������� �뷮 ���.
	inline UINT	GetUseSize(void) const {
		if (enqOffset >= deqOffset) {
			return enqOffset - deqOffset;
		}
		else {
			//return enqOffset + (capacity - deqOffset);
			return enqOffset + (internalCapacity - deqOffset);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
	// ���� ���ۿ� ���� �뷮 ���. 
	inline UINT	GetFreeSize(void) const {
		return capacity - GetUseSize();
	}
	////////////////////////////////////////////////////////////////////////////////////////

	


	////////////////////////////////////////////////////////////////////////////////////////
	// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����. (������ ���� ����)
	// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
	// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
	// 
	// - Parameters: ����.
	// - Return: (UINT)��밡�� �뷮.
	inline UINT	GetDirectEnqueueSize(void) {
		if (deqOffset == 0) {
			return (internalCapacity - 1) - enqOffset;
		}
		else {
			if (enqOffset >= deqOffset) {
				//return capacity - enqOffset;
				return internalCapacity - enqOffset;
			}
			else {
				return deqOffset - enqOffset - 1;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
	inline UINT	GetDirectDequeueSize(void) {
		if (enqOffset >= deqOffset) {
			return enqOffset - deqOffset;
		}
		else {
			return internalCapacity - deqOffset;
		}
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


	////////////////////////////////////////////////////////////////////////////////////////
	// [Enqueue]
	//
	// - Desc: enqueue ��û ũ�Ⱑ �������� ���� ���� ũ�⺸�� ũ�ٸ�, ���� ���� ũ�⸸ �����Ѵ�.
	// - Parameters: (BYTE *)����Ÿ ������. (UINT)ũ��. 
	// - Return: (UINT)���� ũ��.
	inline UINT	Enqueue(const BYTE* data, UINT uiSize) {
		UINT freeSize = GetFreeSize();
		UINT enqueueSize = (freeSize >= uiSize) ? uiSize : freeSize;	// �� enqueue ũ��
		UINT dirEnqueueSize = GetDirectEnqueueSize();

		//if (dirEnqueueSize >= enqueueSize) {
		//	memcpy(GetEnqueueBufferPtr(), data, enqueueSize);
		//	enqOffset = (enqOffset + enqueueSize) % (capacity + 1);
		//	if (enqOffset == capacity && deqOffset != 0) {
		//		enqOffset = 0;
		//	}
		//}
		//else {
		//	memcpy(GetEnqueueBufferPtr(), data, dirEnqueueSize);
		//	memcpy(buffer, &data[dirEnqueueSize], enqueueSize - dirEnqueueSize);
		//	enqOffset = enqueueSize - dirEnqueueSize;
		//	if (enqOffset == 0) {
		//		deqOffset = 0;
		//	}
		//}

		if (dirEnqueueSize >= enqueueSize) {
			memcpy(GetEnqueueBufferPtr(), data, enqueueSize);
		}
		else {
			memcpy(GetEnqueueBufferPtr(), data, dirEnqueueSize);
			memcpy(buffer, &data[dirEnqueueSize], enqueueSize - dirEnqueueSize);
		}
		enqOffset = (enqOffset + enqueueSize) % internalCapacity;

		return enqueueSize;
	}
	////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////
	// [Dequeue]
	// 
	// - Desc: dequeue ��û ũ�Ⱑ ���۹��� ���� ��� ũ�⺸�� ũ�ٸ�, ���� ��� ũ�⸸ŭ�� ��ȯ�Ѵ�.
	// - Parameters: (BYTE *)����Ÿ ������. (UINT)ũ��.
	// - Return: (UINT)������ ũ��.
	inline UINT	Dequeue(BYTE* dest, UINT uiSize) {
		UINT useSize = GetUseSize();
		UINT dequeueSize = (useSize >= uiSize) ? uiSize : useSize;
		UINT dirDequeueSize = GetDirectDequeueSize();

		//if (dirDequeueSize >= moveSize) {
		//	memcpy(dest, GetDequeueBufferPtr(), moveSize);
		//	//#ifdef _DEBUG
		//	//			memset(GetDequeueBufferPtr(), 0xff, dequeueSize);
		//	//#endif // DEBUG
		//	
		//	deqOffset = (deqOffset + moveSize) % (capacity + 1);
		//	if (deqOffset == capacity) {
		//		if (enqOffset == capacity) {
		//			enqOffset = 0;
		//		}
		//		deqOffset = 0;
		//	}
		//	//if (deqOffset == capacity && enqOffset == capacity) {
		//	//	enqOffset = 0;
		//	//	deqOffset = 0;
		//	//}
		//}
		//else {
		//	memcpy(dest, GetDequeueBufferPtr(), dirDequeueSize);
		//	//#ifdef _DEBUG
		//	//			memset(GetDequeueBufferPtr(), 0xff, dirDequeueSize);
		//	//#endif // DEBUG
		//	memcpy(&dest[dirDequeueSize], buffer, moveSize - dirDequeueSize);
		//	//#ifdef _DEBUG
		//	//			memset(buffer, 0xff, dequeueSize - dirDequeueSize);
		//	//#endif // DEBUG
		//	deqOffset = moveSize - dirDequeueSize;
		//	//if (deqOffset == 0) {
		//	//	enqOffset = 0;
		//	//}
		//}

		if (dirDequeueSize >= dequeueSize) {
			memcpy(dest, GetDequeueBufferPtr(), dequeueSize);
		}
		else {
			memcpy(dest, GetDequeueBufferPtr(), dirDequeueSize);
			memcpy(&dest[dirDequeueSize], buffer, dequeueSize - dirDequeueSize);
		}
		deqOffset = (deqOffset + dequeueSize) % internalCapacity;

#if !defined(JBUFF_DIRPTR_MANUAL_RESET)
		if (deqOffset == enqOffset) {
			deqOffset = enqOffset = 0;
		}
#else
		//if (enqOffset == deqOffset && enqOffset == capacity) {
		//	enqOffset = deqOffset = 0;
		//}
#endif

		return deqOffset;
	}
	////////////////////////////////////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////////////////////////////////////
	// [Enqueue/Dequeue ������ ���� �̵�]
	//
	// - Desc: ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
	// - Parameters: ����.
	// - Return: (UINT)�̵�ũ��
	inline UINT	DirectMoveEnqueueOffset(UINT uiSize) {
		//UINT moveCnt = uiSize;
		UINT freeSize = GetFreeSize();
		UINT moveSize = (freeSize >= uiSize) ? uiSize : freeSize;
		UINT dirEnqueueSize = GetDirectEnqueueSize();

		//if (dirEnqueueSize >= moveSize) {
		//	enqOffset = (enqOffset + moveSize) % (capacity + 1);
		//	if (enqOffset == capacity && deqOffset != 0) {
		//		enqOffset = 0;
		//	}
		//}
		//else {
		//	enqOffset = moveSize - dirEnqueueSize;
		//	if (enqOffset == 0) {
		//		deqOffset = 0;
		//	}
		//}

		enqOffset = (enqOffset + moveSize) % internalCapacity;

		return moveSize;
	}
	inline UINT	DirectMoveDequeueOffset(UINT uiSize) {
		UINT useSize = GetUseSize();
		UINT moveSize = (useSize >= uiSize) ? uiSize : useSize;
		UINT dirDequeueSize = GetDirectDequeueSize();

		//if (dirDequeueSize >= moveSize) {
		//	deqOffset = (deqOffset + moveSize) % (capacity + 1);
		//	if (deqOffset == capacity) {
		//		if (enqOffset == capacity) {
		//			enqOffset = 0;
		//		}
		//		deqOffset = 0;
		//	}
		//	//if (deqOffset == capacity && enqOffset == capacity) {
		//	//	enqOffset = 0;
		//	//	deqOffset = 0;
		//	//}
		//}
		//else {
		//	deqOffset = moveSize - dirDequeueSize;				// <------------- ����
		//	//if (deqOffset == 0) {
		//	//	enqOffset = 0;
		//	//}
		//}

		deqOffset = (deqOffset + moveSize) % internalCapacity;

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

		return *this;
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

		return *this;
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

class JSerBuffer : public JBuffer
{
public:
	JSerBuffer(JBuffer& ringbuff, UINT length, bool isFull = false)
		: JBuffer(ringbuff)
	{
		isExternalBuffer = true;
		if (isFull) {
			enqOffset = (deqOffset + length) % internalCapacity;
		}
	}
};