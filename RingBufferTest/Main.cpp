#include <Windows.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <tchar.h>
#include "JBuffer.h"

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

/*
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
*/

/*
template<typename... Args>
JBuffer* CreateJBuffer(Args... args) {
	return new JBuffer(args...);
}

int main() {
	JBuffer* buffer = CreateJBuffer(1000);

	std::cout << "buffer" << std::endl;
}
*/

//WCHAR wchars1[] = L"가나다라마바사";
//WCHAR wchars2[] = L"Talk That Talk That Talk That";
//WCHAR wchars3[] = L"그만해 그만해 제발";
//WCHAR wchars4[] = L"어떻게 네가 이럴 수 있는지 내가 이렇게 아파야 하는지";
//WCHAR wchars5[] = L"내 생각을 넌 하기는 한건지 내가 우스운지";
//WCHAR wchars6[] = L"그래 들어줄게 어차피 다 거짓이겠지만";
//WCHAR wchars7[] = L"그래 웃어줄게 네가 놀던 그 여자들처럼 그렇게";
//WCHAR wchars8[] = L"처음부터 다 똑같은 거짓말 진심이었단 말은 하지도 마";
//WCHAR wchars9[] = L"이젠 널 믿기 힘들 것 같아 나.내가 우스운지";
//WCHAR wchars10[] = L"그래 들어줄게 알고 있는 변명일 테지만";
//WCHAR wchars11[] = L"그래 웃어줄게 이제 그만 꺼져줄래 상관 안 할게";
//WCHAR wchars12[] = L"(Talk that) 말도 안 되는 소리 해봐";
//WCHAR wchars13[] = L"(Talk that) 믿을 거란 생각하지 마";
//WCHAR wchars14[] = L"(Talk that) 네가 한 말 모두 헛소리 소리 소리니까";
//WCHAR wchars15[] = L"(Talk that) 있는대로 내게 말해봐";
//WCHAR wchars16[] = L"(Talk that) 나를 그만 가지고 놀아";
//WCHAR wchars17[] = L"(Talk that) 내게 한 말 전부 헛소리 소리 소리 같아 너..";
//WCHAR wchars18[] = L"내게 또 말을 하지만 듣기 싫어 그만해 다";
//WCHAR wchars19[] = L"하태일이 고~";
//WCHAR wchars20[] = L"정말 넌 다 잊었더라";
//WCHAR wchars21[] = L"반갑게 날 보는 너의 얼굴 보니";
//WCHAR wchars22[] = L"그제야 어렴풋이 아파오더라";
//WCHAR wchars23[] = L"새 살 차오르지 못한 상처가";
//WCHAR wchars24[] = L"눈물은 흐르질 않더라";
//WCHAR wchars25[] = L"이별이라 하는 게 대단치도 못해서";
//WCHAR wchars26[] = L"이렇게 보잘것없어서";
//WCHAR wchars27[] = L"좋은 이별이란 거, 결국 세상엔 없는 일이라는 걸";
//WCHAR wchars28[] = L"알았다면 그때 차라리 다 울어둘 걸";
//WCHAR wchars29[] = L"그때 이미 나라는 건 네겐 끝이었다는 건";
//WCHAR wchars30[] = L"나만 몰랐었던 이야기";


CHAR chr1[] = "Twinkle, twinkle, little star,";
CHAR chr2[] = "How I wonder what you are!";
CHAR chr3[] = "Up above the world so high,";
CHAR chr4[] = "Like a diamond in the sky.";
CHAR chr5[] = "Twinkle, twinkle, little star,";
CHAR chr6[] = "How I wonder what you are!";

CHAR c1[] = "Twink";
CHAR c2[] = "le, twi";
CHAR c3[] = "nkle, litt";
CHAR c4[] = "le star,";
CHAR c5[] = "How I wonder w";
CHAR c6[] = "hat you are!";
CHAR c7[] = "Up above the world so high,\0Like a diam";
CHAR c8[] = "ond in the sky.";
CHAR c9[] = "Twinkle, twinkle, little ";
CHAR c10[] = "star,\0How I wonder what you are!";


int main() {
	// 로케일 설정
	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale());

	JBuffer jbuff1(sizeof(chr1));
	jbuff1.Enqueue((BYTE*)chr1, sizeof(chr1));
	JBuffer jbuff2(sizeof(chr2));
	jbuff2.Enqueue((BYTE*)chr2, sizeof(chr2));
	JBuffer jbuff3(sizeof(chr3));
	jbuff3.Enqueue((BYTE*)chr3, sizeof(chr3));
	JBuffer jbuff4(sizeof(chr4));
	jbuff4.Enqueue((BYTE*)chr4, sizeof(chr4));
	JBuffer jbuff5(sizeof(chr5));
	jbuff5.Enqueue((BYTE*)chr5, sizeof(chr5));
	JBuffer jbuff6(sizeof(chr6));
	jbuff6.Enqueue((BYTE*)chr6, sizeof(chr6));
	
	JSerialBuffer jserbuff;
	jserbuff.Serialize(jbuff1, jbuff1.GetUseSize(), true);
	jserbuff.Serialize(jbuff2, jbuff2.GetUseSize(), true);
	jserbuff.Serialize(jbuff3, jbuff3.GetUseSize(), true);
	jserbuff.Serialize(jbuff4, jbuff4.GetUseSize(), true);
	jserbuff.Serialize(jbuff5, jbuff5.GetUseSize(), true);
	jserbuff.Serialize(jbuff6, jbuff6.GetUseSize(), true);

	/*
	JBuffer jbuff1(sizeof(chr1));
	JBuffer jbuff2(sizeof(chr2));
	JBuffer jbuff3(sizeof(chr3));
	JBuffer jbuff4(sizeof(chr4));
	JBuffer jbuff5(sizeof(chr5));
	JBuffer jbuff6(sizeof(chr6));

	JSerialBuffer jserbuff;
	jserbuff.Serialize(jbuff1, jbuff1.GetFreeSize(), false);
	jserbuff.Serialize(jbuff2, jbuff2.GetFreeSize(), false);
	jserbuff.Serialize(jbuff3, jbuff3.GetFreeSize(), false);
	jserbuff.Serialize(jbuff4, jbuff4.GetFreeSize(), false);
	jserbuff.Serialize(jbuff5, jbuff5.GetFreeSize(), false);
	jserbuff.Serialize(jbuff6, jbuff6.GetFreeSize(), false);

	jserbuff.Enqueue((BYTE*)c1, sizeof(c1)-1);
	jserbuff.Enqueue((BYTE*)c2, sizeof(c2)-1);
	jserbuff.Enqueue((BYTE*)c3, sizeof(c3)-1);
	jserbuff.Enqueue((BYTE*)c4, sizeof(c4));
	jserbuff.Enqueue((BYTE*)c5, sizeof(c5)-1);
	jserbuff.Enqueue((BYTE*)c6, sizeof(c6));
	jserbuff.Enqueue((BYTE*)c7, sizeof(c7)-1);
	jserbuff.Enqueue((BYTE*)c8, sizeof(c8));
	jserbuff.Enqueue((BYTE*)c9, sizeof(c9)-1);
	jserbuff.Enqueue((BYTE*)c10, sizeof(c10));
	*/

	while (jserbuff.GetUseSize() > 0) {
		UINT useSize = jserbuff.GetUseSize();
		int randLen = (rand() % jserbuff.GetUseSize()) + 1;
		CHAR* chs = new CHAR[randLen + 1];
		memset(chs, 0, randLen + 1);
		chs[randLen] = 0xFF;
		jserbuff.Dequeue((BYTE*)chs, randLen);

		for (int i = 0; i < randLen; i++) {
			if (chs[i] == 0) {
				chs[i] = '\n';
			}
		}
		chs[randLen] = NULL;
		std::cout << chs;
	}
}