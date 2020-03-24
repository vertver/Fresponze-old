#include "FresponzeAdvancedMixer.h"
#include "FresponzeWavFile.h"

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
	}
	else {
		pLastListener->pNext = new ListenersNode;
		pLastListener = pLastListener->pNext;
	}

	pNode = pLastListener;
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

void*
GetFormatListener(char* pListenerOpenLink)
{
	if (!strcmp(GetFilePathFormat(pListenerOpenLink), ".wav")) return new CRIFFMediaResource();
	return nullptr;
}

bool
CAdvancedMixer::CreateListener(void* pListenerOpenLink, ListenersNode*& pNewListener, PcmFormat ListFormat)
{
	if (!ListFormat.Bits) ListFormat = MixFormat;

	CreateNode(pNewListener);
	IMediaResource* pNewResource = (IMediaResource*)GetFormatListener((char*)pListenerOpenLink);
	if (!pNewResource) return false;
	if (!pNewResource->OpenResource(pListenerOpenLink)) {
		_RELEASE(pNewResource);
		return false;
	}

	if (ListFormat.Bits) pNewResource->SetFormat(ListFormat);
	pNewListener->pListener = new CMediaListener(pNewResource);
	return true;
}

bool
CAdvancedMixer::DeleteListener(ListenersNode* pListNode)
{
	return DeleteNode(pListNode);
}

/*
if (!RenderedBuffers) return false;
	if (OutputBuffer.GetBufferSize() < Frames) {
		if (BufferPosition)
		memcpy(pBuffer, &OutputBuffer.GetData()[BufferPosition], Frames * sizeof(fr_f32));
	}

	return true;


	if (RenderedBuffers) return false;
	OutputBuffer.Resize(Frames);
	OutputBuffer.SetBuffersCount(2);
	while (RenderedBuffers < OutputBuffer.GetBuffersCount()) {
		static fr_f32 phase = 0.f;
		ListenersNode* pListNode = pFirstListener;
		tempBuffer.Resize(Channels, Frames / Channels);
		mixBuffer.Resize(Channels, Frames / Channels);

		for (size_t i = 0; i < Channels; i++) {
			for (size_t i = 0; i < Frames; i++) {
				OutputBuffer.GetBufferData(0)[i] = sinf(phase * 6.283185307179586476925286766559005f);
				phase = fmodf(phase + 1000.f / SampleRate, 1.0f);
			}
		}


		while (pListNode) {
			pListNode->pListener->Process(tempBuffer.GetBuffers(), Frames / Channels);
			for (size_t o = 0; o < Channels; o++) {
				fr_f32* pFirst = tempBuffer.GetBufferData(o);
				fr_f32* pSecond = mixBuffer.GetBufferData(o);
				for (size_t i = 0; i < Frames / Channels; i++) {
					pSecond[i] += pFirst[i];
				}
			}
			pListNode = pListNode->pNext;
		}

		PlanarToLinear(mixBuffer.GetBuffers(), OutputBuffer.Data(), Frames, Channels);

//RenderedBuffers++;
//	}

*/

bool
CAdvancedMixer::Record(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	return false;
}

bool
CAdvancedMixer::Update(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	fr_f32* pData = OutputBuffer.Data();
	if (!pData) memset(pBuffer, 0, Frames * Channels * sizeof(fr_f32));
	else memcpy(pBuffer, pData, Frames * Channels * sizeof(fr_f32));
	return true;
}

bool
CAdvancedMixer::Render(fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	static fr_f32 phase = 0.f;
	ListenersNode* pListNode = pFirstListener;
	OutputBuffer.Resize(Frames * Channels);
	tempBuffer.Resize(Channels, Frames);
	mixBuffer.Resize(Channels, Frames);

	//for (size_t i = 0; i < Frames; i++) {
	//	OutputBuffer.Data()[i] = sinf(phase * 6.283185307179586476925286766559005f);
	//	phase = fmodf(phase + 300.f / SampleRate, 1.0f);
	//}

	
	while (pListNode) {
		pListNode->pListener->Process(tempBuffer.GetBuffers(), Frames);
		for (size_t o = 0; o < Channels; o++) {
			fr_f32* pFirst = tempBuffer.GetBufferData(o);
			fr_f32* pSecond = mixBuffer.GetBufferData(o);
			for (size_t i = 0; i < Frames; i++) {
				pSecond[i] += pFirst[i];
			}
		}
		pListNode = pListNode->pNext;
	}

	PlanarToLinear(mixBuffer.GetBuffers(), OutputBuffer.Data(), Frames * Channels, Channels);

	return true;
}
