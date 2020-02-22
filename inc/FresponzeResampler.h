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
#include "CDSPResampler.h"

class CR8BrainResampler
{
private:
	bool lin = false;
	fr_i32 bufLength = 0;
	fr_i32 inSRate = 0;
	fr_i32 outSRate = 0;
	fr_i32 channels = 0;
	r8b::CDSPResampler24* resampler[MAX_CHANNELS] = {};

public:
	~CR8BrainResampler()
	{
		Destroy();
	}

	void Initialize(fr_i32 MaxBufferIn, fr_i32 InputSampleRate, fr_i32 OutputSampleRate, fr_i32 ChannelsCount, bool isLinear)
	{
		bufLength = MaxBufferIn;
		inSRate = InputSampleRate;
		outSRate = OutputSampleRate;
		channels = ChannelsCount;
		for (size_t i = 0; i < ChannelsCount; i++){
			resampler[i] = new r8b::CDSPResampler24(inSRate, outSRate, bufLength);
		}	
	}

	void Destroy()
	{
		size_t index = 0;
		for (size_t i = 0; i < MAX_CHANNELS; i++) {
			if (resampler[i]) delete resampler[i];
		}
	}

	void Reset(fr_i32 MaxBufferIn, fr_i32 InputSampleRate, fr_i32 OutputSampleRate, fr_i32 ChannelsCount, bool isLinear)
	{
		Destroy();
		Initialize(MaxBufferIn, InputSampleRate, OutputSampleRate, ChannelsCount, isLinear);
	}
	
	void Resample(fr_i32 frames, fr_f64** inputData, fr_f64** outputData)
	{
		if (frames > bufLength) Reset(frames, inSRate, outSRate, channels, lin);
		for (size_t i = 0; i < channels; i++) {
			resampler[i]->process(inputData[i], frames, outputData[i]);
		}
	}
};