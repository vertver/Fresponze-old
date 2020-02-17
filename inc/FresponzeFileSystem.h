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

enum EFSOpenFlags : fr_i32
{
	eNoFlag = 0x0,
	eReadFlag = 0x1,
	eWriteFlag = 0x2,
	eCreateAlwaysFlag = 0x4,
	eMustExistFlag = 0x8
};

enum EFSSeekMode : fr_i32
{
	eNoSeekMode,
	eFromBeginMode,
	eFromEndMode,
	eSetSeekMode
};

class IFreponzeFile : public IBaseInterface
{
protected:
	fr_i32 FileFlags;
	fr_i64 FilePosition = 0;
	fr_ptr pFileHandle = nullptr;

public:
	virtual bool Open(const fr_utf8* FileLink, fr_i32 Flags) = 0;
	virtual void Close() = 0;

	virtual void GetFileFormat(fr_string16& FileFormat) = 0;
	virtual void GetFileFormat(char* FileFormat) = 0;
	virtual fr_i64 GetSize() = 0;
	virtual fr_i64 Seek(fr_i64 SizeToSeek, fr_i32 SeekMode) = 0;

	virtual bool Read(fr_ptr pInBuffer, fr_i64 SizeToRead) = 0;
	virtual bool Write(fr_ptr pOutBuffer, fr_i64 SizeToWrite) = 0;
};

