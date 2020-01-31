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
#include "FresponzeOpusFile.h"

#define OPUS_BUFFER 11520		// 120 ms on 48000Hz

COpusMediaResource::COpusMediaResource(void* pResourceLinker)
{
	OpenResource(pResourceLinker);
}

COpusMediaResource::~COpusMediaResource()
{
	CloseResource();
}

void
COpusMediaResource::ClearBuffers()
{
	for (size_t i = 0; i < MAX_CHANNELS; i++) {
		if (floatBuffers[i]) {
			delete floatBuffers[i];
			floatBuffers[i] = nullptr;
		}		
		if (doubleBuffers[i]) {
			delete doubleBuffers[i];
			doubleBuffers[i] = nullptr;
		}
	}
}

void
COpusMediaResource::AllocateBuffers(fr_i32 ChannelsCount)
{
	for (size_t i = 0; i < ChannelsCount; i++) {
		/* Free old buffer to allocate new */
		if (floatBuffers[i]) {
			delete floatBuffers[i];
			floatBuffers[i] = nullptr;
		}
		if (doubleBuffers[i]) {
			delete doubleBuffers[i];
			doubleBuffers[i] = nullptr;
		}

		floatBuffers[i] = new CFloatBuffer;
		doubleBuffers[i] = new CDoubleBuffer;
	}
}

bool 
COpusMediaResource::OpenResource(void* pResourceLinker)
{
	if (isOpened) return true;
	fr_i32 ret = 0;
	fr_i32 li = 0;
	const OpusHead* head = nullptr;
	const OpusTags* tags = nullptr;
	fr_f32* bufferFrames = (float*)malloc(128 * 48 * sizeof(float) * sizeof(float));

	of = op_open_file((fr_utf8*)pResourceLinker, &ret);
	if (!of) {
		free(bufferFrames);
		return false;
	}
	pcm_offset = (op_pcm_tell(of) - 48000);
	
	BugAssert(((ret = op_read_float_stereo(of, bufferFrames, (128 * 48 * sizeof(float)))) == OP_HOLE), "Corrupted OPUS file segment");
	if (!!ret) {
		free(bufferFrames);
		return false;
	}

	li = op_current_link(of);
	head = op_head(of, li);

	formatOfFile.Bits = 0;
	formatOfFile.Index = 0;
	formatOfFile.Channels = head->channel_count;
	formatOfFile.IsFloat = false;
	formatOfFile.SampleRate = 48000;		// use full quality Opus

	if (op_seekable(of)) {
		formatOfFile.Frames = op_pcm_total(of, li);
		tags = op_tags(of, li);
		BugAssert((ret = op_raw_seek(of, 0)), "Can't seek OPUS file");
		if (!!ret) {
			free(bufferFrames);
			return false;
		}
	}

	if (tags) {
		if (tags->vendor) opusVendor = _strdup(tags->vendor);
		if (tags->comments) {
			commentsCount = tags->comments;
			opusComments = (const char**)malloc(sizeof(void*) * commentsCount);
			for (size_t i = 0; i < commentsCount; i++) {
				if (tags->user_comments[i]) opusComments[i] = _strdup(tags->user_comments[i]);
			}
		}
	}

	return true;
}

bool 
COpusMediaResource::CloseResource()
{
	if (of) op_free(of);
	ClearBuffers();
	return true;;
}

void 
COpusMediaResource::GetVendorName(const char*& vendorName)
{
	if (opusVendor) vendorName = _strdup(vendorName);
}

void 
COpusMediaResource::GetVendorString(const char*& vendorString)
{
	if (opusComments) if (opusComments[0]) vendorString = _strdup(opusComments[0]);
}

void 
COpusMediaResource::GetFormat(PcmFormat& format)
{
	format = formatOfFile;
}

void 
COpusMediaResource::SetFormat(PcmFormat outputFormat)
{
	if (outputFormat.Channels != OutputFormat.Channels) AllocateBuffers(outputFormat.Channels);
	OutputFormat = outputFormat;
}

bool
COpusMediaResource::NextBlock()
{
	fr_i32 ret = 0;
	fr_i32 li = 0;
	const OpusHead* head = nullptr;
	const OpusTags* tags = nullptr;
	if (!tempBuffer.Size()) tempBuffer.Resize(OPUS_BUFFER);

tryCicle:
	/* The file can be corrupted, so we must to check it before read data */
	ret = op_read_float_stereo(of, tempBuffer.Data(), tempBuffer.Size());
	if (ret == OP_HOLE) {
		goto tryCicle;
	} else if (ret < 0) return false;

	/*
		If our new block has new channel count - we must to verify with new format.
		(lower than current channels count? try to convert to backend audio format via mid : 
		bigger than current channels count ? try to convert to backend audio format via mid/side)
	*/
	li = op_current_link(of);
	if (li != previous_li) {
		head = op_head(of, li);
		formatOfFile.Channels = head->channel_count;
		previous_li = li;
	}



	return true;
}

bool 
COpusMediaResource::Read(fr_i64 FramesCount, fr_f32** ppFloatData)
{
	if (!floatBuffers[0]) return false;
	if (BufferPosition < floatBuffers[0]->Size() - FramesCount) {

	} else {
		
	}

}

bool
COpusMediaResource::ReadRaw(fr_i64 FramesCount, fr_f32** ppFloatData)
{
	return false;
}

fr_i64 
COpusMediaResource::SetPosition(fr_i64 FramePosition)
{
	fr_i32 ret = 0;
	if (!op_seekable(of)) return -1;
	BugAssert((ret = op_pcm_seek(of, FramePosition)), "Can't seek OPUS file");
	return ret;
}
