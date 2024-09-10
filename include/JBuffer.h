#pragma once
#include <stdexcept>
#include <list>

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
typedef unsigned char       BYTE;
typedef int                 INT;
typedef unsigned int        UINT;

#define JBUFFER_DEFAULT_CAPACITY 5000

class JBuffer
{
protected:
	UINT	m_EnqOffset;
	UINT	m_DeqOffset;
	UINT	m_Capacity;

	BYTE*	m_Buffer;
	UINT	m_InternalCapacity;
	bool	m_IsExternalBuffer;

public:
	//////////////////////////////////////////////////////////////////////////////////////////////
	// [JBuffer ������, �Ҹ���]
	// ������ ȣ�� �� �����ϴ� _capacity ũ�⺸�� ���� ���۴� 1 ����Ʈ �� ũ�� �����ȴ�. 
	// �̴� ���� ������ ����� �������� ���� ���� ��� ũ��, ���� ũ�� ������ ���Ǽ��� ���ؼ��̴�.
	// ���� �ܺο��� ���۸� �����ϴ� ������� �����ϴ� �������� ������ �뷮�� 1 ����Ʈ �� �۴�.
	JBuffer() : JBuffer(JBUFFER_DEFAULT_CAPACITY) {}
	// new ���� �Ҵ��� ���� ���� ���� ����
	JBuffer(UINT capacity) 
		: m_DeqOffset(0), m_EnqOffset(0), m_Capacity(capacity), 
		m_InternalCapacity(m_Capacity + 1), m_IsExternalBuffer(false) 
	{
		m_Buffer = new BYTE[m_InternalCapacity];
	}
	// �ܺο��� �����۰� ������ ���� ���� ����, �⺻ Resize �Լ� ��� �Ұ�
	JBuffer(UINT capacity, BYTE* buffer) : m_DeqOffset(0), m_EnqOffset(0), m_Capacity(capacity - 1), 
		m_InternalCapacity(capacity), m_IsExternalBuffer(true), m_Buffer(buffer) {}
	JBuffer(const JBuffer* jbuff) { *this = *jbuff; }
	~JBuffer() {
		if (!m_IsExternalBuffer) {
			delete[] m_Buffer;
		}
	}
	void Init(UINT _capacity) {
		m_DeqOffset = 0;
		m_EnqOffset = 0;
		m_Capacity = _capacity;
		m_InternalCapacity = m_Capacity + 1;
		m_IsExternalBuffer = false;
		m_Buffer = new BYTE[m_InternalCapacity];
	}
	void Init(UINT _capacity, BYTE* _buffer) {
		m_DeqOffset = 0;
		m_EnqOffset = 0;
		m_Capacity = _capacity - 1;
		m_InternalCapacity = m_Capacity;
		m_IsExternalBuffer = true;
		m_Buffer = _buffer;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// [Resize]
	// new ���� �Ҵ��� ���� ������ ���� ���۸� ���� ���۹��� ��� ���� ũ�� Resizing ����
	// ���� ���� ũ�⺸�� ū ũ��� ������ ����
	//bool Resize(UINT _size);

	////////////////////////////////////////////////////////////////
	// [Clear]
	inline void	ClearBuffer(void) { m_DeqOffset = m_EnqOffset = 0; }

	/////////////////////////////////////////////////////////////////////////////
	// [Slice]
	JBuffer SliceBuffer(UINT length, bool dequeuing) {
		JBuffer sliceBuff(*this);				// shallow copy
		sliceBuff.m_IsExternalBuffer = true;	// has external buffer
		sliceBuff.m_Capacity = length;			
		if (dequeuing) {
			sliceBuff.m_EnqOffset = (m_DeqOffset + length) % m_InternalCapacity;
		}
		return sliceBuff;
	}
	friend JBuffer SliceBuffer(const JBuffer& other, UINT length, bool dequeuing) {
		JBuffer sliceBuff(other);				// shallow copy
		sliceBuff.m_IsExternalBuffer = true;	// has external buffer
		sliceBuff.m_Capacity = length;	
		if (dequeuing) {
			sliceBuff.m_EnqOffset = (other.m_DeqOffset + length) % other.m_InternalCapacity;
		}
		return sliceBuff;
	}

	/////////////////////////////////////////////////////////////////////////////
	// [Getter / Setter]
	UINT GetEnqOffset() { return m_EnqOffset; }
	UINT GetDeqOffset() { return m_DeqOffset; }
	bool SetEnqOffset(UINT offset) {
		if (offset > m_Capacity) { return false; } 
		m_EnqOffset = offset;
		return true;
	}
	bool SetDeqOffset(UINT offset) {
		if (offset > m_Capacity) { return false; }
		m_DeqOffset = offset;
		return true;
	}
	inline BYTE* GetBeginBufferPtr(void) { return m_Buffer; }
	inline BYTE* GetBufferPtr(UINT offset) { return &m_Buffer[offset]; }

	// ���� �뷮, ������� ũ��, ���� ũ�� ����
	// return: capacity�� ���� ������ ����ڰ� ��û�� �뷮���� 1 ����Ʈ ū ��. ���� capacity - 1  �� ��ȯ
	inline UINT	GetBufferSize(void) const { return m_Capacity; }
	// enqueued ������ ũ��
	inline UINT	GetUseSize(void) const {
		if (m_EnqOffset >= m_DeqOffset) { return m_EnqOffset - m_DeqOffset; }
		else { return m_EnqOffset + (m_InternalCapacity - m_DeqOffset); }
	}
	// enqueue ���� ������ ũ��
	inline UINT	GetFreeSize(void) const { return m_Capacity - GetUseSize(); }

	// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����. (������ ���� ����)
	// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
	// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
	inline UINT	GetDirectEnqueueSize(void) {
		if (m_DeqOffset == 0) { return (m_Capacity)-m_EnqOffset; }	// capacity: 7 && endOffset: 7 => enqueue X
		else {														// capacity: 7 && endOffset: 7 => enqueue OK
			if (m_EnqOffset >= m_DeqOffset) { return m_InternalCapacity - m_EnqOffset; }
			else { return m_DeqOffset - m_EnqOffset - 1; }
		}
	}
	inline UINT	GetDirectDequeueSize(void) {
		if (m_EnqOffset >= m_DeqOffset) { return m_EnqOffset - m_DeqOffset; }
		else { return m_InternalCapacity - m_DeqOffset; }
	}
	
	inline BYTE* GetEnqueueBufferPtr(void) { return &m_Buffer[m_EnqOffset]; }
	inline void* GetEnqueueBufferVoidPtr(void) { return reinterpret_cast<void*>(&m_Buffer[m_EnqOffset]); }
	inline BYTE* GetDequeueBufferPtr(void) { return &m_Buffer[m_DeqOffset]; }
	inline void* GetDequeueBufferVoidPtr(void) { return reinterpret_cast<void*>(&m_Buffer[m_DeqOffset]); }


	////////////////////////////////////////////////////////////////////////////////////////
	// [Enqueue]
	// enqueue ��û ũ�Ⱑ �������� ���� ���� ũ�⺸�� ũ�ٸ�, ���� ���� ũ�⸸ �����Ѵ�.
	// params: (BYTE *)����Ÿ ������. (UINT)ũ��. 
	// return: (UINT)���� ũ��.
	inline UINT	Enqueue(const BYTE* data, UINT uiSize) {
		UINT freeSize = GetFreeSize();
		UINT enqueueSize = (freeSize >= uiSize) ? uiSize : freeSize;	// ���� enqueue ũ��
		UINT dirEnqueueSize = GetDirectEnqueueSize();

		if (dirEnqueueSize >= enqueueSize) {
			memcpy(GetEnqueueBufferPtr(), data, enqueueSize);
		}
		else {
			memcpy(GetEnqueueBufferPtr(), data, dirEnqueueSize);
			memcpy(m_Buffer, &data[dirEnqueueSize], enqueueSize - dirEnqueueSize);
		}
		m_EnqOffset = (m_EnqOffset + enqueueSize) % m_InternalCapacity;
		return enqueueSize;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// [Dequeue]
	// dequeue ��û ũ�Ⱑ ���۹��� ���� ��� ũ�⺸�� ũ�ٸ�, ���� ��� ũ�⸸ŭ�� ��ȯ�Ѵ�.
	// params: (BYTE *)����Ÿ ������. (UINT)ũ��.
	// return: (UINT)������ ũ��.
	inline UINT	Dequeue(BYTE* dest, UINT uiSize) {
		UINT useSize = GetUseSize();
		UINT dequeueSize = (useSize >= uiSize) ? uiSize : useSize;
		UINT dirDequeueSize = GetDirectDequeueSize();

		if (dirDequeueSize >= dequeueSize) {
			memcpy(dest, GetDequeueBufferPtr(), dequeueSize);
		}
		else {
			memcpy(dest, GetDequeueBufferPtr(), dirDequeueSize);
			memcpy(&dest[dirDequeueSize], m_Buffer, dequeueSize - dirDequeueSize);
		}
		m_DeqOffset = (m_DeqOffset + dequeueSize) % m_InternalCapacity;

#if !defined(JBUFF_DIRPTR_MANUAL_RESET)
		if (m_DeqOffset == m_EnqOffset) {
			m_DeqOffset = m_EnqOffset = 0;
		}
#endif
		return m_DeqOffset;
	}

	/////////////////////////////////////////////////////////////////
	// [Enqueue/Dequeue ������ ���� �̵�]
	// ���� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
	// return: (UINT)�̵�ũ��
	inline UINT	DirectMoveEnqueueOffset(UINT uiSize) {
		//UINT moveCnt = uiSize;
		UINT freeSize = GetFreeSize();
		UINT moveSize = (freeSize >= uiSize) ? uiSize : freeSize;
		m_EnqOffset = (m_EnqOffset + moveSize) % m_InternalCapacity;
		return moveSize;
	}
	inline UINT	DirectMoveDequeueOffset(UINT uiSize) {
		UINT useSize = GetUseSize();
		UINT moveSize = (useSize >= uiSize) ? uiSize : useSize;
		m_DeqOffset = (m_DeqOffset + moveSize) % m_InternalCapacity;
#if !defined(JBUFF_DIRPTR_MANUAL_RESET)
		if (m_DeqOffset == m_EnqOffset) {
			m_DeqOffset = m_EnqOffset = 0;
		}
#endif
		return moveSize;
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// [Peek]
	// ReadPos ���� ����Ÿ �о��. ReadPos ����.
	// params: (void *)����Ÿ ������. (uUINT32_t)ũ��.
	// return: (UINT)������ ũ��.
	inline UINT	Peek(OUT BYTE* dest, UINT uiSize) {
		UINT useSize = GetUseSize();
		UINT peekSize = (useSize < uiSize) ? useSize : uiSize;
		UINT dirDequeueSize = GetDirectDequeueSize();

		if (dirDequeueSize >= peekSize) {
			memcpy(dest, GetDequeueBufferPtr(), peekSize);
		}
		else {
			memcpy(dest, GetDequeueBufferPtr(), dirDequeueSize);
			memcpy(&dest[dirDequeueSize], m_Buffer, peekSize - dirDequeueSize);
		}
		return peekSize;
	}
	inline bool	Peek(UINT offset, OUT BYTE* dest, UINT uiSize) {
		UINT useSize = GetUseSize();
		UINT neededSize = offset + uiSize;
		if (useSize < neededSize) { return false; }

		UINT dirDequeueSize = GetDirectDequeueSize();
		if (dirDequeueSize < offset) {
			memcpy(dest, m_Buffer + (offset - dirDequeueSize), uiSize);
		}
		else if(dirDequeueSize == offset) {
			memcpy(dest, m_Buffer, uiSize);
		}
		else {
			BYTE* deqPtr = GetDequeueBufferPtr();
			if (dirDequeueSize >= neededSize) {
				memcpy(dest, deqPtr + offset, uiSize);
			}
			else {
				size_t copySize = dirDequeueSize - offset;
				memcpy(dest, deqPtr + offset, copySize);
				memcpy(dest + copySize, m_Buffer, uiSize - copySize);
			}
		}

		return true;
	}
	template<typename T>
	inline UINT	Peek(OUT T* dest) { return Peek(reinterpret_cast<BYTE*>(dest), sizeof(T)); }
	template<typename T>
	inline UINT	Peek(UINT offset, OUT T* dest) { return Peek(offset, reinterpret_cast<BYTE*>(dest), sizeof(T)); }


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [Serialization]
	// Dequeue (>>)
	template<typename T>
	inline JBuffer& operator>>(OUT T& dest) {
		if (GetUseSize() >= sizeof(T)) {
			Dequeue(reinterpret_cast<BYTE*>(&dest), sizeof(T));
			return *this;
		}
		else { throw std::runtime_error("[ERR] Serialization error : Buffer contains data size smaller than the size of dest type!"); }

		return *this;
	}
	// Enqueue (<<)
	template<typename T>
	inline JBuffer& operator<<(const T& src) {
		if (GetFreeSize() >= sizeof(T)) {
			Enqueue(reinterpret_cast<const BYTE*>(&src), sizeof(T));
		}
		else { throw std::runtime_error("[ERR] Serialization error : Buffer is fulled!"); }

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [Buffer Reservation]
	// �� ���� Ư���� �������� �޸� �ּ� ���� ��ȯ�� ������ �� ����.
	// ���� DirectReserve ��� �Լ��� �����Ѵ�. ����ϴ� ���� DirectEnqueueSize() �Լ��� ���� ���ӵ� ���� ������ �ִ��� Ȯ���Ѵ�.
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

class JSerialBuffer {
private:
	std::list<JBuffer> m_Buffers;
	std::list<JBuffer>::iterator m_EnqBufferIter;
	UINT				m_Capacity = 0;
	UINT				m_UsedSize = 0;
public:
	JSerialBuffer() 
		: m_EnqBufferIter(m_Buffers.end()), m_Capacity(0), m_UsedSize(0)
	{}
	inline UINT GetCapacity() { return m_Capacity; }
	inline UINT	GetUseSize() { return m_UsedSize; }

	inline bool Serialize(JBuffer& srcBuff, UINT length, bool dequeuing = false) {
		bool ret = false;
		JBuffer buff = SliceBuffer(srcBuff, length, dequeuing);

		if (dequeuing) {
			// ���� ���� ���۰� ��ť ���� ���¶�� �߰� �Ұ�
			if (m_EnqBufferIter == m_Buffers.end()) {
				m_Buffers.push_back(buff);
				m_UsedSize += length;
				m_Capacity += length;
				ret = true;
			}
		}
		else {
			m_Buffers.push_back(buff);
			m_Capacity += length;

			// ��ť�� ���� ���� ù �߰�
			if (m_EnqBufferIter == m_Buffers.end()) {
				m_EnqBufferIter = m_Buffers.begin();
			}
			ret = true;
		}

		return ret;
	}

	inline bool Enqueue(BYTE* src, UINT length) {
		BYTE* srcPtr = src;
		UINT srcLength = length;
		
		bool done = false;
		while (m_EnqBufferIter != m_Buffers.end()) {
			JBuffer& buff = *m_EnqBufferIter;
			UINT freeSize = buff.GetFreeSize();
			if (freeSize >= srcLength) {
				buff.Enqueue(srcPtr, srcLength);
				m_UsedSize += srcLength;
				if (freeSize == srcLength) {
					m_EnqBufferIter++;
				}

				done = true;
				break;
			}
			else {
				buff.Enqueue(srcPtr, freeSize);
				m_UsedSize += freeSize;
				srcPtr += freeSize;
				srcLength -= freeSize;
				m_EnqBufferIter++;
			}
		}

		return done;
	}

	// ����, dest �޸� ������ �ݵ�� length���� Ŀ����. �����÷ο츦 ���� �� ����
	inline bool Dequeue(OUT BYTE* dest, UINT length) {
		BYTE* destPtr = dest;
		size_t destLength = length;

		bool done = false;
		std::list<JBuffer>::iterator iter;
		while (true) {
			iter = m_Buffers.begin();
			if (iter == m_Buffers.end()) {
				break;
			}

			JBuffer& buff = *iter;
			UINT usedSize = buff.GetUseSize();
			if (usedSize > destLength) {
				buff.Dequeue(destPtr, destLength);
				m_UsedSize -= destLength;
				m_Capacity -= destLength;

				done = true;
				break;
			}
			else {
				buff.Dequeue(destPtr, usedSize);
				m_UsedSize -= usedSize;
				m_Capacity -= usedSize;
				destPtr += usedSize;
				destLength -= usedSize;

				if (iter == m_EnqBufferIter) {
					break;
				}
				else {
					m_Buffers.pop_front();
				}
			}
		}

		return done;
	}

	template<typename T>
	inline JSerialBuffer& operator<<(T& src) {
		if (m_Capacity - m_UsedSize < sizeof(T)) {
			throw std::runtime_error("[ERR] JSerialBuffer::operator<< Fail, capacity shortage");
		}
		if (!Enqueue(&src, sizeof(T))) {
			throw std::runtime_error("[ERR] JSerialBuffer::operator<< Fail, capacity shortage");
		}
		return *this;
	}

	template<typename T>
	inline JSerialBuffer& operator>>(OUT T& dest) {
		if (m_UsedSize > sizeof(T)) {
			throw std::runtime_error("[ERR] JSerialBuffer::operator>> Fail, used size shortage");
		}
		if (!Dequeue(&dest, sizeof(T))) {
			throw std::runtime_error("[ERR] JSerialBuffer::operator>> Fail, used size shortage");
		}
		return *this;
	}

	inline bool Peek(OUT BYTE* dest, UINT length) {
		BYTE* destPtr = dest;
		size_t destLength = length;

		bool done = false;
		std::list<JBuffer>::iterator iter = m_Buffers.begin();
		while (iter != m_Buffers.end()) {
			JBuffer& buff = *iter;
			UINT usedSize = buff.GetUseSize();
			if (usedSize > destLength) {
				buff.Peek(destPtr, destLength);
				done = true;
				break;
			}
			else {
				buff.Peek(destPtr, usedSize);
				destPtr += usedSize;
				destLength -= usedSize;

				if (iter == m_EnqBufferIter) break;
			}
			iter++;
		}
		return done;
	}
};