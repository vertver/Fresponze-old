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

COpusMediaResource::COpusMediaResource(void* pResourceLinker)
{
	OpenResource(pResourceLinker);
}

COpusMediaResource::~COpusMediaResource()
{
	CloseResource();
}

bool 
COpusMediaResource::OpenResource(void* pResourceLinker)
{
	const OpusHead* head;
	const OpusTags* tags;
	if (isOpened) return true;
	float* bufferFrames = (float*)malloc(128 * 48 * sizeof(float) * sizeof(float));
	int ret = 0;
	int li = 0;

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

	for (size_t i = 0; i < formatOfFile.Channels; i++) {
		floatBuffers[i] = new CFloatBuffer;
		doubleBuffers[i] = new CDoubleBuffer;
	}

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
			opusComments = (char**)malloc(sizeof(void*) * commentsCount);
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
	
	for (size_t i = 0; i < MAX_CHANNELS; i++) {
		if (floatBuffers[i]) delete floatBuffers[i];
		if (doubleBuffers[i]) delete doubleBuffers[i];
	}

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
	OutputFormat = outputFormat;
}

bool
COpusMediaResource::NextBlock()
{

}

bool 
COpusMediaResource::Read(fr_f32** ppFloatData)
{
	return false;
}

bool
COpusMediaResource::ReadRaw(fr_f32** ppFloatData)
{
	return false;
}

fr_i64 
COpusMediaResource::SetPosition(fr_i64 FramePosition)
{
	int ret = 0;

	if (!op_seekable(of)) return -1;
	BugAssert((ret = op_raw_seek(of, 0)), "Can't seek OPUS file");
	return ret;
}
