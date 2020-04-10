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
#include "FresponzeMediaResource.h"
#include "opusfile.h"

class COpusMediaResource : public IMediaResource
{
private:
	bool isOpened = false;
	fr_i32 file_li = 0;
	fr_i32 previous_li = 0;
	fr_i32 commentsCount = 0;
	fr_i32 BufferPosition = 0;
	fr_i32 FileBufferPosition = 0;
	fr_i32 LastBlockSize = 0;
	fr_i32 CurrentBuffer = 0;
	fr_i32 FileReadSize = 0;
	fr_i64 pcm_offset = 0;
	OggOpusFile* of = nullptr;
	const char* opusVendor = nullptr;
	const char** opusComments = nullptr;
	PcmFormat formatOfFile = {};
	PcmFormat OutputFormat = {};
	OpusFileCallbacks cb = { nullptr, nullptr, nullptr, nullptr };
	CFloatBuffer tempBuffer = {};
	C2DFloatBuffer* floatBuffers = {};
	C2DDoubleBuffer* doubleBuffers[2] = {};
	CR8BrainResampler resampler;

	bool NextBlock();
	void ClearBuffers();
	void AllocateBuffers(fr_i32 ChannelsCount);

	void AddToBuffer(fr_f32* InputBuffer, fr_i32 InputBufferSize, fr_i32 ChannelsCount);

	fr_i64 CompareFileSize(fr_i32 InputFrames);
	fr_i32 CompareSize(fr_i32 InputFrames);

public:
	COpusMediaResource() {}
	COpusMediaResource(void* pResourceLinker);
	~COpusMediaResource();

	bool OpenResource(void* pResourceLinker) override;
	bool CloseResource() override;

	void GetVendorName(const char*& vendorName) override;		// vendor name, in tags structure
	void GetVendorString(const char*& vendorString) override;	// vendor comment, in tags structure
	void GetFormat(PcmFormat& format) override;
	void SetFormat(PcmFormat outputFormat) override;

	fr_i64 Read(fr_i64 FramesCount, fr_f32** ppFloatData) override;
	fr_i64 ReadRaw(fr_i64 FramesCount, fr_f32** ppFloatData) override;

	fr_i64 SetPosition(fr_i64 FramePosition) override;
	fr_i64 GetPosition() override;
};
