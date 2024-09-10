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
	// [JBuffer 생성자, 소멸자]
	// 생성자 호출 시 전달하는 _capacity 크기보다 내부 버퍼는 1 바이트 더 크게 생성된다. 
	// 이는 내부 오프셋 제어와 오프셋을 통한 버퍼 사용 크기, 여분 크기 산출의 편의성을 위해서이다.
	// 따라서 외부에서 버퍼를 주입하는 방식으로 생성하는 링버퍼의 실질적 용량은 1 바이트 더 작다.
	JBuffer() : JBuffer(JBUFFER_DEFAULT_CAPACITY) {}
	// new 동적 할당을 통해 내부 버퍼 생성
	JBuffer(UINT capacity) 
		: m_DeqOffset(0), m_EnqOffset(0), m_Capacity(capacity), 
		m_InternalCapacity(m_Capacity + 1), m_IsExternalBuffer(false) 
	{
		m_Buffer = new BYTE[m_InternalCapacity];
	}
	// 외부에서 링버퍼가 랩핑할 내부 버퍼 주입, 기본 Resize 함수 사용 불가
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
	// new 동적 할당을 통해 생성된 내부 버퍼를 가진 링퍼버의 경우 버퍼 크기 Resizing 가능
	// 기존 버퍼 크기보다 큰 크기로 사이즈 변경
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

	// 버퍼 용량, 사용중인 크기, 여분 크기 산출
	// return: capacity는 생성 시점에 사용자가 요청한 용량보다 1 바이트 큰 값. 따라서 capacity - 1  을 반환
	inline UINT	GetBufferSize(void) const { return m_Capacity; }
	// enqueued 데이터 크기
	inline UINT	GetUseSize(void) const {
		if (m_EnqOffset >= m_DeqOffset) { return m_EnqOffset - m_DeqOffset; }
		else { return m_EnqOffset + (m_InternalCapacity - m_DeqOffset); }
	}
	// enqueue 가능 데이터 크기
	inline UINT	GetFreeSize(void) const { return m_Capacity - GetUseSize(); }

	// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이. (끊기지 않은 길이)
	// 원형 큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로 돌아가서
	// 2번에 데이터를 얻거나 넣을 수 있음. 이 부분에서 끊어지지 않은 길이를 의미
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
	// enqueue 요청 크기가 링버퍼의 남은 여분 크기보다 크다면, 남은 여분 크기만 삽입한다.
	// params: (BYTE *)데이타 포인터. (UINT)크기. 
	// return: (UINT)넣은 크기.
	inline UINT	Enqueue(const BYTE* data, UINT uiSize) {
		UINT freeSize = GetFreeSize();
		UINT enqueueSize = (freeSize >= uiSize) ? uiSize : freeSize;	// 실제 enqueue 크기
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
	// dequeue 요청 크기가 링퍼버의 실제 사용 크기보다 크다면, 실제 사용 크기만큼만 반환한다.
	// params: (BYTE *)데이타 포인터. (UINT)크기.
	// return: (UINT)가져온 크기.
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
	// [Enqueue/Dequeue 포인터 직접 이동]
	// 지정 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	// return: (UINT)이동크기
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
	// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
	// params: (void *)데이타 포인터. (uUINT32_t)크기.
	// return: (UINT)가져온 크기.
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
	// 링 버퍼 특성상 연속적인 메모리 주소 공간 반환을 보장할 수 없다.
	// 따라서 DirectReserve 라는 함수를 제공한다. 사용하는 측은 DirectEnqueueSize() 함수를 통해 연속된 예약 공간이 있는지 확인한다.
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
			// 현재 꼬리 버퍼가 인큐 가능 상태라면 추가 불가
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

			// 인큐잉 가능 버퍼 첫 추가
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

	// 주의, dest 메모리 공간은 반드시 length보다 커야함. 오버플로우를 막을 수 없음
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