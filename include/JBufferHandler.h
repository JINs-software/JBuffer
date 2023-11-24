#pragma once
#include "JBuffer.h"
#include <vector>

using namespace std;

struct stTestHdr {
	uint32_t type;
	uint32_t msgLen;
};
struct stTestMsg {
	short s;
	int i;
	long long ll;
};
struct stTestMsg2 {
	vector<stTestMsg> msgVec;
	wstring wstr;
};

#pragma pack(1) 
struct stTEST_A {
	uint16_t packetSize;
	uint16_t packetID;

	uint64_t id;
	uint32_t hp;
	uint16_t attack;
};

// [stTEST_B][요소1 요소2 .. 요소n]
struct stTEST_B {
	struct stTEST_DATA {
		uint64_t dataId;
		float value;
	};

	uint16_t packetSize;
	uint16_t packetID;

	uint64_t id;
	uint32_t hp;
	uint16_t attack;

	uint16_t vecOffset;
	uint16_t vecLen;

	vector<stTEST_DATA> vec;

	bool validate() {	// 위 구조로 validation을 한번에 할 수 있다.
		// 위 구조가 아니면 필드를 파싱하면서 분석해야 한다.
		uint32_t size = 0;
		size += sizeof(stTEST_B);
		size += vecLen * sizeof(stTEST_DATA);
		if (size != packetSize)
			return false;
		if (vecOffset + vecLen * sizeof(stTEST_DATA) > packetSize)
			return false;

		return true;
	}
};
#pragma pack()

class JBufferHandler
{
public:
	static bool Dequeue_TestHdr(JBuffer& jbuff, stTestHdr& testHdr);
	static bool Enqueue_TestHdr(JBuffer& jbuff, const stTestHdr& testHdr);

	static bool Dequeue_TestMsg(JBuffer& jbuff, stTestMsg& testMsg);
	static bool Enqueue_TestMsg(JBuffer& jbuff, const stTestMsg& testMsg);

	static bool Dequeue_TestMsg(JBuffer& jbuff, stTestMsg2& testMsg2);
	static bool Enqueue_TestMsg(JBuffer& jbuff, const stTestMsg2& testMsg2);

	static bool Dequeue(JBuffer& jbuff, stTEST_B& msg);
	static bool Enqueue(JBuffer& jbuff, const stTEST_B& msg);
};

