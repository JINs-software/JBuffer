#include "JBuffer.h"

#include <memory>

JBuffer::JBuffer() 
	: JBuffer(JBUFFER_DEFAULT_CAPACITY)
{}
JBuffer::JBuffer(UINT capacity)
	: m_DeqOffset(0), m_EnqOffset(0), m_Capacity(capacity), m_InternalCapacity(m_Capacity + 1), m_IsExternalBuffer(false)
{
	m_Buffer = new BYTE[m_InternalCapacity];
}
JBuffer::JBuffer(UINT capacity, BYTE* buffer) 
	: m_DeqOffset(0), m_EnqOffset(0), m_Capacity(capacity - 1), m_InternalCapacity(capacity), m_IsExternalBuffer(true), m_Buffer(buffer)
{}

JBuffer::JBuffer(JBuffer* jbuff)
{
	*this = *jbuff;
}

JBuffer::~JBuffer()
{
	if (!m_IsExternalBuffer) {
		delete[] m_Buffer;
	}	
}

void JBuffer::Init(UINT _capacity) {
	m_DeqOffset = 0;
	m_EnqOffset = 0;
	m_Capacity = _capacity;
	m_InternalCapacity = m_Capacity + 1;
	m_IsExternalBuffer = false;
	m_Buffer = new BYTE[m_InternalCapacity];
}
void JBuffer::Init(UINT _capacity, BYTE* _buffer) {
	m_DeqOffset = 0;
	m_EnqOffset = 0;
	m_Capacity = _capacity - 1;
	m_InternalCapacity = m_Capacity;
	m_IsExternalBuffer = true;
	m_Buffer = _buffer;
}

JBuffer JBuffer::SliceBuffer(UINT length, bool dequeuing) {
	// �⺻ ���� ������ ȣ��(���� ����)
	JBuffer sliceBuff(*this);

	sliceBuff.m_IsExternalBuffer = true;	// �ܺ� �������� ���

	sliceBuff.m_Capacity = length;			// �뷮 ����

	if (dequeuing) {
		sliceBuff.m_EnqOffset = (m_DeqOffset + length) % m_InternalCapacity;
	}

	return sliceBuff;
}
JBuffer SliceBuffer(const JBuffer& other, UINT length, bool dequeuing)
{
	// �⺻ ���� ������ ȣ��(���� ����)
	JBuffer sliceBuff(other);

	sliceBuff.m_IsExternalBuffer = true;	// �ܺ� �������� ���

	sliceBuff.m_Capacity = length;			// �뷮 ����

	if (dequeuing) {
		sliceBuff.m_EnqOffset = (other.m_DeqOffset + length) % other.m_InternalCapacity;
	}

	return sliceBuff;
}

//bool JBuffer::Resize(UINT _size) {
//	if (!m_IsExternalBuffer && m_Capacity < _size) {
//		m_Capacity = _size;
//		BYTE* newBuffer = new BYTE[_size + 1];
//
//		if (GetUseSize() > 0) {
//			if (GetUseSize() > GetDirectDequeueSize()) {
//				memcpy(newBuffer, GetDequeueBufferPtr(), GetDirectDequeueSize());
//				memcpy(&newBuffer[GetDirectDequeueSize()], m_Buffer, GetUseSize() - GetDirectDequeueSize());
//			}
//			else {
//				memcpy(newBuffer, GetDequeueBufferPtr(), GetUseSize());
//			}
//		}
//
//		m_DeqOffset = 0;
//		m_EnqOffset = GetUseSize();
//		m_Capacity = _size;
//		delete[] m_Buffer;
//
//		m_Buffer = newBuffer;
//
//		return true;
//	}
//	else {
//		return false;
//	}
//}
// => ���� �ʿ�
