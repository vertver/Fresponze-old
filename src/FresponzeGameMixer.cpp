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
#include "FresponzeGameMixer.h"

#ifdef WINDOWS_PLATFORM
extern void* hModule = nullptr;
#endif

EffectNodeStruct* 
CGameMixer::FindEffectNode(IBaseEffect* pEffect)
{
	EffectNodeStruct* pParentEffectNode = pInputFirstEffect;
	EffectNodeStruct* pThisEffectNode = nullptr;

	if (pParentEffectNode->pEffect == pEffect) {
		return pParentEffectNode;
	}
	else {
		while (true) {
			pThisEffectNode = pParentEffectNode->pNext;
			if (!pThisEffectNode) return nullptr;
			if (pThisEffectNode->pEffect == pEffect) break;
			pParentEffectNode = pThisEffectNode;
		}

		return pThisEffectNode;
	}
}

SoundNodeStruct*
CGameMixer::FindSoundNode(CBaseSound* pSound)
{
	SoundNodeStruct* pParentNode = pSoundsNode;
	SoundNodeStruct* pThisNode = nullptr;

	/* Try to find node with our sound */
	if (!pSound || !pParentNode) return nullptr;
	if (pParentNode->pSound == pSound) {
		pThisNode = pParentNode;
	}
	else {
		while (true) {
			pThisNode = pParentNode->pNext;
			if (!pThisNode) return nullptr;
			if (pThisNode->pSound == pSound) break;
			pParentNode = pThisNode;
		}
	}

	return pThisNode;
}

bool
CGameMixer::SetMixFormat(PcmFormat& NewFormat)
{
	memcpy(&MixFormat, &NewFormat, sizeof(PcmFormat));
	return true;
}

bool
CGameMixer::AddSound(CBaseSound* pNeedySound)
{
	SoundNodeStruct* pParentNode = pSoundsNode;
	SoundNodeStruct* pThisNode = nullptr;

	if (!pNeedySound) return false;
	while (pParentNode) {
		if (pParentNode->pNext) {
			pParentNode = pParentNode->pNext;
		} else break;
	}

	pThisNode = (SoundNodeStruct*)FastMemAlloc(sizeof(SoundNodeStruct));
	pNeedySound->Clone((void**)&pThisNode->pSound);
	if (pParentNode) pParentNode->pNext = pThisNode;
	else {
		pParentNode = pThisNode;
		pSoundsNode = pParentNode;
	}
	return true;
}

bool 
CGameMixer::DeleteSound(CBaseSound* pNeedySound)
{
	SoundNodeStruct* pParentNode = pSoundsNode;
	SoundNodeStruct* pThisNode = nullptr;
	EffectNodeStruct* pThisEffectNode = nullptr;
	EffectNodeStruct* pNextEffectNode = nullptr;

	/* Try to find node with our sound */
	if (!pNeedySound || !pParentNode) return false;
	if (pParentNode->pSound == pNeedySound) {
		pThisNode = pParentNode;
		pParentNode = nullptr;
	} else {
		while (true) {
			pThisNode = pParentNode->pNext;
			if (!pThisNode) return false;
			if (pThisNode->pSound == pNeedySound) break;
			pParentNode = pThisNode;
		}
	}

	/* Free effect node tree */
	pThisEffectNode = pThisNode->pFirstEffectNode;
	while (pThisEffectNode) {
		pNextEffectNode = pThisEffectNode->pNext;
		_RELEASE(pThisEffectNode->pEffect);
		if (pThisEffectNode) FreeFastMemory(pThisEffectNode);
		pThisEffectNode = pNextEffectNode;
	}

	/* Free sound node struct */
	if (pParentNode) pParentNode->pNext = pThisNode->pNext;
	_RELEASE(pThisNode->pSound);
	if (pThisNode->pSoundOptions) FreeFastMemory(pThisNode->pSoundOptions);
	FreeFastMemory(pThisNode);
	return true;
}

bool 
CGameMixer::PlaySoundById(CBaseSound* pNeedySound)
{
	SoundNodeStruct* pThisNode = nullptr;

	/* Try to find node with our sound */
	pThisNode = FindSoundNode(pNeedySound);
	if (!pThisNode) return false;

	pThisNode->pSound->SetState(PlayingState);
	return true;
}

bool 
CGameMixer::PauseSound(CBaseSound* pNeedySound)
{
	SoundNodeStruct* pThisNode = nullptr;

	/* Try to find node with our sound */
	pThisNode = FindSoundNode(pNeedySound);
	if (!pThisNode) return false;

	pThisNode->pSound->SetState(PausedState);
	return true;
}

bool 
CGameMixer::StopSound(CBaseSound* pNeedySound)
{
	SoundNodeStruct* pThisNode = nullptr;

	/* Try to find node with our sound */
	pThisNode = FindSoundNode(pNeedySound);
	if (!pThisNode) return false;

	pThisNode->pSound->SetState(StoppedState);
	return true;
}

bool 
CGameMixer::EnableInputPlay(bool bPlay)
{
	InputState = bPlay ? PlayingState : StoppedState;
	return true;
}

bool 
CGameMixer::AddInputEffect(IBaseEffect* pEffectToClone)
{
	EffectNodeStruct* pThisEffectNode = pInputFirstEffect;
	EffectNodeStruct* pNextEffectNode = nullptr;

	if (!pThisEffectNode) {
		pInputFirstEffect = (EffectNodeStruct*)FastMemAlloc(sizeof(EffectNodeStruct));
		pEffectToClone->Clone((void**)&pInputFirstEffect->pEffect);
		pInputFirstEffect->pModuleHandle = hModule;
	} else {
		while (true) {
			pNextEffectNode = pThisEffectNode->pNext;
			if (!pNextEffectNode) break;

			pThisEffectNode = pNextEffectNode;
		}
	
		pThisEffectNode->pNext = (EffectNodeStruct*)FastMemAlloc(sizeof(EffectNodeStruct));
		pEffectToClone->Clone((void**)&pThisEffectNode->pNext->pEffect);
		pThisEffectNode->pNext->pModuleHandle = hModule;
	}

	return true;
}

bool 
CGameMixer::RemoveInputEffect(IBaseEffect* pEffectToRemove)
{
	EffectNodeStruct* pParentEffectNode = pInputFirstEffect;
	EffectNodeStruct* pThisEffectNode = nullptr;

	if (!pParentEffectNode) return false;
	if (pParentEffectNode->pEffect == pEffectToRemove) {
		pInputFirstEffect = pParentEffectNode->pNext;
		_RELEASE(pParentEffectNode->pEffect);
		FreeFastMemory(pParentEffectNode);
	} else {
		while (true) {
			pThisEffectNode = pParentEffectNode->pNext;
			if (!pThisEffectNode) return false;
			if (pThisEffectNode->pEffect == pEffectToRemove) break;
			pParentEffectNode = pThisEffectNode;
		}

		pParentEffectNode->pNext = pThisEffectNode->pNext;
		_RELEASE(pThisEffectNode->pEffect);
		FreeFastMemory(pThisEffectNode);
		return true;
	}

	return true;
}

bool
CGameMixer::SetDefaultSoundOptions(fr_f32* pOptions)
{
	/*
		#TODO
	*/
}

bool 
CGameMixer::SetSoundOption(CBaseSound* pNeedySound, fr_i32 OptionIndex, fr_f32* ValueToSet, fr_i32 ValueSize)
{
	SoundNodeStruct* pThisNode = nullptr;

	/* Try to find node with our sound */
	pThisNode = FindSoundNode(pNeedySound);
	if (!pThisNode || pThisNode->pSoundOptions) return false;

	switch (ValueSize)
	{
		case sizeof(fr_f32) :
			pThisNode->pSoundOptions[OptionIndex] = *ValueToSet;
			break;
		default:
			break;
	}

	return true;
}

bool 
CGameMixer::ResetSoundOption(CBaseSound* pNeedySound, fr_i32 OptionIndex)
{
	SoundNodeStruct* pParentNode = pSoundsNode;
	SoundNodeStruct* pThisNode = nullptr;

	/* Try to find node with our sound */
	pThisNode = FindSoundNode(pNeedySound);
	if (!pThisNode || pThisNode->pSoundOptions) return false;

	/*
		#TODO
	*/

	return true;
}

bool 
CGameMixer::AddEffect(CBaseSound* pNeedySound, IBaseEffect* pEffectToClone)
{
	EffectNodeStruct* pThisEffectNode = nullptr;
	EffectNodeStruct* pNextEffectNode = nullptr;
	SoundNodeStruct* pThisNode = nullptr;

	/* Try to find node with our sound */
	pThisNode = FindSoundNode(pNeedySound);
	if (!pThisNode) return false;

	pThisEffectNode = pThisNode->pFirstEffectNode;
	if (!pThisEffectNode) {
		pThisNode->pFirstEffectNode = (EffectNodeStruct*)FastMemAlloc(sizeof(EffectNodeStruct));
		pEffectToClone->Clone((void**)&pThisNode->pFirstEffectNode->pEffect);
		pThisNode->pFirstEffectNode->pModuleHandle = hModule;
	}
	else {
		while (true) {
			pNextEffectNode = pThisEffectNode->pNext;
			if (!pNextEffectNode) break;

			pThisEffectNode = pNextEffectNode;
		}

		pThisEffectNode->pNext = (EffectNodeStruct*)FastMemAlloc(sizeof(EffectNodeStruct));
		pEffectToClone->Clone((void**)&pThisEffectNode->pNext->pEffect);
		pThisEffectNode->pNext->pModuleHandle = hModule;
	}

	return true;
}

bool 
CGameMixer::RemoveEffect(CBaseSound* pNeedySound, IBaseEffect* pEffectToRemove)
{
	EffectNodeStruct* pParentEffectNode = nullptr;
	EffectNodeStruct* pThisEffectNode = nullptr;
	SoundNodeStruct* pThisNode = nullptr;

	/* Try to find node with our sound */
	pThisNode = FindSoundNode(pNeedySound);
	if (!pThisNode) return false;

	pParentEffectNode = pThisNode->pFirstEffectNode;
	if (!pParentEffectNode) return false;
	if (pParentEffectNode->pEffect == pEffectToRemove) {
		pThisNode->pFirstEffectNode = pParentEffectNode->pNext;
		_RELEASE(pParentEffectNode->pEffect);
		FreeFastMemory(pParentEffectNode);
	}
	else {
		while (true) {
			pThisEffectNode = pParentEffectNode->pNext;
			if (!pThisEffectNode) return false;
			if (pThisEffectNode->pEffect == pEffectToRemove) break;
			pParentEffectNode = pThisEffectNode;
		}

		pParentEffectNode->pNext = pThisEffectNode->pNext;
		_RELEASE(pThisEffectNode->pEffect);
		FreeFastMemory(pThisEffectNode);
		return true;
	}
}

bool 
CGameMixer::Record(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	return false;
}

bool 
CGameMixer::Update(fr_f32* pBuffer, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	return false;
}

bool 
CGameMixer::Render(fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	return false;
}
