#include "JBufferHandler.h"

bool JBufferHandler::Dequeue_TestHdr(JBuffer& jbuff, stTestHdr& testHdr)
{
	return false;
}

bool JBufferHandler::Enqueue_TestHdr(JBuffer& jbuff, const stTestHdr& testHdr)
{
	return false;
}

bool JBufferHandler::Dequeue_TestMsg(JBuffer& jbuff, stTestMsg& testMsg)
{
	return false;
}

bool JBufferHandler::Enqueue_TestMsg(JBuffer& jbuff, const stTestMsg& testMsg)
{
	return false;
}

bool JBufferHandler::Dequeue_TestMsg(JBuffer& jbuff, stTestMsg2& testMsg2)
{
	UINT msgVecLen;
	jbuff >> msgVecLen;
	testMsg2.msgVec.resize(msgVecLen);
	for (int i = 0; i < msgVecLen; i++) {
		jbuff >> testMsg2.msgVec[i];
	}

	UINT wstrLen;
	jbuff >> wstrLen;
	testMsg2.wstr.resize(wstrLen);
	//jbuff.Dequeue((BYTE*)testMsg2.wstr.data(), wstrLen * sizeof(wchar_t));	<- OK
	jbuff.Dequeue(reinterpret_cast<BYTE*>(const_cast<wchar_t*>(testMsg2.wstr.data())), wstrLen * sizeof(wchar_t));

	return true;
}

bool JBufferHandler::Enqueue_TestMsg(JBuffer& jbuff, const stTestMsg2& testMsg2)
{
	jbuff << static_cast<UINT>(testMsg2.msgVec.size());
	for (const stTestMsg& msg : testMsg2.msgVec) {
		jbuff << msg;
	}

	jbuff << static_cast<UINT>(testMsg2.wstr.size());
	//jbuff.Enqueue(static_cast<const BYTE*>(testMsg2.wstr.data()), testMsg2.wstr.size()); <- NOT OK
	jbuff.Enqueue(reinterpret_cast<const BYTE*>(testMsg2.wstr.data()), testMsg2.wstr.size() * sizeof(wchar_t));

	return true;
}

bool JBufferHandler::Dequeue(JBuffer& jbuff, stTEST_B& msg)
{
	jbuff >> msg;
	if (!msg.validate())
		return false;

	msg.vec.resize(msg.vecLen);
	//jbuff.Dequeue(msg.vec.data()...)
	for (int i = 0; i < msg.vecLen; i++) {
		jbuff >> msg.vec[i];
	}
}

bool JBufferHandler::Enqueue(JBuffer& jbuff, const stTEST_B& msg)
{
	
}
