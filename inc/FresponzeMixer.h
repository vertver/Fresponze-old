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
#include "FresponzeEffect.h"

class IAudioMixer : public IBaseInterface
{
protected:
	fr_i32 SoundsCount = 0;
	fr_i32 QueuedBuffers = 0;
	SoundState InputState = NoneState;
	SoundNodeStruct* pSoundsNode = nullptr;
	EffectNodeStruct* pInputFirstEffect = nullptr;
	EffectNodeStruct* pMasterFirstEffect = nullptr;
	IAudioCallback* pAudioCallback = nullptr;
	CRingFloatBuffer InputBuffer;
	CRingFloatBuffer OutputBuffer;
	PcmFormat MixFormat = {};
	PcmFormat InputFormat = {};

public:
	virtual bool SetMixFormat(PcmFormat& NewFormat) = 0;
	virtual bool GetMixFormat(PcmFormat& ThisFormat) = 0;

	/*
		Function for adding, managming or deleting sound from sound node
	*/
	virtual bool AddSound(CBaseSound* pNeedySound) = 0;
	virtual bool LoadSound(CBaseSound* pNeedySound, CBaseSound* pNewSound) = 0;
	virtual bool DeleteSound(CBaseSound* pNeedySound) = 0;
	virtual bool PlaySoundById(CBaseSound* pNeedySound) = 0;
	virtual bool PauseSound(CBaseSound* pNeedySound) = 0;
	virtual bool StopSound(CBaseSound* pNeedySound) = 0;

	/*
		Function for manipulating with capture device and effect for it
	*/
	virtual bool EnableInputPlay(bool bPlay) = 0;
	virtual bool AddInputEffect(IBaseEffect* pEffectToClone) = 0;
	virtual bool RemoveInputEffect(IBaseEffect* pEffectToRemove) = 0;
	virtual bool SetInputEffectOption(IBaseEffect* pEffect, fr_i32 OptionIndex, fr_f32* ValueToSet, fr_i32 ValueSize) = 0;

	/*
		Function for manipulating with sound options
	*/
	virtual bool SetDefaultSoundOptions(fr_f32* pOptions) = 0;
	virtual bool SetSoundOption(CBaseSound* pNeedySound, fr_i32 OptionIndex, fr_f32* ValueToSet, fr_i32 ValueSize) = 0;
	virtual bool ResetSoundOption(CBaseSound* pNeedySound, fr_i32 OptionIndex) = 0;

	/*
		Function for manipulating with effects for sound
	*/
	virtual bool AddEffect(CBaseSound* pNeedySound, IBaseEffect* pEffectToClone) = 0;
	virtual bool RemoveEffect(CBaseSound* pNeedySound, IBaseEffect* pEffectToRemove) = 0;

	/*
		Function for internal use
	*/
	virtual bool Record(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) = 0;
	virtual bool Update(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) = 0;
	virtual bool Render(fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) = 0;
};

class CAudioCallback final : public IAudioCallback
{
protected:
	IAudioMixer* pAudioMixer = nullptr;

public:
	CAudioCallback(IAudioMixer* pParentMixer)
	{
#ifdef WINDOWS_PLATFORM
		_InterlockedIncrement(&Counter);
#endif
		pAudioMixer = pParentMixer;
	}

	fr_err EndpointCallback(fr_f32* pData, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate, fr_i32 CurrentEndpointType) override
	{
		if (!pAudioMixer) return -1;

		if (CurrentEndpointType == RenderType) {
			return (pAudioMixer->Update(pData, Frames, Channels, SampleRate) ? 0 : -1);
		}
		else if (CurrentEndpointType == CaptureType) {
			return (pAudioMixer->Record(pData, Frames, Channels, SampleRate) ? 0 : -1);
		}
	}

	fr_err RenderCallback(fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
	{
		if (!pAudioMixer) return -1;
		return (pAudioMixer->Render(Frames, Channels, SampleRate) ? 0 : -1);
	}
};
