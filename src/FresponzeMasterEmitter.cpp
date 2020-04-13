#include "FresponzeMasterEmitter.h"

IPLhandle context = nullptr;
long Counter = 0;

void
CSteamAudioEmitter::Reset()
{

}

CSteamAudioEmitter::CSteamAudioEmitter()
{
	AddRef();
	if (!context) iplCreateContext(nullptr, nullptr, nullptr, &context);
	Counter++;
}

CSteamAudioEmitter::~CSteamAudioEmitter()
{
	IMediaListener* pTemp = ((IMediaListener*)pParentListener);
	_RELEASE(pTemp);
	Counter--;
	if (Counter <= 0) {
		iplDestroyContext(&context);
		context = nullptr;
	}
}

void	
CSteamAudioEmitter::SetListener(void* pListener) 
{
	IMediaListener* pMediaListener = (IMediaListener*)pListener;
	pParentListener = pListener;
	pMediaListener->GetFormat(outputFormat);

	/* Steam audio API settings apply */
	EmitterSettings.convolutionType = IPL_CONVOLUTIONTYPE_PHONON;
	EmitterSettings.frameSize = outputFormat.Frames;
	EmitterSettings.samplingRate = outputFormat.SampleRate;
	hrtfParams.type = IPL_HRTFDATABASETYPE_DEFAULT;
	iplCreateBinauralRenderer(context, EmitterSettings, hrtfParams, &BinauralRender);
	EmitterFormat.channelLayoutType = IPL_CHANNELLAYOUTTYPE_SPEAKERS;
	EmitterFormat.channelLayout =
		outputFormat.Channels == 2 ? IPL_CHANNELLAYOUT_STEREO		: outputFormat.Channels == 4 ? IPL_CHANNELLAYOUT_QUADRAPHONIC :
		outputFormat.Channels == 6 ? IPL_CHANNELLAYOUT_FIVEPOINTONE : outputFormat.Channels == 8 ? IPL_CHANNELLAYOUT_SEVENPOINTONE : 
		outputFormat.Channels == 1 ? IPL_CHANNELLAYOUT_MONO			: IPL_CHANNELLAYOUT_STEREO;
	EmitterFormat.channelOrder = IPL_CHANNELORDER_DEINTERLEAVED;

	iplCreateBinauralEffect(BinauralRender, EmitterFormat, EmitterFormat, &BinauralEffect);
	InputBuffer.format = EmitterFormat;
	OutputBuffer.format = EmitterFormat;
	InputBuffer.numSamples = outputFormat.Frames;
	OutputBuffer.numSamples = outputFormat.Frames;
	InputBuffer.deinterleavedBuffer = (IPLfloat32**)FastMemAlloc(outputFormat.Channels * sizeof(void*));
	OutputBuffer.deinterleavedBuffer = (IPLfloat32**)FastMemAlloc(outputFormat.Channels * sizeof(void*));
	for (size_t i = 0; i < outputFormat.Channels; i++) {
		InputBuffer.deinterleavedBuffer[i] = (IPLfloat32*)FastMemAlloc(outputFormat.Frames * sizeof(fr_f32));
		OutputBuffer.deinterleavedBuffer[i] = (IPLfloat32*)FastMemAlloc(outputFormat.Frames * sizeof(fr_f32));
	}
}

void	
CSteamAudioEmitter::SetState(fr_i32 state)
{
	EmittersState = state;
}

void	
CSteamAudioEmitter::SetPosition(fr_i64 FPosition) 
{
	FilePosition = FPosition;
}

void*	
CSteamAudioEmitter::GetListener()
{
	return pParentListener;
}

fr_i32	
CSteamAudioEmitter::GetState()
{
	return EmittersState;
}

fr_i64	
CSteamAudioEmitter::GetPosition()
{
	return FilePosition;
}

bool	
CSteamAudioEmitter::GetEffectCategory(fr_i32& EffectCategory)
{
	return true;
}

bool	
CSteamAudioEmitter::GetEffectType(fr_i32& EffectType) 
{
	return true;
}

bool	
CSteamAudioEmitter::GetPluginName(fr_string64& DescriptionString)
{
	strcpy(DescriptionString, "Steam Audio Emitter");
	return true;
}

bool	
CSteamAudioEmitter::GetPluginVendor(fr_string64& DescriptionString) 
{
	strcpy(DescriptionString, "Valve Inc.");
	return true;
}

bool	
CSteamAudioEmitter::GetPluginDescription(fr_string256& DescriptionString) 
{
	strcpy(DescriptionString, "Steam Audio Emitter for HRTF audio");
	return true;
}

bool	
CSteamAudioEmitter::GetVariablesCount(fr_i32& CountOfVariables) 
{
	CountOfVariables = eCountOfParameters;
	return true;
}

bool	
CSteamAudioEmitter::GetVariableDescription(fr_i32 VariableIndex, fr_string128& DescriptionString) 
{
	if (VariableIndex >= ePluginParametersCount) return false;
	return true;
}

bool	
CSteamAudioEmitter::GetVariableKnob(fr_i32 VariableIndex, fr_i32& KnobType)
{
	if (VariableIndex >= ePluginParametersCount) return false;
	return true;
}

void	
CSteamAudioEmitter::SetOption(fr_i32 Option, fr_f32* pData, fr_i32 DataSize) 
{
	if (!pData) return;
	if (Option >= ePluginParametersCount) return;
	if (DataSize != sizeof(fr_f32)) return;
	fr_f32 ValueToApply = *pData;


	switch (Option)
	{
	case eVolumeParameter:		VolumeLevel = ValueToApply;
	case eXAxis:				vectorOfAngle.x = ValueToApply;
	case eYAxis:				vectorOfAngle.y = ValueToApply;
	case eZAxis:				vectorOfAngle.z = ValueToApply;
	default:
		break;
	}
}

void	
CSteamAudioEmitter::GetOption(fr_i32 Option, fr_f32* pData, fr_i32 DataSize)
{
	if (!pData) return;
	if (Option >= ePluginParametersCount) return;
	if (DataSize != sizeof(fr_f32)) return;
	fr_f32& ValueToApply = *pData;

	switch (Option)
	{
	case eVolumeParameter:		ValueToApply = VolumeLevel;
	case eXAxis:				ValueToApply = vectorOfAngle.x;
	case eYAxis:				ValueToApply = vectorOfAngle.y;
	case eZAxis:				ValueToApply = vectorOfAngle.z;
	default:
		break;
	}
}

bool	
CSteamAudioEmitter::Process(fr_f32** ppData, fr_i32 Frames)
{
	if (!pParentListener) return false;
	fr_i32 BaseEmitterPosition = 0;
	fr_i32 BaseListenerPosition = 0;
	fr_i32 FramesReaded = 0;
	IMediaListener* ThisListener = (IMediaListener*)pParentListener;
	PcmFormat ListenerFormat = {};

	if (EmittersState == eStopState || EmittersState == ePauseState) return false;

	/* Get current position of listener and emitter to reset old state */
	ThisListener->GetFormat(ListenerFormat);
	if (memcmp(&ListenerFormat, &outputFormat, sizeof(PcmFormat))) Reset();
	BaseEmitterPosition = GetPosition();
	BaseListenerPosition = ThisListener->GetPosition();

	/* Set emitter position to listener and read data */
	ThisListener->SetPosition((fr_i64)BaseEmitterPosition);
	FramesReaded = ThisListener->Process(ppData, Frames);
	if (FramesReaded < Frames) {
		/* We don't want replay audio if we set this flag */
		if (EmittersState == ePlayState) EmittersState = eStopState;
		BaseEmitterPosition = 0;
	}
	else {
		BaseEmitterPosition += FramesReaded;
	}

	static bool state = false;
	static fr_f32 phase = 0.f;

	/* Process by emitter effect */
	for (size_t i = 0; i < outputFormat.Channels; i++) {
		memcpy(InputBuffer.deinterleavedBuffer[i], ppData[i], sizeof(fr_f32) * Frames);
	}

	iplApplyBinauralEffect(BinauralEffect, BinauralRender, InputBuffer, IPLVector3{ 2.0f * sinf(phase), 0.0f, 2.0f * cosf(phase) }, IPL_HRTFINTERPOLATION_NEAREST, OutputBuffer);
	for (size_t i = 0; i < outputFormat.Channels; i++) {
		memcpy(ppData[i], OutputBuffer.deinterleavedBuffer[i], sizeof(fr_f32) * Frames);
	}

	phase += 0.1f;

	SetPosition(BaseEmitterPosition);
	ThisListener->SetPosition((fr_i64)BaseListenerPosition);
}