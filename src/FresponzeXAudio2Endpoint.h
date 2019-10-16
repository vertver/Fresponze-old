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
#pragma once
#include "FresponzeEndpoint.h"
#include <xaudio2.h>
#include <mmdeviceapi.h>
#include <AudioClient.h>

#ifndef GUID_SECT
#define GUID_SECT
#endif

#define __FRDEFINE_GUID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const GUID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __FRDEFINE_IID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const IID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define __FRDEFINE_CLSID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) const CLSID n GUID_SECT = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#define FRDEFINE_CLSID(className, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __FRDEFINE_CLSID(FRE_CLSID_##className, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)
#define FRDEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __FRDEFINE_IID(FRE_IID_##interfaceName, 0x##l, 0x##w1, 0x##w2, 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8)

#define maxmin(a, minimum, maximum)  min(max(a, minimum), maximum)

#define DECLSPEC_UUID_WRAPPER(x) __declspec(uuid(#x))

#define DEFINE_IID(interfaceName, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
            interface DECLSPEC_UUID_WRAPPER(l##-##w1##-##w2##-##b1##b2##-##b3##b4##b5##b6##b7##b8) interfaceName; \
            EXTERN_C const GUID DECLSPEC_SELECTANY IID_##interfaceName = __uuidof(interfaceName)

DEFINE_IID(IXAudio2Legacy, 8bcf1f58, 9fe7, 4583, 8a, c6, e2, ad, c4, 65, c8, bb);

enum EXAudio2Versions : fr_i32
{
	eNoneVersion,
	eXAudio27,
	eXAudio28,
	eXAudio29
};

inline
bool 
GetDeviceInformation(IMMDevice* pDevicePtr, PcmFormat& fmt)
{
	FRDEFINE_IID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 00000003, 0000, 0010, 80, 00, 00, aa, 00, 38, 9b, 71);
	IAudioClient* pAudioClient = nullptr;
	WAVEFORMATEX* pWaveFormat = nullptr;
	PROPVARIANT value = { 0 };

	if (FAILED(pDevicePtr->Activate(__uuidof(pAudioClient), CLSCTX_ALL, nullptr, (void**)&pAudioClient))) return false;
	if (FAILED(pAudioClient->GetMixFormat(&pWaveFormat))) {
		_RELEASE(pAudioClient);
		return false;
	}

	/* Set local format struct */
	fmt.IsFloat = pWaveFormat->wFormatTag == 3;
	fmt.Bits = pWaveFormat->wBitsPerSample;
	fmt.Channels = pWaveFormat->nChannels;
	fmt.SampleRate = pWaveFormat->nSamplesPerSec;
	if (pWaveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		WAVEFORMATEXTENSIBLE* pTmp = (WAVEFORMATEXTENSIBLE*)pWaveFormat;
		fmt.IsFloat = pTmp->SubFormat == FRE_IID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	}

	if (pWaveFormat) CoTaskMemFree(pWaveFormat);
	_RELEASE(pAudioClient);
	return true;
}

DECLARE_INTERFACE_(IXAudio2Legacy, IUnknown)
{
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, __deref_out void** ppvInterface) PURE;

	STDMETHOD_(ULONG, AddRef) (THIS) PURE;

	STDMETHOD_(ULONG, Release) (THIS) PURE;

	STDMETHOD(GetDeviceCount) (THIS_ __out UINT32* pCount) PURE;

	STDMETHOD(GetDeviceDetails) (THIS_ UINT32 Index, __out void* pDeviceDetails) PURE;

	STDMETHOD(Initialize) (THIS_ UINT32 Flags X2DEFAULT(0),
		XAUDIO2_PROCESSOR XAudio2Processor X2DEFAULT(XAUDIO2_DEFAULT_PROCESSOR)) PURE;

	STDMETHOD(RegisterForCallbacks) (__in IXAudio2EngineCallback* pCallback) PURE;

	STDMETHOD_(void, UnregisterForCallbacks) (__in IXAudio2EngineCallback* pCallback) PURE;

	STDMETHOD(CreateSourceVoice) (THIS_ __deref_out IXAudio2SourceVoice** ppSourceVoice,
		__in const WAVEFORMATEX* pSourceFormat,
		UINT32 Flags X2DEFAULT(0),
		float MaxFrequencyRatio X2DEFAULT(XAUDIO2_DEFAULT_FREQ_RATIO),
		__in_opt IXAudio2VoiceCallback* pCallback X2DEFAULT(NULL),
		__in_opt const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
		__in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL)) PURE;

	STDMETHOD(CreateSubmixVoice) (THIS_ __deref_out IXAudio2SubmixVoice** ppSubmixVoice,
		UINT32 InputChannels, UINT32 InputSampleRate,
		UINT32 Flags X2DEFAULT(0), UINT32 ProcessingStage X2DEFAULT(0),
		__in_opt const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
		__in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL)) PURE;

	STDMETHOD(CreateMasteringVoice) (THIS_ __deref_out IXAudio2MasteringVoice** ppMasteringVoice,
		UINT32 InputChannels X2DEFAULT(XAUDIO2_DEFAULT_CHANNELS),
		UINT32 InputSampleRate X2DEFAULT(XAUDIO2_DEFAULT_SAMPLERATE),
		UINT32 Flags X2DEFAULT(0), UINT32 DeviceIndex X2DEFAULT(0),
		__in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL)) PURE;

	STDMETHOD(StartEngine) (THIS) PURE;

	STDMETHOD_(void, StopEngine) (THIS) PURE;

	STDMETHOD(CommitChanges) (THIS_ UINT32 OperationSet) PURE;

	STDMETHOD_(void, GetPerformanceData) (THIS_ __out XAUDIO2_PERFORMANCE_DATA* pPerfData) PURE;

	STDMETHOD_(void, SetDebugConfiguration) (THIS_ __in_opt const XAUDIO2_DEBUG_CONFIGURATION* pDebugConfiguration,
		__in_opt __reserved void* pReserved X2DEFAULT(NULL)) PURE;
};

DEFINE_IID(XAudio2Legacy, 5a508685, a254, 4fba, 9b, 82, 9a, 24, b0, 03, 06, af);

class ÑXAudio2DownlevelWrap : public IXAudio2
{
private:
	long Ref = 0;
	IMMDeviceEnumerator* pDeviceEnumerator = nullptr;
	IXAudio2Legacy* pLegacyXAudio2;

	fr_i32 FindDeviceIndexById(LPCWSTR szDeviceId, PcmFormat& fmt)
	{
		EDataFlow flow = eRender;
		fr_i32 ret = 0;
		UINT CountOfDevices = 0;
		LPWSTR lpId = nullptr;
		IMMDevice* pTempDevice = nullptr;
		IMMDevice* pTempEnumDevice = nullptr;
		IMMDevice* pTempDefaultDevice = nullptr;
		IMMEndpoint* pTempEndpoint = nullptr;
		IMMDeviceCollection* pDeviceCollection = nullptr;

		/*
			Try to open device to get device flow
		*/
		if (!pDeviceEnumerator) return -2;
		if (!szDeviceId) return -1;
		if (FAILED(pDeviceEnumerator->GetDevice(szDeviceId, &pTempDevice))) return -1;
		if (FAILED(pTempDevice->QueryInterface(IID_PPV_ARGS(&pTempEndpoint)))) {
			ret = -2;
			goto EndOfFunc;
		}

		if (FAILED(pTempEndpoint->GetDataFlow(&flow))) {
			ret = -2;
			goto EndOfFunc;
		}

		/*
			We can'to open capture device by this way
		*/
		if (flow != eRender) {
			ret = -2;
			goto EndOfFunc;
		}

		/*
			Open default device to compare ids
		*/
		if (FAILED(pDeviceEnumerator->GetDefaultAudioEndpoint(flow, eConsole, &pTempDefaultDevice))) {
			ret = -2;
			goto EndOfFunc;
		}

		if (FAILED(pTempDefaultDevice->GetId(&lpId))) {
			ret = -2;
			goto EndOfFunc;
		}

		_RELEASE(pTempDefaultDevice);
		if (!wcscmp(lpId, szDeviceId)) {
			ret = -1;
			goto EndOfFunc;
		}

		/*
			Try to enumerate all devices to get indexes of it
		*/
		CoTaskMemFree(lpId);
		lpId = nullptr;
		if (FAILED(pDeviceEnumerator->EnumAudioEndpoints(flow, eConsole, &pDeviceCollection))) {
			ret = -2;
			goto EndOfFunc;
		}

		if (FAILED(pDeviceCollection->GetCount(&CountOfDevices))) {
			ret = -2;
			goto EndOfFunc;
		}

		for (fr_u32 i = 0; i < CountOfDevices; i++) {
			if (FAILED(pDeviceCollection->Item(i, &pTempEnumDevice))) {
				ret = -2;
				goto EndOfFunc;
			}

			if (FAILED(pTempEnumDevice->GetId(&lpId))) {
				ret = -2;
				goto EndOfFunc;
			}

			if (!wcscmp(lpId, szDeviceId)) {
				GetDeviceInformation(pTempEnumDevice, fmt);
				ret = i;
				goto EndOfFunc;
			}

			CoTaskMemFree(lpId);
			lpId = nullptr;
			_RELEASE(pTempEnumDevice);
		}

EndOfFunc:
		if (lpId) CoTaskMemFree(lpId);
		_RELEASE(pTempDefaultDevice);
		_RELEASE(pTempEnumDevice);
		_RELEASE(pDeviceCollection);
		_RELEASE(pTempEndpoint);
		_RELEASE(pTempDevice);
		return ret;
	}

public:
	ÑXAudio2DownlevelWrap() 
	{
		_InterlockedIncrement(&Ref);
	}

	bool Initialize()
	{
		HMODULE hXAudio2 = nullptr;
		HRESULT hr = 0;

		hXAudio2 = LoadLibraryW(L"XAudio2_7.dll");
		if (IsInvalidHandle(hXAudio2)) return false;
		hr = CoCreateInstance(IID_XAudio2Legacy, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pLegacyXAudio2));
		if (FAILED(hr)) {
			FreeLibrary(hXAudio2);
			return false;
		} 

		hr = pLegacyXAudio2->Initialize(0, 1);
		if (FAILED(hr)) {
			_RELEASE(pLegacyXAudio2);
			FreeLibrary(hXAudio2);
			return false;
		}

		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDeviceEnumerator));
		if (FAILED(hr)) {
			_RELEASE(pLegacyXAudio2);
			FreeLibrary(hXAudio2);
			return false;
		}

		return true;
	}

	STDMETHOD(RegisterForCallbacks) (__in IXAudio2EngineCallback* pCallback)
	{
		return pLegacyXAudio2->RegisterForCallbacks(pCallback);
	}

	STDMETHOD_(void, UnregisterForCallbacks) (__in IXAudio2EngineCallback* pCallback)
	{
		return pLegacyXAudio2->UnregisterForCallbacks(pCallback);
	}

	STDMETHOD(CreateSourceVoice) (THIS_ __deref_out IXAudio2SourceVoice** ppSourceVoice,
		__in const WAVEFORMATEX* pSourceFormat,
		UINT32 Flags X2DEFAULT(0),
		float MaxFrequencyRatio X2DEFAULT(XAUDIO2_DEFAULT_FREQ_RATIO),
		__in_opt IXAudio2VoiceCallback* pCallback X2DEFAULT(NULL),
		__in_opt const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
		__in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL))
	{
		return pLegacyXAudio2->CreateSourceVoice(ppSourceVoice, pSourceFormat, Flags, MaxFrequencyRatio, pCallback, pSendList, pEffectChain);
	}

	STDMETHOD(CreateSubmixVoice) (THIS_ __deref_out IXAudio2SubmixVoice** ppSubmixVoice,
		UINT32 InputChannels, UINT32 InputSampleRate,
		UINT32 Flags X2DEFAULT(0), UINT32 ProcessingStage X2DEFAULT(0),
		__in_opt const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
		__in_opt const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL))
	{
		return pLegacyXAudio2->CreateSubmixVoice(ppSubmixVoice, InputChannels, InputSampleRate, Flags, ProcessingStage, pSendList, pEffectChain);
	}

	STDMETHOD(CreateMasteringVoice) (THIS_ _Outptr_ IXAudio2MasteringVoice** ppMasteringVoice,
		UINT32 InputChannels X2DEFAULT(XAUDIO2_DEFAULT_CHANNELS),
		UINT32 InputSampleRate X2DEFAULT(XAUDIO2_DEFAULT_SAMPLERATE),
		UINT32 Flags X2DEFAULT(0), _In_opt_z_ LPCWSTR szDeviceId X2DEFAULT(NULL),
		_In_opt_ const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL),
		_In_ AUDIO_STREAM_CATEGORY StreamCategory X2DEFAULT(AudioCategory_GameEffects))
	{
		PcmFormat fmt = {};
		fr_i32 DeviceIndex = FindDeviceIndexById(szDeviceId, fmt);
		if (DeviceIndex == -2) return E_FAIL;
		DeviceIndex++;		// because first device is 1, not 0
		return pLegacyXAudio2->CreateMasteringVoice(
			ppMasteringVoice, 
			InputChannels ? InputChannels : fmt.Channels, 
			InputSampleRate ? InputSampleRate : fmt.SampleRate, 
			Flags, 
			DeviceIndex, 
			pEffectChain
		);
	}

	STDMETHOD(StartEngine) (THIS)
	{
		return pLegacyXAudio2->StartEngine();
	}

	STDMETHOD_(void, StopEngine) (THIS)
	{
		return pLegacyXAudio2->StopEngine();
	}

	STDMETHOD(CommitChanges) (THIS_ UINT32 OperationSet)
	{
		return pLegacyXAudio2->CommitChanges(OperationSet);
	}

	STDMETHOD_(void, GetPerformanceData) (THIS_ __out XAUDIO2_PERFORMANCE_DATA* pPerfData)
	{
		return pLegacyXAudio2->GetPerformanceData(pPerfData);
	}

	STDMETHOD_(void, SetDebugConfiguration) (THIS_ __in_opt const XAUDIO2_DEBUG_CONFIGURATION* pDebugConfiguration,
		__in_opt __reserved void* pReserved X2DEFAULT(NULL))
	{
		return pLegacyXAudio2->SetDebugConfiguration(pDebugConfiguration, pReserved);
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(const IID &, void **) override
	{
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef() override
	{
		return _InterlockedIncrement(&Ref);
	}

	ULONG STDMETHODCALLTYPE Release() override
	{
		ULONG ulRef = _InterlockedDecrement(&Ref);
		if (0 == ulRef)
		{
			delete this;
		}
		return ulRef;
	}
};

typedef HRESULT(__stdcall *PXAudio2CreateWithVersionInfoFunc)(_Outptr_ IXAudio2**, UINT32, XAUDIO2_PROCESSOR, DWORD);
typedef HRESULT(__stdcall *PXAudio2CreateInfoFunc)(_Outptr_ IXAudio2**, UINT32, XAUDIO2_PROCESSOR);

inline
IXAudio2*
InitializeXAudio2(fr_i32& Version)
{
	DWORD dwFlags = 0;
	HMODULE hLibrary = nullptr;
	IXAudio2* pRetEngine = nullptr;
	ÑXAudio2DownlevelWrap* pWrappedEngine = nullptr;
	PXAudio2CreateInfoFunc pXAudio2Create = nullptr;
	PXAudio2CreateWithVersionInfoFunc pXAudio2CreateWithVersion = nullptr;

	/*
		"The DirectX SDK versions of XAUDIO2 supported a flag XAUDIO2_DEBUG_ENGINE to
		select between the release and 'checked' version. This flag is not supported
		or defined in the Windows 8 version of XAUDIO2."

		https://docs.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-xaudio2create
	*/
#ifdef _DEBUG
	dwFlags = XAUDIO2_DEBUG_ENGINE;
#endif

tryTo29:
	hLibrary = LoadLibraryW(L"xaudio2_9.dll");
	if (IsInvalidHandle(hLibrary)) goto tryTo28;

	/*
		This function become from Windows 10 RS5 (1809) update, and Microsoft
		recommend to use it if available. Of course, if you need it,
		you can ignore and use lower versions
	*/
	pXAudio2CreateWithVersion = (PXAudio2CreateWithVersionInfoFunc)GetProcAddress(hLibrary, "XAudio2CreateWithVersionInfo");
	if (!pXAudio2CreateWithVersion) {
		pXAudio2Create = (PXAudio2CreateInfoFunc)GetProcAddress(hLibrary, "XAudio2Create");
		if (!pXAudio2Create) {
			FreeLibrary(hLibrary);
			return nullptr;
		}
	}

	if (pXAudio2CreateWithVersion) {
#ifdef _DEBUG
		if (FAILED(pXAudio2CreateWithVersion(&pRetEngine, dwFlags, XAUDIO2_DEFAULT_PROCESSOR, NTDDI_VERSION)))
#endif
		{
			if (FAILED(pXAudio2CreateWithVersion(&pRetEngine, 0, XAUDIO2_DEFAULT_PROCESSOR, NTDDI_VERSION))) {
				FreeLibrary(hLibrary);
				goto tryTo28;
			}
		}

		Version = eXAudio29;
		goto EndOfFunction;
	}

	if (pXAudio2Create) {
#ifdef _DEBUG
		if (FAILED(pXAudio2Create(&pRetEngine, dwFlags, XAUDIO2_DEFAULT_PROCESSOR)))
#endif
		{
			if (FAILED(pXAudio2Create(&pRetEngine, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
				FreeLibrary(hLibrary);
				goto tryTo28;
			}
		}

		Version = eXAudio29;
		goto EndOfFunction;
	}

tryTo28:
	hLibrary = LoadLibraryW(L"xaudio2_8.dll");
	if (IsInvalidHandle(hLibrary)) goto tryTo27;

	pXAudio2Create = (PXAudio2CreateInfoFunc)GetProcAddress(hLibrary, "XAudio2Create");
	if (!pXAudio2Create) {
		FreeLibrary(hLibrary);
		goto tryTo27;
	}

#ifdef _DEBUG
	if (FAILED(pXAudio2Create(&pRetEngine, dwFlags, XAUDIO2_DEFAULT_PROCESSOR))) 
#endif
	{
		if (FAILED(pXAudio2Create(&pRetEngine, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
			FreeLibrary(hLibrary);
			goto tryTo27;
		}
	}

	Version = eXAudio28;
	goto EndOfFunction;

tryTo27:
	hLibrary = LoadLibraryW(L"xaudio2_7.dll");
	if (IsInvalidHandle(hLibrary)) goto EndOfFunction;

	pWrappedEngine = new ÑXAudio2DownlevelWrap;
	if (!pWrappedEngine->Initialize()) { 
		_RELEASE(pWrappedEngine);
		FreeLibrary(hLibrary);
		return nullptr;
	}
	pRetEngine = pWrappedEngine;
	Version = eXAudio27;

EndOfFunction:
	return pRetEngine;
}