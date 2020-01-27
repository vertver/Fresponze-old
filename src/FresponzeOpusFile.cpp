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
#include "FresponzeOpusFile.h"

bool COpusMediaResource::OpenResource(void* pResourceLinker)
{
	int ret = 0;
	of = op_open_file((fr_utf8*)pResourceLinker, &ret);
	return false;
}

bool COpusMediaResource::CloseResource()
{
	return false;
}

void COpusMediaResource::GetFormat(PcmFormat& format)
{

}

void COpusMediaResource::SetFormat(PcmFormat outputFormat)
{

}

bool COpusMediaResource::Read(fr_f32** ppFloatData)
{
	return false;
}

bool COpusMediaResource::ReadRaw(fr_f32** ppFloatData)
{
	return false;
}
