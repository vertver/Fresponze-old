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
#pragma once
#include "FresponzeMixer.h"

enum GameMixerSoundOptions
{
	NoneOption,
	OutputGain,		// 0.f to 1.f
	Position3D,		// < 1.f - disabled, 1.f >= - enabled
	PositionX,
	PositionY,
	PositionZ,		
	PositionFOV,	// 45.f to 180.f degrees
	Reverb
};

class CGameMixer final : public IAudioMixer
{
private:
	fr_f32* FloatMixBuffers[MAX_CHANNELS] = {};
	EffectNodeStruct* pReverbBusFirstEffect = nullptr;
	EffectNodeStruct* pBeforeMixingBusFirstEffect = nullptr;
	CRingFloatBuffer ReverbBus;
	CRingFloatBuffer BeforeMixingBus;

	EffectNodeStruct* FindEffectNode(IBaseEffect* pEffect);
	SoundNodeStruct* FindSoundNode(CBaseSound* pSound);

public:
	CGameMixer()
	{
		pAudioCallback = new CMixerAudioCallback(this);
	}

	~CGameMixer()
	{
		if (pAudioCallback) delete pAudioCallback;
	}

	bool SetMixFormat(PcmFormat& NewFormat) override;
	bool GetMixFormat(PcmFormat& ThisFormat) override;

	bool AddSound(CBaseSound* pNeedySound);
	bool LoadSound(CBaseSound* pNeedySound, CBaseSound* pNewSound);
	bool DeleteSound(CBaseSound* pNeedySound);
	bool PlaySoundById(CBaseSound* pNeedySound);
	bool PauseSound(CBaseSound* pNeedySound);
	bool StopSound(CBaseSound* pNeedySound);

	bool EnableInputPlay(bool bPlay);
	bool AddInputEffect(IBaseEffect* pEffectToClone);
	bool RemoveInputEffect(IBaseEffect* pEffectToRemove);
	bool SetInputEffectOption(IBaseEffect* pEffect, fr_i32 OptionIndex, fr_f32* ValueToSet, fr_i32 ValueSize);

	bool SetDefaultSoundOptions(fr_f32* pOptions);
	bool SetSoundOption(CBaseSound* pNeedySound, fr_i32 OptionIndex, fr_f32* ValueToSet, fr_i32 ValueSize);
	bool ResetSoundOption(CBaseSound* pNeedySound, fr_i32 OptionIndex);

	bool AddEffect(CBaseSound* pNeedySound, IBaseEffect* pEffectToClone);
	bool RemoveEffect(CBaseSound* pNeedySound, IBaseEffect* pEffectToRemove);

	bool Record(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) override;
	bool Update(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) override;
	bool Render(fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) override;
	bool Flush() override {}
};
