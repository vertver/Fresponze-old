#include "FresponzeAdvancedMixer.h"
#include "FresponzeWavFile.h"

#define RING_BUFFERS_COUNT 2

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
	SetBufferSamples(NewFormat.Frames);
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

bool
CAdvancedMixer::Record(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	return false;
}

bool
CAdvancedMixer::Update(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	fr_i32 UpdatedSamples = 0;
	if (RingBuffer.GetLeftBuffers() <= 0) {
		QueuedBuffers = 0;
		QueuedSamples = 0;
		Render(BufferedSamples, Channels, SampleRate);
	}

	fr_i32 ret = RingBuffer.ReadData(pBuffer, Frames * Channels);
	UpdatedSamples += ret / Channels;
	if (UpdatedSamples < Frames) {
		QueuedBuffers = 0;
		QueuedSamples = 0;
		Render(BufferedSamples, Channels, SampleRate);
		ret = RingBuffer.ReadData(&pBuffer[ret], (Frames - UpdatedSamples) * Channels);
		UpdatedSamples += ret / Channels;
		if (UpdatedSamples < Frames) return false;
	}

	return true;
}

/*  Test function for generating float sin (300 to 1200 Hz)
	static bool state = false;
	static fr_f32 phase = 0.f;
	static fr_f32 freq = 150.f;
	fr_f32* pBuf = (fr_f32*)pByte;
	for (size_t i = 0; i < AvailableFrames * CurrentChannels; i++) {
		if (freq >= 300.f) state = !state;
		pBuf[i] = sinf(phase * 6.283185307179586476925286766559005f) * 0.1f;
		phase = fmodf(phase + freq / SampleRate, 1.0f);
		freq = !state ? freq + 0.005f : freq - 0.005f;
		if (freq <= 150.f) state = !state;
	}
*/

bool
CAdvancedMixer::Render(fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	if (QueuedBuffers || QueuedSamples) return false;
	RingBuffer.SetBuffersCount(RING_BUFFERS_COUNT);
	RingBuffer.Resize(Frames * Channels);
	OutputBuffer.Resize(Frames * Channels);
	tempBuffer.Resize(Channels, Frames);
	mixBuffer.Resize(Channels, Frames);
	QueuedBuffers = 0;
	QueuedSamples = 0;

	for (size_t i = 0; i < RING_BUFFERS_COUNT; i++) {
		mixBuffer.Clear();
		tempBuffer.Clear();
		ListenersNode* pListNode = pFirstListener;

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
		RingBuffer.PushBuffer(OutputBuffer.Data(), Frames * Channels);
		RingBuffer.NextBuffer();
		QueuedBuffers++;
		QueuedSamples += Frames;
	}

	return true;
}
