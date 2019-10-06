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
#include <audiopolicy.h>
#include <AudioClient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#define _RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr;} }

class CWASAPIAudioEnpoint final : public IAudioEndpoint
{
private:
	HANDLE hThread = nullptr;
	IBaseEvent* pThreadEvent = nullptr;
	IMMDevice* pCurrentDevice = nullptr;
	IAudioClient* pAudioClient = nullptr;
	IAudioRenderClient* pRenderClient = nullptr;
	IAudioCaptureClient* pCaptureClient = nullptr;

	bool CreateWasapiThread();
	bool GetEndpointDeviceInfo();

	bool InitalizeEndpoint()
	{
		pSyncEvent = new CWinEvent;
		pThreadEvent = new CWinEvent;
		pSyncEvent = new CWinEvent;
		return !!pThreadEvent || !!pSyncEvent;
	}

	bool InitializeClient(IMMDevice* pDev)
	{	
		if (pDev) return SUCCEEDED(pDev->Activate(__uuidof(pAudioClient), CLSCTX_ALL, nullptr, (void**)&pAudioClient));
		return false;
	}

	bool InitializeToPlay(fr_f32 Delay);

public:
	CWASAPIAudioEnpoint(fr_i32 DeviceType, void* pMMDevice, EndpointInformation& Info)
	{
		_InterlockedIncrement(&Counter);
		IMMDevice* pTempDevice = (IMMDevice*)pMMDevice;
		if (!pTempDevice) return;

		if (InitializeClient(pTempDevice)) {
			pTempDevice->QueryInterface(IID_PPV_ARGS(&pCurrentDevice));
			InitalizeEndpoint();
			memcpy(&EndpointInfo, &Info, sizeof(EndpointInformation));
		}
	}

	~CWASAPIAudioEnpoint()
	{
		Stop();
		_RELEASE(pAudioCallback);
		_RELEASE(pCaptureClient);
		_RELEASE(pRenderClient);
		_RELEASE(pAudioClient);
		_RELEASE(pCurrentDevice);
		if (pSyncEvent) delete pSyncEvent;
		if (pThreadEvent) delete pThreadEvent;
	}

	void ThreadProc();
	void SetDeviceInfo(EndpointInformation& DeviceInfo) override;
	void GetDeviceInfo(EndpointInformation& DeviceInfo) override;
	void SetCallback(IAudioCallback* pCallback) override;
	bool Open(fr_f32 Delay) override;
	bool Close() override;
	bool Start(fr_f32 Delay) override;
	bool Stop() override;
};
