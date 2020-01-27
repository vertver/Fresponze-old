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
#include "FresponzeTypes.h"

enum EListenerState
{
	eStopState,
	ePauseState,
	ePlayState
};

class IMediaListener : public IBaseInterface
{
protected:
	fr_i32 CurrentState = 0;

public:
	virtual void Initialize() = 0;
	virtual void Destroy() = 0;

	virtual bool SetListenerState(fr_i32 State) = 0;

	virtual fr_i32 SetPosition(fr_f32 floatPosition) = 0;
	virtual fr_i32 SetPosition(fr_i64 framePosition) = 0;

	virtual fr_i32 GetFormat(PcmFormat& fmt) = 0;

	virtual fr_i32 Flush(fr_f32** ppOutputFloatData) = 0;
};
