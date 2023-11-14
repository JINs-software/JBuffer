#include <Windows.h>
#include <iostream>
#include <ctime>

#include "RingBuffer.h"

int BuffSize = 100;
const char TestString[] = "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345";
const int TestStringLen = 81;

int main() {
	// �ڵ��� ������
	HWND console = GetConsoleWindow();
	RECT r;
	// ���� â�� ũ�� ������
	GetWindowRect(console, &r);
	// ���ϴ� ���� �� ����
	//int desiredWidth = TestStringLen * 10;  // ���÷� 10�ȼ� �� ���� ���� ����
	MoveWindow(console, r.left, r.top, 685, r.bottom - r.top, TRUE);

	// ���α׷� ���� ������ ���� �ӽ÷� ��ٸ���
	//system("pause");

	RingBuffer ringBuff(BuffSize);
	int testStringIdx = 0;

	//unsigned long long loop = 0;
	while (true) {
		srand((unsigned int)time(NULL));

		// ���� Ring ���� ��ť ���� ����
		int buffFreeSize = ringBuff.GetFreeSize();

		// ���ۿ� ���� ������(����)
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
			// Enqueue Ȱ��
			int enqueueLen = ringBuff.Enqueue(writeBuf, writeLen);
			if (enqueueLen != writeLen) {
				std::cout << "[EXCEPTION] enqueueLen != writeLen" << std::endl;
				break;
			}
		}
		else {
			// DirectEnqueue Ȱ��
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
		

		// ���ۿ��� ���� ������(����)
		int buffUseSize = ringBuff.GetUseSize();
		int readLen = (rand() % buffUseSize)  + 1;

		char* readBuf = new char[readLen + 1];	// ����� ���� �� ���� ����
		readBuf[readLen] = NULL;

		char* peekedBuf = new char[readLen + 1];
		peekedBuf[readLen] = NULL;

		int peekedLen = ringBuff.Peek(peekedBuf, readLen);
		if (peekedLen != readLen) {
			std::cout << "[EXCEPTION] peekedLen != readLen" << std::endl;
			break;
		}

		if (rand() % 2 == 0) {
			// Dequeue Ȱ��
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

		// �������� RingBuffer ũ�Ⱑ 1�� ����
		//BuffSize++;
		//ringBuff.Resize(BuffSize);
	}
}