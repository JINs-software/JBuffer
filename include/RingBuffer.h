#pragma once
#include <memory>

class RingBuffer
{
private:
	int		frontOffset;
	int		endOffset;
	char*	buffer;
	int		capacity;

public:
	RingBuffer(int capacity);
	~RingBuffer();

	void Resize(int size);

	int	GetBufferSize(void);

	// ���� ������� �뷮 ���.
	int	GetUseSize(void);

	// ���� ���ۿ� ���� �뷮 ���. 
	int	GetFreeSize(void);

	// WritePos �� ����Ÿ ����.
	// Parameters: (char *)����Ÿ ������. (int)ũ��. 
	// Return: (int)���� ũ��.
	int	Enqueue(char* chpData, int iSize);


	// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
	// Parameters: (char *)����Ÿ ������. (int)ũ��.
	// Return: (int)������ ũ��.
	int	Dequeue(char* chpDest, int iSize);

	// ReadPos ���� ����Ÿ �о��. ReadPos ����.
	// Parameters: (char *)����Ÿ ������. (int)ũ��.
	// Return: (int)������ ũ��.
	int	Peek(char* chpDest, int iSize);

	// ������ ��� ����Ÿ ����.
	// Parameters: ����.
	// Return: ����.
	void ClearBuffer(void);

	// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����. (������ ���� ����)
	// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
	// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
	// Parameters: ����.
	// Return: (int)��밡�� �뷮.
	int	DirectEnqueueSize(void);
	int	DirectDequeueSize(void);
	// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
	// Parameters: ����.
	// Return: (int)�̵�ũ��
	int	DirectMoveEnd(int iSize);
	int	DirectMoveFront(int iSize);

	// ������ Front ������ ����.
	// Parameters: ����.
	// Return: (char *) ���� ������.
	char* GetEnqueueBufferPtr(void);

	// ������ RearPos ������ ����.
	// Parameters: ����.
	// Return: (char *) ���� ������.
	char* GetDequeueBufferPtr(void);

};

