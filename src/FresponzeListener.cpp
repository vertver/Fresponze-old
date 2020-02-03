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
	return SetPosition(fr_i64(((fr_f32)ListenerFormat.Frames * FloatPosition)));
}

fr_i32	
CMediaListener::SetPosition(fr_i64 FramePosition)
{
	fr_i32 outputFrames = 0;
	CalculateFrames(FramePosition, ListenerFormat.SampleRate, ResourceFormat.SampleRate, outputFrames);
	return (fr_i32)pLocalResource->SetPosition(outputFrames);
}

fr_i32 
CMediaListener::GetFullFrames()
{
	return ListenerFormat.Frames;
}

fr_i32	
CMediaListener::GetFormat(PcmFormat& fmt)
{
	fmt = ListenerFormat;
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
CMediaListener::Flush(fr_f32** ppOutputFloatData)
{
	return 0;
}
