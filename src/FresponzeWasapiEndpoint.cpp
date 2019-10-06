/*****************************************************************
* Copyright (C) Vertver, 2019. All rights reserved.
* Fresponze - fast, simple and modern multimedia sound library
* Apache-2 License
******************************************************************
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*****************************************************************/
#include "FresponzeWasapi.h"
#include <process.h>
#include <avrt.h>
#pragma comment(lib, "avrt.lib")

#ifndef GUID_SECT
#define GUID_SECT
#endif

#define __FRDEFINE_GUID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const GUID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __FRDEFINE_IID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const IID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __FRDEFINE_CLSID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const CLSID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define FRDEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __FRDEFINE_CLSID(FR_CLSID_##className, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
#define FRDEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __FRDEFINE_IID(FR_IID_##interfaceName, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)

FRDEFINE_IID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 00000003, 0000, 0010, 80, 00, 00, aa, 00, 38, 9b, 71);
FRDEFINE_IID(KSDATAFORMAT_SUBTYPE_PCM, 00000001, 0000, 0010, 80, 00, 00, aa, 00, 38, 9b, 71);
#define maxmin(a, minimum, maximum)  min(max(a, minimum), maximum)

PROPERTYKEY FRPKEY_Device_FriendlyName = { { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } }, 14 };

inline
DWORD
GetSleepTime(
	DWORD Frames,
	DWORD SampleRate
)
{
	float fRet = 0.f;
	if (!SampleRate) return 0;

	fRet = ((((float)Frames / (float)SampleRate) * 1000) / 2);
	return (DWORD)fRet;
}

inline
void
CopyDataToBuffer(
	float* FileData,
	void* OutData,
	size_t FramesCount,
	bool IsFloat,
	int Bits,
	int Channels
)
{
	size_t sizeToRead = FramesCount * Channels;

	if (IsFloat) {
		memcpy(OutData, FileData, sizeToRead * sizeof(fr_f32));
	} else  {
		short* pShortData = (short*)FileData;
		switch (Bits) {
		case 16:
			for (size_t i = 0; i < sizeToRead; i++) {
				pShortData[i] = maxmin(((short)(FileData[i] * 32768.0f)), -32768, 32767);
			}
			break;
		default:
			break;
		}
	}
}

inline
void
CopyDataFromBuffer(
	void* FileData,
	float* OutData,
	size_t FramesCount,
	bool IsFloat,
	int Bits,
	int Channels
)
{
	size_t sizeToRead = FramesCount * Channels;

	if (IsFloat) {
		memcpy(OutData, FileData, sizeToRead * sizeof(fr_f32));
	}
	else {
		short* pShortData = (short*)FileData;
		switch (Bits) {
		case 16:
			for (size_t i = 0; i < sizeToRead; i++) {
				OutData[i] = (float)pShortData[i] < 0 ? pShortData[i] / 32768.0f : pShortData[i] / 32767.0f;
			}
			break;
		default:
			break;
		}
	}
}

void
WINAPIV
WASAPIThreadProc(void* pData)
{
	CWASAPIAudioEnpoint* pThis = (CWASAPIAudioEnpoint*)pData;
	pThis->ThreadProc();
}

void
CWASAPIAudioEnpoint::ThreadProc()
{	
	bool isFloat = EndpointInfo.EndpointFormat.IsFloat;
	fr_err errCode = 0;
	UINT32 CurrentFrames = 0;
	UINT32 AvailableFrames = 0;
	UINT32 StreamPadding = 0;
	UINT32 SampleRate = EndpointInfo.EndpointFormat.SampleRate;
	UINT32 Bits = EndpointInfo.EndpointFormat.Bits;
	UINT32 FramesInBuffer = EndpointInfo.EndpointFormat.Frames;
	UINT32 CurrentChannels = EndpointInfo.EndpointFormat.Channels;
	DWORD dwTask = 0;
	DWORD dwFlags = 0;
	DWORD dwFlushTime = GetSleepTime(FramesInBuffer, (DWORD)SampleRate);
	HRESULT hr = 0;
	HANDLE hMMCSS = nullptr;
	BYTE* pByte = nullptr;

	switch (EndpointInfo.Type) {
	case ProxyType:
	case RenderType: {
		hMMCSS = AvSetMmThreadCharacteristicsA("Pro Audio", &dwTask);
		if (IsInvalidHandle(hMMCSS)) return;	
	}
	break;
	case CaptureType: {
		hMMCSS = AvSetMmThreadCharacteristicsA("Capture", &dwTask);
		if (IsInvalidHandle(hMMCSS)) return;
	}
	break;
	default:
		break;
	}

	if (!AvSetMmThreadPriority(hMMCSS, AVRT_PRIORITY_CRITICAL))	{
		goto EndOfThread;
	}

	pSyncEvent->Raise();

	while (!pThreadEvent->Wait(dwFlushTime)) {
		try {
			switch (EndpointInfo.Type) {
			case ProxyType:
			case RenderType: {
				errCode = pAudioCallback->EndpointCallback(pTempBuffer, FramesInBuffer, CurrentChannels, (fr_i32)SampleRate, RenderType);
				hr = pAudioClient->GetCurrentPadding(&StreamPadding);
				if (FAILED(hr)) goto EndOfThread;	

				AvailableFrames = FramesInBuffer - StreamPadding;
				while (AvailableFrames) {
					/*
						In this case, "GetBuffer" function can be failed if
						buffer length is too much for us
					*/
					hr = pRenderClient->GetBuffer(AvailableFrames, &pByte);
					if (SUCCEEDED(hr)) {
						/* Process soundworker and copy data to main buffer */
						if (!pByte) continue;
						if (SUCCEEDED(errCode)) 
							CopyDataToBuffer(&pTempBuffer[FramesInBuffer - AvailableFrames], pByte, AvailableFrames, isFloat, Bits, CurrentChannels);
					} else {
						/* Don't try to destroy device if the buffer is unavailable */
						if (hr == AUDCLNT_E_BUFFER_TOO_LARGE) continue;
						goto EndOfThread;
					}

					/* If we can't release buffer - close invalid host */
					hr = pRenderClient->ReleaseBuffer(AvailableFrames, errCode == -2 ? AUDCLNT_BUFFERFLAGS_SILENT : 0);
					if (FAILED(hr)) goto EndOfThread;
					hr = pAudioClient->GetCurrentPadding(&StreamPadding);
					if (FAILED(hr)) goto EndOfThread;

					AvailableFrames = FramesInBuffer - StreamPadding;
				}

				pAudioCallback->RenderCallback(FramesInBuffer, CurrentChannels, (fr_i32)SampleRate);
			}
			break;
			case CaptureType: {
				hr = pCaptureClient->GetNextPacketSize(&StreamPadding);
				if (FAILED(hr)) goto EndOfThread;

				/* Process all data while we have it in WASAPI capture buffer */
				AvailableFrames = StreamPadding;
				while (AvailableFrames) {
					hr = pCaptureClient->GetBuffer(&pByte, &AvailableFrames, &dwFlags, nullptr, nullptr);
					if (SUCCEEDED(hr)) {
						/* If current data from capture device is silent - skip to next frame */
						if (dwFlags & AUDCLNT_BUFFERFLAGS_SILENT) continue;
						CopyDataFromBuffer(pByte, pTempBuffer, AvailableFrames, isFloat, Bits, CurrentChannels);
						if (FAILED(pAudioCallback->EndpointCallback(pTempBuffer, AvailableFrames, CurrentChannels, (fr_i32)SampleRate, CaptureType))) 
							goto EndOfThread;

						/* Release current buffer and get to next buffer */
						hr = pCaptureClient->ReleaseBuffer(AvailableFrames);
						if (FAILED(hr)) goto EndOfThread;
						hr = pCaptureClient->GetNextPacketSize(&AvailableFrames);
						if (FAILED(hr)) goto EndOfThread;
					}
				} 
			}
			break;
			default:
				break;
			}
		} catch (...) {
			goto EndOfThread;
		}
	}

EndOfThread:
	if (pThreadEvent->IsRaised()) pThreadEvent->Reset();
	if (pSyncEvent->IsRaised()) pSyncEvent->Reset();
	if (!IsInvalidHandle(hMMCSS)) AvRevertMmThreadCharacteristics(hMMCSS);
}

bool
CWASAPIAudioEnpoint::CreateWasapiThread()
{
	DWORD dwThreadId = 0;

#ifndef XBOX_BUILD
	hThread = (HANDLE)_beginthread(WASAPIThreadProc, 0, this);
#else
	hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)WASAPIThreadProc, this, 0, &dwThreadId);
#endif

	if (!IsInvalidHandle(hThread)) {
		if (!pSyncEvent->Wait(30)) {
			/* Terminate our thread if it was timeouted */
			if (WaitForSingleObject(hThread, 0) != WAIT_OBJECT_0) {
				TerminateThread(hThread, (DWORD)-1);
			}

			return false;
		}
	}
	else return false;

	return true;
}

void 
CWASAPIAudioEnpoint::SetDeviceInfo(EndpointInformation& DeviceInfo)
{
	memcpy(&EndpointInfo, &DeviceInfo, sizeof(EndpointInformation));
}

void 
CWASAPIAudioEnpoint::GetDeviceInfo(EndpointInformation& DeviceInfo)
{
	memcpy(&DeviceInfo, &EndpointInfo, sizeof(EndpointInformation));
}

void
CWASAPIAudioEnpoint::SetCallback(IAudioCallback* pCallback)
{
	pCallback->Clone((void**)&pAudioCallback);
}

bool
CWASAPIAudioEnpoint::GetEndpointDeviceInfo()
{
	LPWSTR lpwDeviceId = nullptr;
	IPropertyStore* pPropertyStore = nullptr;
	WAVEFORMATEX* pWaveFormat = nullptr;
	PROPVARIANT value = { 0 };

	if (!pCurrentDevice) return false;
	if (FAILED(pCurrentDevice->OpenPropertyStore(STGM_READ, &pPropertyStore))) return false;

	/* Get friendly name of current device */
	PropVariantInit(&value);
	if (SUCCEEDED(pPropertyStore->GetValue(FRPKEY_Device_FriendlyName, &value))) {
		if (value.vt == VT_LPWSTR) {
			/* Get size of new UTF-8 string */
			int StringSize = WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, NULL, 0, NULL, NULL);
			char lpNewString[260] = {};

			/* Translate UTF-16 string to normalized UTF-8 string */
			if (StringSize && StringSize < 259) {
				if (WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, lpNewString, 260, NULL, NULL)) {
					strcpy_s(EndpointInfo.EndpointName, lpNewString);
				}
			} else {
				strcpy_s(EndpointInfo.EndpointName, "Unknown Device Name");
			}
		}
	} else {
		strcpy_s(EndpointInfo.EndpointName, "Unknown Device Name");
	}
	PropVariantClear(&value);

	/* Get UUID of current device */
	if (SUCCEEDED(pCurrentDevice->GetId(&lpwDeviceId))) {
		/* Get size of new UTF-8 string */
		int StringSize = WideCharToMultiByte(CP_UTF8, 0, lpwDeviceId, -1, NULL, 0, NULL, NULL);
		char lpNewString[260] = {};

		/* Translate UTF-16 string to normalized UTF-8 string */
		if (StringSize && StringSize < 259) {
			if (WideCharToMultiByte(CP_UTF8, 0, value.pwszVal, -1, lpNewString, 260, NULL, NULL)) {
				strcpy_s(EndpointInfo.EndpointName, lpNewString);
			}
			else {
				strcpy_s(EndpointInfo.EndpointName, "Unknown Device UUID");
			}
		}
	} else {
		strcpy_s(EndpointInfo.EndpointName, "Unknown Device UUID");
	}

	return true;
}

bool
CWASAPIAudioEnpoint::InitializeToPlay(fr_f32 Delay)
{	
	UINT32 BufferFrames = 0;
	HRESULT hr = 0;
	REFERENCE_TIME refTimeDefault = 0;
	REFERENCE_TIME refTimeMin = 0;
	REFERENCE_TIME refTimeAccepted = REFERENCE_TIME(Delay * 10000.f);	
	WAVEFORMATEX* pWaveFormat = nullptr;

	Close();
	if (!GetEndpointDeviceInfo()) return false;
	if (FAILED(pAudioClient->GetMixFormat(&pWaveFormat))) return false;

	/* Set local format struct */
	EndpointInfo.EndpointFormat.IsFloat = pWaveFormat->wFormatTag == 3;
	EndpointInfo.EndpointFormat.Bits = pWaveFormat->wBitsPerSample;
	EndpointInfo.EndpointFormat.Channels = pWaveFormat->nChannels;
	EndpointInfo.EndpointFormat.SampleRate = pWaveFormat->nSamplesPerSec;
	if (pWaveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		WAVEFORMATEXTENSIBLE* pTmp = (WAVEFORMATEXTENSIBLE*)pWaveFormat;
		EndpointInfo.EndpointFormat.IsFloat = pTmp->SubFormat == FR_IID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	}

	if (FAILED(pAudioClient->GetDevicePeriod(&refTimeDefault, &refTimeMin))) {
		refTimeDefault = 1000000;
	}

	if (refTimeAccepted < refTimeMin) {
		refTimeAccepted = refTimeDefault;
	}

	/* Try to initalize with custom delay time */
	hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, refTimeAccepted, 0, pWaveFormat, nullptr);
	if (FAILED(hr)) {
		/* The delay time can be unaligned, so we use default device time */
		if (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED) {
			hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, refTimeDefault, 0, pWaveFormat, nullptr);
			if (FAILED(hr)) {
				CoTaskMemFree(pWaveFormat);
				return false;
			}
		}
	}

	if (FAILED(pAudioClient->GetBufferSize(&BufferFrames))) {
		CoTaskMemFree(pWaveFormat);
		return false;
	}

	/* Try to get needy for us service */
	if (EndpointInfo.Type == CaptureType) {
		if (FAILED(pAudioClient->GetService(IID_PPV_ARGS(&pCaptureClient)))) {
			CoTaskMemFree(pWaveFormat);
			return false;
		}
	} else if (EndpointInfo.Type == RenderType) {
		if (FAILED(pAudioClient->GetService(IID_PPV_ARGS(&pRenderClient)))) {
			CoTaskMemFree(pWaveFormat);
			return false;
		}
	} 

	EndpointInfo.EndpointFormat.Frames = (fr_i32)BufferFrames;
	pTempBuffer = (fr_f32*)FastMemAlloc(sizeof(fr_f32) * EndpointInfo.EndpointFormat.Frames * EndpointInfo.EndpointFormat.Channels);
	CoTaskMemFree(pWaveFormat);
	return true;
}

bool
CWASAPIAudioEnpoint::Open(fr_f32 Delay)
{
	InitializeToPlay(Delay);
}

bool
CWASAPIAudioEnpoint::Close()
{
	Stop();
	_RELEASE(pCaptureClient);
	_RELEASE(pRenderClient);
	return true;
}

bool 
CWASAPIAudioEnpoint::Start(fr_f32 Delay)
{
	pStartEvent->Raise();
	return SUCCEEDED(pAudioClient->Start());
}

bool
CWASAPIAudioEnpoint::Stop()
{
	if (pAudioClient) pAudioClient->Stop();
	if (!IsInvalidHandle(hThread)) {
		pThreadEvent->Raise();
		pStartEvent->Reset();
		if (WaitForSingleObject(hThread, 30) == WAIT_TIMEOUT) {
			TerminateThread(hThread, (DWORD)-1);
		}

		if (pTempBuffer) {
			FreeFastMemory(pTempBuffer);
			pTempBuffer = nullptr;
		}

		return true;
	}  

	return false;
}
