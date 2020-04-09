/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2019. All rights reserved.
* Fresponze - fast, simple and modern multimedia sound library
* Apache-2 License
**********************************************************************
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
#include "Fresponze.h"
#include "FresponzeWasapiHardware.h"
#include "FresponzeAdvancedMixer.h"

void* hModule = nullptr;

void
TypeToLog(const char* Text)
{
	static fr_string1k outputString = {};
	GetDebugTime(outputString, sizeof(outputString));

#ifdef _DEBUG
#ifdef WINDOWS_PLATFORM
	strcat_s(outputString, Text);
	OutputDebugStringA(outputString);
	OutputDebugStringA("\n");
#else
	printf("%s%s\n", outputString, Text);
#endif
#endif
}

void
CFresponze::GetHardwareInterface(
	EndpointType endpointType, 
	void* pCustomCallback,
	void** ppHardwareInterface
)
{
	switch (endpointType)
	{
	case eEndpointNoneType:
	case eEndpointWASAPIType:
		*ppHardwareInterface = (void*)(new CWASAPIAudioHardware((IAudioCallback*)pCustomCallback));
		break;
	case eEndpointXAudio2Type:
#if 0
		*ppHardwareInterface = (void*)(new CXAudio2AudioHardware((IAudioCallback*)pCustomCallback));
#endif
		break;
	default:
		break;
	}
}

void 
CFresponze::GetMixerInterface(
	MixerType mixerType,
	void** ppMixerInterface
)
{
	switch (mixerType)
	{
	case eMixerNoneType:
		break;
	case eMixerGameType:
		break;
	case eMixerAdvancedType:
		*ppMixerInterface = new CAdvancedMixer();
		break;
	case eMixerCustomType:
		break;
	default:
		break;
	}
}

void* 
AllocateInstance()
{
	return new CFresponze;
}

void 
DeleteInstance(
	void* pInstance
)
{
	CFresponze* pFresponze = (CFresponze*)pInstance;
	_RELEASE(pFresponze);
}

extern "C"
{
	fr_err
	FRAPI
	FrInitializeInstance(
		void** ppOutInstance
	)
	{
		if (!Fresponze::InitMemory()) return -1;
#if defined(WINDOWS_PLATFORM) && defined(DLL_PLATFORM)
		GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&hModule, (HMODULE*)&hModule);
#endif
		*ppOutInstance = AllocateInstance();
		if (!*ppOutInstance) return -1;

		return 0;
	}

	fr_err 
	FRAPI
	FrDestroyInstance(
		void* pInstance
	)
	{
		Fresponze::DestroyMemory();
		DeleteInstance(pInstance);
		return 0;
	}

	fr_err
	FRAPI
	FrGetRemoteInterface(
		void** ppRemoteInterface
	)
	{
		return -1;
	}
} 
