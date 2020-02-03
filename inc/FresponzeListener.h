/*********************************************************************
* Copyright (C) Anton Kovalev (Anton Kovalev (vertver)), 2019. All rights reserved.
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
#include "FresponzeMediaResource.h"

enum EListenerState
{
	eStopState,
	ePauseState,
	ePlayState
};

class CMediaListener : public IBaseInterface
{
protected:
	fr_i32 CurrentState = 0;
	PcmFormat ResourceFormat = {};
	PcmFormat ListenerFormat = {};
	IMediaResource* pLocalResource = nullptr;

public:
	/*
		Resource prototype must include constructor with initial media 
		resource interface (IMediaResource). It's can be either 
		internal Opus/Vorbis decoder or a custom decoder module
		from other DLL.
	*/
	CMediaListener(IMediaResource* pInitialResource = nullptr);
	~CMediaListener();

	bool SetResource(IMediaResource* pInitialResource);
	bool SetListenerState(fr_i32 State);

	fr_i32 SetPosition(fr_f32 FloatPosition);		// 0.0f to 1.0f
	fr_i32 SetPosition(fr_i64 FramePosition);		// 0 to x (end) 

	fr_i32 GetFullFrames();

	fr_i32 GetFormat(PcmFormat& fmt);
	fr_i32 SetFormat(PcmFormat fmt);

	fr_i32 Flush(fr_f32** ppOutputFloatData);
};

struct ListenersNode;
struct ListenersNode
{
	ListenersNode* pNext = nullptr;
	ListenersNode* pPrev = nullptr;
	CMediaListener* pListener = nullptr;
};
