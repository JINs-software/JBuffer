#include <Windows.h>
#include <iostream>
#include <ctime>

#include "RingBuffer.h"

int BuffSize = 100;
const char TestString[] = "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345";
const int TestStringLen = 81;

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

	RingBuffer ringBuff(BuffSize);
	int testStringIdx = 0;

	//unsigned long long loop = 0;
	while (true) {
		srand((unsigned int)time(NULL));

		// 현재 Ring 버퍼 인큐 가용 공간
		int buffFreeSize = ringBuff.GetFreeSize();

		// 버퍼에 넣을 사이즈(랜덤)
		int writeLen = (rand() % buffFreeSize) + 1;
		writeLen = (writeLen > TestStringLen) ? TestStringLen : writeLen;

		char* writeBuf = new char[writeLen];
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
			int dirEnqueueSize = ringBuff.DirectEnqueueSize();
			if (dirEnqueueSize < writeLen) {
				memcpy(ringBuff.GetEnqueueBufferPtr(), writeBuf, dirEnqueueSize);
				ringBuff.DirectMoveEnd(dirEnqueueSize);
				memcpy(ringBuff.GetEnqueueBufferPtr(), &writeBuf[dirEnqueueSize], writeLen - dirEnqueueSize);
				ringBuff.DirectMoveEnd(writeLen - dirEnqueueSize);
			}
			else {
				memcpy(ringBuff.GetEnqueueBufferPtr(), writeBuf, writeLen);
				ringBuff.DirectMoveEnd(writeLen);
			}
		}
		

		// 버퍼에서 읽을 사이즈(랜덤)
		int buffUseSize = ringBuff.GetUseSize();
		int readLen = (rand() % buffUseSize)  + 1;

		char* readBuf = new char[readLen + 1];	// 출력을 위한 널 문자 포함
		readBuf[readLen] = NULL;

		char* peekedBuf = new char[readLen + 1];
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
			int dirDequeueSize = ringBuff.DirectDequeueSize();
			if (dirDequeueSize < readLen) {
				memcpy(readBuf, ringBuff.GetDequeueBufferPtr(), dirDequeueSize);
				ringBuff.DirectMoveFront(dirDequeueSize);
				memcpy(&readBuf[dirDequeueSize], ringBuff.GetDequeueBufferPtr(), readLen - dirDequeueSize);
				ringBuff.DirectMoveFront(readLen - dirDequeueSize);
			}
			else {
				memcpy(readBuf, ringBuff.GetDequeueBufferPtr(), readLen);
				ringBuff.DirectMoveFront(readLen);
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