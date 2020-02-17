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
#include "FresponzeWavFile.h"

CRIFFMediaResource::CRIFFMediaResource(IFreponzeMapFile* pNewMapper)
{ 
	pNewMapper->Clone((void**)&pMapper);
}

CRIFFMediaResource::~CRIFFMediaResource()
{
	CloseResource();
}

bool
CRIFFMediaResource::OpenResource(void* pResourceLinker)
{
	fr_u32 cmpdword = 0x46464952;
	wav_header* wavHeader = nullptr;
	if (!pMapper->Open((const fr_utf8*)pResourceLinker, eReadFlag | eMustExistFlag)) return false;
	if (!pMapper->MapPointer(sizeof(wav_header), (fr_ptr&)wavHeader, 0, eMappingRead)) {
		pMapper->Close();
		return false;
	}

	BugAssert(!memcmp(wavHeader->riff_header, &cmpdword, sizeof(fr_u32)), "Wrong file format");
	riff_to_pcm(wavHeader, &fileFormat);
	if (!pMapper->UnmapPointer(sizeof(wav_header), (fr_ptr&)wavHeader)) {
		pMapper->Close();
		return false;
	}

	if (!pMapper->MapFile(pMappedArea, 0, eMappingRead)) {
		pMapper->Close();
		return false;
	}

	return true;
}

bool
CRIFFMediaResource::CloseResource()
{
	_RELEASE(pMapper);
}

void 
CRIFFMediaResource::GetVendorName(const char*& vendorName)
{

}

void 
CRIFFMediaResource::GetVendorString(const char*& vendorString)
{

}

void 
CRIFFMediaResource::GetFormat(PcmFormat& format)
{

}

void 
CRIFFMediaResource::SetFormat(PcmFormat outputFormat)
{

}

bool 
CRIFFMediaResource::Read(fr_i64 FramesCount, fr_f32** ppFloatData)
{

}

bool 
CRIFFMediaResource::ReadRaw(fr_i64 FramesCount, fr_f32** ppFloatData)
{

}

fr_i64 
CRIFFMediaResource::SetPosition(fr_i64 FramePosition)
{

}
