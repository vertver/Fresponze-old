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
#include "FresponzeEmitter.h"

IBaseEmitter*
GetAdvancedEmitter()
{
	return new CAdvancedEmitter;
}

CAdvancedEmitter::CAdvancedEmitter()
{
	AddRef();
}

CAdvancedEmitter::~CAdvancedEmitter()
{
	IMediaListener* pTemp = ((IMediaListener*)pParentListener);
	_RELEASE(pTemp);
}

/* Base emitter code (parent source, position, state) */
void   
CAdvancedEmitter::SetListener(void* pListener)
{
	IMediaListener* pTemp = (IMediaListener*)pListener;
	pTemp->Clone(&pParentListener);
}

void*
CAdvancedEmitter::GetListener()
{
	return pParentListener;
}

void   
CAdvancedEmitter::SetState(fr_i32 state)
{
	EmittersState = state;
}

fr_i32
CAdvancedEmitter::GetState()
{
	return EmittersState;
}

void   
CAdvancedEmitter::SetPosition(fr_i64 FPosition)
{
	FilePosition = FPosition;
}

fr_i64 
CAdvancedEmitter::GetPosition()
{
	return FilePosition;
}

/* Advanced emitter processing code */
bool 
CAdvancedEmitter::GetEffectCategory(fr_i32& EffectCategory)
{
	EffectCategory = EmitterEffectCategory;
	return true;
}

bool 
CAdvancedEmitter::GetEffectType(fr_i32& EffectType)
{
	EffectType = EmitterEffectType;
	return true;
}

bool 
CAdvancedEmitter::GetPluginName(fr_string64& DescriptionString) 
{
	strcpy(DescriptionString, EmitterName);
	return true;
}

bool 
CAdvancedEmitter::GetPluginVendor(fr_string64& DescriptionString) 
{
	strcpy(DescriptionString, EmitterVendor);
	return true;
}

bool 
CAdvancedEmitter::GetPluginDescription(fr_string256& DescriptionString)
{
	strcpy(DescriptionString, EmitterDescription);
	return true;
}

bool 
CAdvancedEmitter::GetVariablesCount(fr_i32& CountOfVariables) 
{
	CountOfVariables = ePluginParametersCount;
	return true;
}

bool 
CAdvancedEmitter::GetVariableDescription(fr_i32 VariableIndex, fr_string128& DescriptionString)
{
	if (VariableIndex >= ePluginParametersCount) return false;
	strcpy(DescriptionString, EmitterConfigurationDescription[VariableIndex]);
	return true;
}

bool 
CAdvancedEmitter::GetVariableKnob(fr_i32 VariableIndex, fr_i32& KnobType) 
{
	if (VariableIndex >= ePluginParametersCount) return false;
	KnobType = EmitterConfigurationKnob[VariableIndex];
	return true;
}

void 
CAdvancedEmitter::SetOption(fr_i32 Option, fr_f32* pData, fr_i32 DataSize)
{
	if (!pData) return;
	if (Option >= ePluginParametersCount) return;
	if (DataSize != sizeof(fr_f32)) return;
	fr_f32 ValueToApply = *pData;

	switch (Option)
	{
	case eVolumeParameter:		VolumeLevel = ValueToApply;
	case eAngleParameter:		Angle = ValueToApply;
	case eLPParameterResonance: LowPassSettings.FilterResonance = ValueToApply;
	case eLPParameterFrequency:	LowPassSettings.FilterFrequency = ValueToApply;
	case eLPParameterVolume:	LowPassSettings.FilterVolume = ValueToApply;
	case eHPParameterResonance: HighPassSettings.FilterResonance = ValueToApply;
	case eHPParameterFrequency:	HighPassSettings.FilterFrequency = ValueToApply;
	case eHPParameterVolume:	HighPassSettings.FilterVolume = ValueToApply;
	default:
		break;
	}
}

void 
CAdvancedEmitter::GetOption(fr_i32 Option, fr_f32* pData, fr_i32 DataSize)
{
	if (!pData) return;
	if (Option >= ePluginParametersCount) return;
	if (DataSize != sizeof(fr_f32)) return;
	fr_f32 ValueToApply = *pData;

	switch (Option)
	{
	case eVolumeParameter:		VolumeLevel = ValueToApply;
	case eAngleParameter:		Angle = ValueToApply;
	case eLPParameterResonance: LowPassSettings.FilterResonance = ValueToApply;
	case eLPParameterFrequency:	LowPassSettings.FilterFrequency = ValueToApply;
	case eLPParameterVolume:	LowPassSettings.FilterVolume = ValueToApply;
	case eHPParameterResonance: HighPassSettings.FilterResonance = ValueToApply;
	case eHPParameterFrequency:	HighPassSettings.FilterFrequency = ValueToApply;
	case eHPParameterVolume:	HighPassSettings.FilterVolume = ValueToApply;
	default:
		break;
	}
}

void
CAdvancedEmitter::ProcessInternal(fr_f32** ppData, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate)
{
	fr_f32* LowPasArray = FiltersTempValues[0];
	fr_f32* HighPasArray = FiltersTempValues[1];
	fr_f32 LowPassFeedback = LowPassSettings.FilterResonance * (1.0f + 1.0f / (1.0f - LowPassSettings.FilterFrequency));
	fr_f32 HighPassFeedback = HighPassSettings.FilterResonance * (1.0f + 1.0f / (1.0f - HighPassSettings.FilterFrequency));
	fr_f32 fLowFreq = LowPassSettings.FilterFrequency;
	fr_f32 fHighFreq = HighPassSettings.FilterFrequency;

	//for (size_t i = 0; i < Channels; i++) {
	//	fr_f32 TempValue = 0.f;
	//	fr_f32 TempValue2 = 0.f;
	//	fr_f32* pLowPassTemp = LowPassTempArray[i];
	//	fr_f32* pHighPassTemp = HighPassTempArray[i];
	//
	//	for (size_t o = 0; o < Frames; o++) {
	//		TempValue = ppData[i][o];
	//		pLowPassTemp[0] += fLowFreq * (TempValue - pLowPassTemp[0] + LowPassFeedback * (pLowPassTemp[0] - pLowPassTemp[1]));
	//		pLowPassTemp[1] += fLowFreq * (pLowPassTemp[0] - pLowPassTemp[1]);
	//		pLowPassTemp[2] += fLowFreq * (pLowPassTemp[1] - pLowPassTemp[2]);
	//		pLowPassTemp[3] += fLowFreq * (pLowPassTemp[2] - pLowPassTemp[3]);
	//		ppData[i][o] = pLowPassTemp[3];
	//
	//		/* Apple volume gain */
	//		ppData[i][o] *= VolumeLevel;
	//	}
	//}

	/* Apple angle to signal */
	if (Channels >= 2) {
		for (size_t o = 0; o < Frames; o++) {
			ppData[0][o] *= (0.707106f * (cosf(Angle) - sinf(Angle)));
			ppData[1][o] *= (0.707106f * (cosf(Angle) + sinf(Angle)));
		}
	}
}

bool 
CAdvancedEmitter::Process(fr_f32** ppData, fr_i32 Frames) 
{
	if (!pParentListener) return false;
	fr_i32 BaseEmitterPosition = 0;
	fr_i32 BaseListenerPosition =0;
	fr_i32 FramesReaded = 0;
	IMediaListener* ThisListener = (IMediaListener*)pParentListener;
	PcmFormat ListenerFormat = {};

	/* Get current position of listener and emitter to reset old state */
	ThisListener->GetFormat(ListenerFormat);
	BaseEmitterPosition = GetPosition();
	BaseListenerPosition = ThisListener->GetPosition();

	/* Set emitter position to listener and read data */
	ThisListener->SetPosition((fr_i64)BaseEmitterPosition);
	FramesReaded = ThisListener->Process(ppData, Frames);
	if (FramesReaded < Frames) {
		/* We don't want replay audio if we set this flag */
		if (EmittersState == ePlayState) EmittersState = eStopState;
		BaseEmitterPosition = 0;
	} else {
		BaseEmitterPosition += FramesReaded;
	}

	/* Process by emitter effect */
	ProcessInternal(ppData, Frames, ListenerFormat.Channels, ListenerFormat.SampleRate);

	SetPosition(BaseEmitterPosition);
	ThisListener->SetPosition((fr_i64)BaseListenerPosition);

	return true;
}

