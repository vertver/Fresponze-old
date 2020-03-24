#include "FresponzeListener.h"

CMediaListener::CMediaListener(IMediaResource* pInitialResource)
{
	pInitialResource->Clone((void**)&pLocalResource);
	pLocalResource->GetFormat(ResourceFormat);
	ListenerFormat = ResourceFormat;
}

CMediaListener::~CMediaListener()
{
	if (pLocalResource) _RELEASE(pLocalResource);
}

bool	
CMediaListener::SetResource(IMediaResource* pInitialResource)
{
	if (pLocalResource) _RELEASE(pLocalResource);
	return pInitialResource->Clone((void**)&pLocalResource);
}

bool	
CMediaListener::SetListenerState(fr_i32 State)
{
	CurrentState = State;
	return true;
}

fr_i32	
CMediaListener::SetPosition(fr_f32 FloatPosition)
{
	return SetPosition(fr_i64(((fr_f64)ListenerFormat.Frames * FloatPosition)));
}

fr_i32	
CMediaListener::SetPosition(fr_i64 FramePosition)
{
	fr_i64 outputFrames = 0;
	CalculateFrames64(FramePosition, ListenerFormat.SampleRate, ResourceFormat.SampleRate, outputFrames);
	return (fr_i32)pLocalResource->SetPosition(outputFrames);
}

fr_i32 
CMediaListener::GetFullFrames()
{
	fr_i64 outputFrames = 0;
	CalculateFrames64(ResourceFormat.Frames, ResourceFormat.SampleRate, ListenerFormat.SampleRate, outputFrames);
	return outputFrames;
}

fr_i32	
CMediaListener::GetFormat(PcmFormat& fmt)
{
	fmt = ResourceFormat;
	return 0;
}

fr_i32
CMediaListener::SetFormat(PcmFormat fmt)
{
	ListenerFormat = fmt;
	pLocalResource->SetFormat(ListenerFormat);
	return 0;
}

fr_i32	
CMediaListener::Process(fr_f32** ppOutputFloatData, fr_i32 frames)
{
	switch (CurrentState)
	{
	case eStopState:
	case ePauseState:
		break;
	case ePlayState:
	case eReplayState:
		framesPos = pLocalResource->Read(frames, ppOutputFloatData) ? 0 : -1;
	default:
		break;
	}

	return frames;
}
