#include <Windows.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <tchar.h>

//#include "RingBuffer.h"
#include "JBuffer.h"
#include "JBufferHandler.h"

int BuffSize = 10000;
const char TestString[] = "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345";
const int TestStringLen = 81;

using namespace std;

struct stHdr {
	int type;
	int len;
};
struct stDummy {
	short s;
	int i;
	long long ll;
};
struct stVariableLenMsg {
	stDummy dummArr[0];
};

const int numOfMsgType = 4;
struct stMsg0 {
	//int msgLen;
	//char msg[20];// = "MSG0";
	int i;
};
struct stMsg1 {
	//int msgLen;
	//char msg[20];// = "MSG01";
	short s;
};
struct stMsg2 {
	//int msgLen;
	//char msg[20];// = "MSG002";
	double b;
};
struct stMsg3 {
	//int msgLen;
	//char msg[20];// = "MSG0003";
	stDummy dummy;
};

void PrintMsg(stMsg0 msg) {
	//for (int i = 0; i < msg.msgLen; i++) {
	//	std::cout << msg.msg[i];
	//}
	std::cout << "(Msg0)" << std::endl;
	std::cout << "msg.i: " << msg.i << std::endl;
}
void PrintMsg(stMsg1 msg) {
	//for (int i = 0; i < msg.msgLen; i++) {
	//	std::cout << msg.msg[i];
	//}
	std::cout << "(Msg1)" << std::endl;
	std::cout << "msg.s: " << msg.s << std::endl;
}
void PrintMsg(stMsg2 msg) {

	//for (int i = 0; i < msg.msgLen; i++) {
	//	std::cout << msg.msg[i];
	//}
	std::cout << "(Msg2)" << std::endl;
	std::cout << "msg.b: " << msg.b << std::endl;
}
void PrintMsg(stMsg3 msg) {
	//for (int i = 0; i < msg.msgLen; i++) {
	//	std::cout << msg.msg[i];
	//}
	std::cout << "(Msg3)" << std::endl;
	std::cout << "msg.dummy.s: " << msg.dummy.s << std::endl;
	std::cout << "msg.dummy.i: " << msg.dummy.i << std::endl;
	std::cout << "msg.dummy.ll: " << msg.dummy.ll << std::endl;
}

/*
int main() {
	vector<int> vec;
	vector<long long> vec2(10);
	cout << sizeof(vec) << endl;
	cout << sizeof(vec2) << endl;

	//JBuffer ringBuff(BuffSize);
	//
	//stTestMsg2 enqMsg;
	//
	//enqMsg.msgVec.resize(10);
	//for (int i = 0; i < 10; i++) {
	//	stTestMsg msg;
	//	msg.s = 1;
	//	msg.i = 10;
	//	msg.ll = 100;
	//	enqMsg.msgVec[i] = msg;
	//}
	//enqMsg.wstr = L"안녕하세요 HELLO";
	//
	//JBufferHandler::Enqueue_TestMsg(ringBuff, enqMsg);
	//
	//stTestMsg2 deqMsg;
	//JBufferHandler::Dequeue_TestMsg(ringBuff, deqMsg);
}
*/
/*
int main() {

	JBuffer ringBuff(BuffSize);

	while (true) {
		srand((unsigned int)time(NULL));
		int inputCnt = rand() % 10;

		cout << "================================" << endl;
		// 링버퍼 삽입
		for (int i = 0; i < 3; i++) {
			switch (rand() % numOfMsgType)
			{
			case 0:
			{
				if (ringBuff.GetDirectEnqueueSize() >= sizeof(stHdr)) {
					stHdr* hdr = ringBuff.Reserve<stHdr>();
					hdr->type = 0;
					hdr->len = sizeof(stMsg0);
				}
				else {
					stHdr hdr;
					hdr.type = 0;
					hdr.len = sizeof(stMsg0);

					ringBuff << hdr;
				}

				int num = rand() % 100;
				ringBuff << num;
				
				cout << "[Msg0 삽입]" << endl;
				cout << "num: " << num << endl;
				break;
			}
			case 1: {
				if (ringBuff.GetDirectEnqueueSize() >= sizeof(stHdr)) {
					stHdr* hdr = ringBuff.Reserve<stHdr>();
					hdr->type = 1;
					hdr->len = sizeof(stMsg1);
				}
				else {
					stHdr hdr;
					hdr.type = 1;
					hdr.len = sizeof(stMsg1);

					ringBuff << hdr;
				}

				short num = rand() % 100;
				ringBuff << num;

				cout << "[Msg1 삽입]" << endl;
				cout << "num: " << num << endl;
				break;
			}
			case 2: {
				if (ringBuff.GetDirectEnqueueSize() >= sizeof(stHdr)) {
					stHdr* hdr = ringBuff.Reserve<stHdr>();
					hdr->type = 2;
					hdr->len = sizeof(stMsg2);
				}
				else {
					stHdr hdr;
					hdr.type = 2;
					hdr.len = sizeof(stMsg2);

					ringBuff << hdr;
				}

				double num = static_cast<double>(rand() % 100) / (rand() % 100 + 1);
				ringBuff << num;

				cout << "[Msg2 삽입]" << endl;
				cout << "num: " << num << endl;
				break;
			}
			case 3: {
				if (ringBuff.GetDirectEnqueueSize() >= sizeof(stHdr)) {
					stHdr* hdr = ringBuff.Reserve<stHdr>();
					hdr->type = 3;
					hdr->len = sizeof(stMsg3);
				}
				else {
					stHdr hdr;
					hdr.type = 3;
					hdr.len = sizeof(stMsg3);

					ringBuff << hdr;
				}

				stDummy dumm;
				dumm.i = rand() % 100;
				dumm.ll = rand() % 100;
				dumm.s = rand() % 100;
				ringBuff << dumm;

				cout << "[Msg3 삽입]" << endl;
				cout << "dumm.s: " << dumm.s << endl;
				cout << "dumm.i: " << dumm.i << endl;
				cout << "dumm.ll: " << dumm.ll << endl;
				break;
			}
			default:
				break;
			}
		}

		// 링버퍼 팝
		while (true) {
			if (ringBuff.GetUseSize() >= sizeof(stHdr)) {
				stHdr hdr;
				ringBuff.Peek(&hdr);
				if (ringBuff.GetUseSize() >= sizeof(stHdr) + hdr.len) {

					ringBuff >> hdr;

					switch (hdr.type)
					{
					case 0: {
						stMsg0 msg;
						//ringBuff >> msg.msgLen;
						//ringBuff >> msg.msg;
						ringBuff >> msg.i;

						PrintMsg(msg);
						break;
					}
					case 1: {
						stMsg1 msg;

						//ringBuff >> msg.msgLen;
						//ringBuff >> msg.msg;
						ringBuff >> msg.s;

						PrintMsg(msg);
						break;
					}
					case 2: {
						stMsg2 msg;

						//ringBuff >> msg.msgLen;
						//ringBuff >> msg.msg;
						ringBuff >> msg.b;

						PrintMsg(msg);
						break;
					}
					case 3: {
						stMsg3 msg;

						//ringBuff >> msg.msgLen;
						//ringBuff >> msg.msg;
						ringBuff >> msg.dummy;

						PrintMsg(msg);
						break;
					}
					default:
						break;
					}
				}
			}
			else {
				break;
			}
		}

		
	}
}
*/

int main() {
	// 핸들을 얻어오기
	HWND console = GetConsoleWindow();
	RECT r;
	// 현재 창의 크기 얻어오기
	GetWindowRect(console, &r);
	// 원하는 가로 폭 설정
	//int desiredWidth = TestStringLen * 10;  // 예시로 10픽셀 당 글자 수를 설정
	MoveWindow(console, r.left, r.top, 685, r.bottom - r.top, TRUE);

	// 프로그램 종료 방지를 위해 임시로 기다리기
	//system("pause");

	JBuffer ringBuff(BuffSize);
	int testStringIdx = 0;

	//unsigned long long loop = 0;
	while (true) {
		srand((unsigned int)time(NULL));

		// 현재 Ring 버퍼 인큐 가용 공간
		int buffFreeSize = ringBuff.GetFreeSize();

		// 버퍼에 넣을 사이즈(랜덤)
		int writeLen = (rand() % buffFreeSize) + 1;
		writeLen = (writeLen > TestStringLen) ? TestStringLen : writeLen;

		unsigned char* writeBuf = new unsigned char[writeLen];
		if (TestStringLen - testStringIdx < writeLen) {
			memcpy(writeBuf, &TestString[testStringIdx], TestStringLen - testStringIdx);
			memcpy(&writeBuf[TestStringLen - testStringIdx], TestString, writeLen - (TestStringLen - testStringIdx));
			testStringIdx = writeLen - (TestStringLen - testStringIdx);
		}
		else {
			memcpy(writeBuf, &TestString[testStringIdx], writeLen);
			testStringIdx = (testStringIdx + writeLen) % TestStringLen;
		}

		if (rand() % 2 == 0) {
			// Enqueue 활용
			int enqueueLen = ringBuff.Enqueue(writeBuf, writeLen);
			if (enqueueLen != writeLen) {
				std::cout << "[EXCEPTION] enqueueLen != writeLen" << std::endl;
				break;
			}
		}
		else {
			// DirectEnqueue 활용
			int dirEnqueueSize = ringBuff.GetDirectEnqueueSize();
			if (dirEnqueueSize < writeLen) {
				memcpy(ringBuff.GetEnqueueBufferPtr(), writeBuf, dirEnqueueSize);
				ringBuff.DirectMoveEnqueueOffset(dirEnqueueSize);
				memcpy(ringBuff.GetEnqueueBufferPtr(), &writeBuf[dirEnqueueSize], writeLen - dirEnqueueSize);
				ringBuff.DirectMoveEnqueueOffset(writeLen - dirEnqueueSize);
			}
			else {
				memcpy(ringBuff.GetEnqueueBufferPtr(), writeBuf, writeLen);
				ringBuff.DirectMoveEnqueueOffset(writeLen);
			}
		}
		

		// 버퍼에서 읽을 사이즈(랜덤)
		int buffUseSize = ringBuff.GetUseSize();
		int readLen = (rand() % buffUseSize)  + 1;

		unsigned char* readBuf = new unsigned char[readLen + 1];	// 출력을 위한 널 문자 포함
		readBuf[readLen] = NULL;

		unsigned char* peekedBuf = new unsigned char[readLen + 1];
		peekedBuf[readLen] = NULL;

		int peekedLen = ringBuff.Peek(peekedBuf, readLen);
		if (peekedLen != readLen) {
			std::cout << "[EXCEPTION] peekedLen != readLen" << std::endl;
			break;
		}

		if (rand() % 2 == 0) {
			// Dequeue 활용
			int dequeueLen = ringBuff.Dequeue(readBuf, readLen);
			if (dequeueLen != readLen) {
				std::cout << "[EXCEPTION] dequeueLen != readLen" << std::endl;
				break;
			}
		}
		else {
			int dirDequeueSize = ringBuff.GetDirectDequeueSize();
			if (dirDequeueSize < readLen) {
				memcpy(readBuf, ringBuff.GetDequeueBufferPtr(), dirDequeueSize);
				ringBuff.DirectMoveDequeueOffset(dirDequeueSize);
				memcpy(&readBuf[dirDequeueSize], ringBuff.GetDequeueBufferPtr(), readLen - dirDequeueSize);
				ringBuff.DirectMoveDequeueOffset(readLen - dirDequeueSize);
			}
			else {
				memcpy(readBuf, ringBuff.GetDequeueBufferPtr(), readLen);
				ringBuff.DirectMoveDequeueOffset(readLen);
			}
		}

		if (memcmp(readBuf, peekedBuf, readLen) != 0) {
			std::cout << "[EXCEPTION] memcmp(readBuf, peekedBuf, readLen) != 0" << std::endl;
			break;
		}

		std::cout << readBuf;
		delete[] writeBuf;
		delete[] readBuf;

		// 루프마다 RingBuffer 크기가 1씩 증가
		//BuffSize++;
		//ringBuff.Resize(BuffSize);
	}
}
