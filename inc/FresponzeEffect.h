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
#include "FresponzeTypes.h"

enum EEffectType : fr_i32
{
	UnknownEffectType,		// No effect
	SoundEffectType,		// For single sound or input signal
	PreMixEffect,			// For pre-master state, for check audio engine picture 
	AfterMixEffect, 		// For master-channel
	FFTEffectType			// Special FFT-effect, in FFT chain
};

enum EKnobType : fr_i32
{
	NoKnobType,		// Text
	CircleKnob,		// Circle-like knob
	LineKnob,		// Flat knob
	CustomKnob		// Custom knob, just draw it yourself
};

class IBaseEffect : public IBaseInterface
{
protected:

public:
	virtual bool GetEffectCategory(fr_i32& EffectCategory) = 0;			// Use BasePluginCategory enum here
	virtual bool GetEffectType(fr_i32& EffectTpye) = 0;

	virtual bool GetPluginName(fr_string64& DescriptionString) = 0;
	virtual bool GetPluginVendor(fr_string64& DescriptionString) = 0;

	virtual bool GetVariablesCount(fr_i32& CountOfVariables) = 0;
	virtual bool GetVariableDescription(fr_i32 VariableIndex, fr_string128& DescriptionString) = 0;
	virtual bool GetVariableKnob(fr_i32 VariableIndex, fr_i32& KnobType) = 0;
	virtual bool SetVariable(fr_i32 VariableIndex, fr_f32 VariableValue) = 0;
	virtual bool GetVariable(fr_f32 VariableIndex, fr_f32& VariableValue) = 0;

	virtual bool Process(fr_i32** ppData, fr_i32 Frames) = 0;
}; 

struct EffectNodeStruct;
struct EffectNodeStruct
{
	EffectNodeStruct* pNext;
	IBaseEffect* pEffect;  
	void* pModuleHandle;  
	void* pReserved;
};

struct SoundNodeStruct;
struct SoundNodeStruct
{
	SoundNodeStruct* pNext;
	EffectNodeStruct* pFirstEffectNode;
	CBaseSound* pSound;
	fr_f32* pSoundOptions;
};
