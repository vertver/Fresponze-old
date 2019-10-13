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
#include "FresponzeMixer.h"

enum GameMixerSoundOptions
{
	
};

class CGameMixer final : public IAudioMixer
{
private:
	EffectNodeStruct* pReverbBusFirstEffect = nullptr;
	EffectNodeStruct* pBeforeMixingBusFirstEffect = nullptr;
	CFloatBuffer ReverbBus;
	CFloatBuffer BeforeMixingBus;

	EffectNodeStruct* FindEffectNode(IBaseEffect* pEffect);
	SoundNodeStruct* FindSoundNode(CBaseSound* pSound);

public:
	CGameMixer()
	{
		pAudioCallback = new CAudioCallback(this);
	}

	bool SetMixFormat(PcmFormat& NewFormat) override;
	bool AddSound(CBaseSound* pNeedySound) override;
	bool DeleteSound(CBaseSound* pNeedySound) override;
	bool PlaySoundById(CBaseSound* pNeedySound) override;
	bool PauseSound(CBaseSound* pNeedySound) override;
	bool StopSound(CBaseSound* pNeedySound) override;

	bool EnableInputPlay(bool bPlay) override;
	bool AddInputEffect(IBaseEffect* pEffectToClone) override;
	bool RemoveInputEffect(IBaseEffect* pEffectToRemove) override;
	bool SetInputEffectOption(IBaseEffect* pEffect, fr_i32 OptionIndex, fr_f32* ValueToSet, fr_i32 ValueSize)  override;

	bool SetDefaultSoundOptions(fr_f32* pOptions) override;
	bool SetSoundOption(CBaseSound* pNeedySound, fr_i32 OptionIndex, fr_f32* ValueToSet, fr_i32 ValueSize) override;
	bool ResetSoundOption(CBaseSound* pNeedySound, fr_i32 OptionIndex) override;

	bool AddEffect(CBaseSound* pNeedySound, IBaseEffect* pEffectToClone) override;
	bool RemoveEffect(CBaseSound* pNeedySound, IBaseEffect* pEffectToRemove) override;

	bool Record(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) override;
	bool Update(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) override;
	bool Render(fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) override;

	~CGameMixer()
	{
		if (pAudioCallback) delete pAudioCallback;
	}
};
