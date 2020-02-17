/*********************************************************************
* Copyright (C) Anton Kovalev (vertver), 2020. All rights reserved.
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
#include "FresponzeTypes.h"

enum EFSOpenFlags : fr_i32
{
	eNoFlag = 0x0,
	eReadFlag = 0x1,
	eWriteFlag = 0x2,
	eCreateAlwaysFlag = 0x4,
	eMustExistFlag = 0x8
};

enum EFSMapping : fr_i32
{
	eMappingRead = 0x1,
	eMappintWrite = 0x2
};

class IFreponzeMapFile : public IBaseInterface
{
protected:
	fr_i32 FileFlags = 0;
	fr_i64 FilePosition = 0;
	fr_ptr pFileHandle = nullptr;
	fr_ptr pMapHandle = nullptr;

public:
	virtual bool Open(const fr_utf8* FileLink, fr_i32 Flags) = 0;
	virtual void Close() = 0;

	virtual fr_i64 GetSize() = 0;

	virtual bool MapFile(fr_ptr& OutPtr, fr_u64 OffsetFile, fr_i32 ProtectFlags) = 0;
	virtual bool MapPointer(fr_i32 SizeToMap, fr_ptr& OutPtr, fr_u64 OffsetFile, fr_i32 ProtectFlags) = 0;

	virtual bool UnmapFile(fr_ptr& OutPtr) = 0;
	virtual bool UnmapPointer(fr_i32 SizeToMap, fr_ptr& OutPtr) = 0;
};

