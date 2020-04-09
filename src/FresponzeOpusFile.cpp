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
	AddRef();
	OpenResource(pResourceLinker);
}

COpusMediaResource::~COpusMediaResource()
{
	CloseResource();
}

void
COpusMediaResource::ClearBuffers()
{
	if (floatBuffers) {
		delete floatBuffers;
		floatBuffers = nullptr;
	}

	for (size_t i = 0; i < 2; i++) {
		if (doubleBuffers[i]) {
			delete doubleBuffers[i];
			doubleBuffers[i] = nullptr;
		}
	}
}

void
COpusMediaResource::AllocateBuffers(fr_i32 ChannelsCount)
{
	ClearBuffers();
	floatBuffers = new C2DFloatBuffer;
	for (size_t i = 0; i < 2; i++) {
		doubleBuffers[i] = new C2DDoubleBuffer;
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
	fr_f32* bufferFrames = (float*)FastMemAlloc(128 * 48 * sizeof(float) * sizeof(float));

	of = op_open_file((fr_utf8*)pResourceLinker, &ret);
	if (!of) {
		FreeFastMemory(bufferFrames);
		return false;
	}
	pcm_offset = (op_pcm_tell(of) - 48000);
	ret = op_read_float_stereo(of, bufferFrames, (128 * 48 * sizeof(float)));
	BugAssert((ret == OP_HOLE), "Corrupted OPUS file segment");
	if (ret <= 0) {
		FreeFastMemory(bufferFrames);
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
		formatOfFile.Frames = (fr_i32)op_pcm_total(of, li);
		tags = op_tags(of, li);
		BugAssert((ret = op_raw_seek(of, 0)), "Can't seek OPUS file");
		if (!!ret) {
			FreeFastMemory(bufferFrames);
			return false;
		}
	}

	if (tags) {
		if (tags->vendor) opusVendor = _strdup(tags->vendor);
		if (tags->comments) {
			commentsCount = tags->comments;
			opusComments = (const char**)FastMemAlloc(sizeof(void*) * commentsCount);
			if (tags->user_comments && opusComments){
				for (size_t i = 0; i < commentsCount; i++) {
					if (tags->user_comments[i]) opusComments[i] = _strdup(tags->user_comments[i]);
				}
			}
		}
	}

	resampler.Initialize(OPUS_BUFFER, 48000, OutputFormat.SampleRate, formatOfFile.Channels, false);
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

fr_i64
COpusMediaResource::CompareFileSize(fr_i32 InputFrames)
{
	bool bCompare = InputFrames + FileBufferPosition > formatOfFile.Frames;
	return bCompare ? formatOfFile.Frames - FileBufferPosition : InputFrames;
}

fr_i32
COpusMediaResource::CompareSize(fr_i32 InputFrames)
{
	bool bCompare = InputFrames + BufferPosition > OPUS_BUFFER;
	return bCompare ? OPUS_BUFFER - BufferPosition : InputFrames;
}

void
COpusMediaResource::AddToBuffer(fr_f32* InputBuffer, fr_i32 InputBufferSize, fr_i32 ChannelsCount)
{
	if (floatBuffers->GetBuffersCount() < ChannelsCount) {
		floatBuffers->Resize(ChannelsCount, OPUS_BUFFER);
		for (size_t i = 0; i < 2; i++) doubleBuffers[i]->Resize(ChannelsCount, OPUS_BUFFER);
	}
	floatBuffers->PushPacked(InputBuffer, InputBufferSize, FileReadSize);
}

bool
COpusMediaResource::NextBlock()
{
	fr_i32 ret = 0;
	fr_i32 li = 0;
	const OpusHead* head = nullptr;
	const OpusTags* tags = nullptr;

	tempBuffer.Resize(OPUS_BUFFER);
	FileReadSize = 0;
	while (FileReadSize + ret < OPUS_BUFFER) {
		/* The file can be corrupted, so we must to check it before read data */
		ret = op_read_float(of, tempBuffer.Data(), tempBuffer.Size(), &li);
		if (ret == OP_HOLE) continue;
		else if (ret < 0) return false;

		/*
			If our new block has new channel count - we must to verify with new format.
			(lower than current channels count? try to convert to backend audio format via mid :
			bigger than current channels count? try to convert to backend audio format via mid/side)
		*/
		if (li != previous_li) {
			head = op_head(of, li);
			formatOfFile.Channels = head->channel_count;
			previous_li = li;
		}

		AddToBuffer(tempBuffer.Data(), ret, formatOfFile.Channels);
		FileReadSize += ret;
	}

	fr_i32 channels = formatOfFile.Channels;
	floatBuffers->PushPacked(tempBuffer.Data(), FileReadSize, channels);

	DoubleToFloat(floatBuffers->GetBuffers(), doubleBuffers[0]->GetBuffers(), channels, FileReadSize);
	resampler.Resample(FileReadSize, doubleBuffers[0]->GetBuffers(), doubleBuffers[1]->GetBuffers());
	FloatToDouble(floatBuffers->GetBuffers(), doubleBuffers[1]->GetBuffers(), channels, FileReadSize);
 
 	return true;
}

fr_i64
COpusMediaResource::Read(fr_i64 FramesCount, fr_f32** ppFloatData)
{
	fr_i32 CopySize = 0;
	fr_i32 TempChannels = OutputFormat.Channels;

	NextBlock();
// 
// 	if (!floatBuffers) return false;
// 	floatBuffers->Resize(TempChannels, OPUS_BUFFER / TempChannels);
// 	for (size_t i = 0; i < 2; i++) doubleBuffers[i]->Resize(TempChannels, OPUS_BUFFER / TempChannels);
// 
// 	if (!LastBlockSize) {
// 		if (!NextBlock()) return false;
// 		BufferPosition = 0;
// 	}
// 
// 	CopySize = CompareSize(FramesCount);
// 	if (CopySize) {
// 		for (size_t i = 0; i < TempChannels; i++) {
// 			memcpy(ppFloatData[i], floatBuffers->GetBufferData(i), LastBlockSize * sizeof(fr_f32));
// 		}
// 	}
// 
// 	if (BufferPosition - CopySize <= 0) {
// 		if (!NextBlock()) return false;
// 		BufferPosition = 0;
// 	} else {
// 		BufferPosition += CopySize;
// 	}

	return true;
}

fr_i64
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
