/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2020. All rights reserved.
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
CMediaListener::AddEmitter(IBaseEmitter* pNewEmitter)
{
	return true;
}

bool 
CMediaListener::DeleteEmitter(IBaseEmitter* pEmitter)
{
	return true;
}

bool 
CMediaListener::GetEmitter(IBaseEmitter* pEmitter)
{
	return true;
}

bool	
CMediaListener::SetResource(IMediaResource* pInitialResource)
{
	if (pLocalResource) _RELEASE(pLocalResource);
	return pInitialResource->Clone((void**)&pLocalResource);
}

fr_i32	
CMediaListener::SetPosition(fr_f32 FloatPosition)
{
	return SetPosition(fr_i64(((fr_f64)ResourceFormat.Frames * FloatPosition)));
}

fr_i32	
CMediaListener::SetPosition(fr_i64 FramePosition)
{
	fr_i64 outputFrames = 0;
	CalculateFrames64(FramePosition, ListenerFormat.SampleRate, ResourceFormat.SampleRate, outputFrames);
	return (fr_i32)pLocalResource->SetPosition(outputFrames );
}

fr_i32 
CMediaListener::GetFullFrames()
{
	fr_i64 outputFrames = 0;
	CalculateFrames64(ResourceFormat.Frames, ResourceFormat.SampleRate, ListenerFormat.SampleRate, outputFrames);
	return (fr_i32)outputFrames;
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
	fr_i32 inFrames = 0;
	inFrames = (fr_i32)pLocalResource->Read(frames, ppOutputFloatData);
	if (inFrames < frames) framesPos = 0;
	else framesPos += inFrames;
	return inFrames;
}
