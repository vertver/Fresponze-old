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
#include "FresponzeEnumerator.h"
#include "FresponzeHardware.h"

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS_PLATFORM 1
#endif

#ifdef WINDOWS_PLATFORM
#ifdef DLL_PLATFORM
#ifdef LIB_EXPORTS
#define FRAPI __declspec(dllexport)
#else
#define FRAPI __declspec(dllimport)
#endif
#else 
#define FRAPI
#endif
#else
#define FRAPI
#endif

#ifdef USE_FUNCS_PROTOTYPES
typedef fr_err(FrInitializeInstance_t)(void** ppInstance);
typedef fr_err(FrDestroyInstance_t)(void* pInstance);
typedef fr_err(FrGetRemoteInterface_t)(void** ppRemoteInterface);
#else
extern "C"
{
	fr_err FRAPI FrInitializeInstance(void** ppInstance);
	fr_err FRAPI FrDestroyInstance(void* pInstance);
	fr_err FRAPI FrGetRemoteInterface(void** ppRemoteInterface);
}
#endif

enum MixerType
{
	eMixerNoneType,
	eMixerGameType,
	eMixerEditType,
	eMixerCustomType
};

enum EndpointType
{
	eEndpointNoneType,
	eEndpointWASAPIType,
	eEndpointXAudio2Type
};

class CFresponze : public IBaseInterface
{
public:
	void GetMixerInterface(MixerType mixerType, void** ppMixerInterface);
	void GetHardwareInterface(EndpointType endpointType, void* pCustomCallback, void** ppHardwareInterface);
};
