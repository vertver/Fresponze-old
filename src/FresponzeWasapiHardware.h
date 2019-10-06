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
#include "FresponzeWasapiNotification.h"

class CWASAPIAudioHardware final : public IAudioHardware
{
private:
	IMMNotificationClient* pNotifyClient = nullptr;

public:
	CWASAPIAudioHardware(IAudioCallback* pParentAudioCallback)
	{
		pAudioCallback = pParentAudioCallback;
		pAudioEnumerator = new CWASAPIAudioEnumerator();
		pNotificationCallback = nullptr;
		pNotifyClient = new CWASAPIAudioNotification(pNotificationCallback);
	}

	~CWASAPIAudioHardware()
	{
		_RELEASE(pInputEndpoint);
		_RELEASE(pOutputEndpoint);
		_RELEASE(pNotificationCallback)
		_RELEASE(pNotifyClient);
		_RELEASE(pAudioEnumerator);
	}

	bool Open(fr_i32 DeviceType, fr_f32 DelayTime) override
	{
		IAudioEndpoint** ppThisEndpoint = ((DeviceType == RenderType) ? &pOutputEndpoint : DeviceType == CaptureType ? &pInputEndpoint : nullptr);
		IAudioEndpoint*& pThisEndpoint = *ppThisEndpoint;

		if (!pAudioEnumerator->GetDefaultDevice(DeviceType, pThisEndpoint)) return false;
		if (!pThisEndpoint->Open(DelayTime)) {
			_RELEASE(pThisEndpoint);
			return false;
		}
		pThisEndpoint->SetCallback(pAudioCallback);
		return true;
	}

	bool Open(fr_i32 DeviceType, fr_f32 DelayTime, char* pUUID) override
	{
		IAudioEndpoint** ppThisEndpoint = ((DeviceType == RenderType) ? &pOutputEndpoint : DeviceType == CaptureType ? &pInputEndpoint : nullptr);
		IAudioEndpoint*& pThisEndpoint = *ppThisEndpoint;

		if (!pAudioEnumerator->GetDeviceByUUID(DeviceType, pUUID, pThisEndpoint)) return false;
		if (!pThisEndpoint->Open(DelayTime)) {
			_RELEASE(pThisEndpoint);
			return false;
		}
		pThisEndpoint->SetCallback(pAudioCallback);
		return true;
	}

	bool Open(fr_i32 DeviceType, fr_f32 DelayTime, fr_i32 DeviceId) override
	{
		IAudioEndpoint** ppThisEndpoint = ((DeviceType == RenderType) ? &pOutputEndpoint : DeviceType == CaptureType ? &pInputEndpoint : nullptr);
		IAudioEndpoint*& pThisEndpoint = *ppThisEndpoint;

		if (!pAudioEnumerator->GetDeviceById(DeviceType, DeviceId, pThisEndpoint)) return false;
		if (!pThisEndpoint->Open(DelayTime)) {
			_RELEASE(pThisEndpoint);
			return false;
		}
		pThisEndpoint->SetCallback(pAudioCallback);
		return true;
	}

	bool Restart(fr_i32 DeviceType, fr_f32 DelayTime) override
	{
		IAudioEndpoint** ppThisEndpoint = ((DeviceType == RenderType) ? &pOutputEndpoint : DeviceType == CaptureType ? &pInputEndpoint : nullptr);
		IAudioEndpoint*& pThisEndpoint = *ppThisEndpoint;

		pThisEndpoint->Close();
		return Open(DeviceType, DelayTime);
	}

	bool Restart(fr_i32 DeviceType, fr_f32 DelayTime, char* pUUID) override
	{
		IAudioEndpoint** ppThisEndpoint = ((DeviceType == RenderType) ? &pOutputEndpoint : DeviceType == CaptureType ? &pInputEndpoint : nullptr);
		IAudioEndpoint*& pThisEndpoint = *ppThisEndpoint;

		pThisEndpoint->Close();
		return Open(DeviceType, DelayTime, pUUID);
	}

	bool Restart(fr_i32 DeviceType, fr_f32 DelayTime, fr_i32 DeviceId) override
	{
		IAudioEndpoint** ppThisEndpoint = ((DeviceType == RenderType) ? &pOutputEndpoint : DeviceType == CaptureType ? &pInputEndpoint : nullptr);
		IAudioEndpoint*& pThisEndpoint = *ppThisEndpoint;

		pThisEndpoint->Close();
		return Open(DeviceType,DelayTime, DeviceId);
	}

	bool Start() override
	{

	}

	bool Stop() override
	{

	}


	bool Close() override
	{

	}

};

