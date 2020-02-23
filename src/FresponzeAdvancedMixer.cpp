#include "FresponzeAdvancedMixer.h"

bool CAdvancedMixer::SetNewFormat(PcmFormat fmt)
{
	int counter = 0;
	ListenersNode* pNode = pFirstListener;
	while (pNode) {
		if (pNode->pListener) pNode->pListener->SetFormat(fmt);
		pNode = pNode->pNext;
		counter++;
	}

	return !!counter;
}

bool
CAdvancedMixer::SetMixFormat(PcmFormat& NewFormat)
{
	if (!SetNewFormat(NewFormat)) return false;
	MixFormat = NewFormat;
	return true;
}

bool 
CAdvancedMixer::GetMixFormat(PcmFormat& ThisFormat)
{
	ThisFormat = MixFormat; 
	return true;
}

bool 
CAdvancedMixer::CreateNode(ListenersNode*& pNode)
{
	ListenersNode* pCurrent = pLastListener;
	if (!pCurrent) {
		pLastListener = new ListenersNode;
		pFirstListener = pLastListener;
		pCurrent = pLastListener;
	}

	return true;
}

bool
CAdvancedMixer::DeleteNode(ListenersNode* pNode)
{
	ListenersNode* pCurrent = pLastListener;
	while (pCurrent) {
		if (pCurrent == pNode) {
			if (pLastListener == pCurrent) pLastListener = pCurrent->pPrev;
			pCurrent->pPrev->pNext = pCurrent->pNext;
			pCurrent->pNext->pPrev = pCurrent->pPrev;
			_RELEASE(pCurrent->pListener);
			delete pCurrent;
			return true;
		}
		pCurrent = pCurrent->pPrev;
	}

	return false;
}

bool
CAdvancedMixer::CreateListener(void* pListenerOpenLink, ListenersNode*& pNewListener, PcmFormat ListFormat)
{
	return false;
}

bool
CAdvancedMixer::DeleteListener(ListenersNode* pListNode)
{
	return false;

}

bool 
CAdvancedMixer::Record(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	return false;
	/*
		Prepare to mix input data
	*/
}

bool
CAdvancedMixer::Update(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	return false;
	/*
		Flush all data
	*/
}

bool
CAdvancedMixer::Render(fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	return false;
	/*
		Mix all data
	*/
}
