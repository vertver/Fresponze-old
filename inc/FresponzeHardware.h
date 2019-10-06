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
#include "FresponzeEndpoint.h"
#include "FresponzeEnumerator.h"

template
<typename TYPE>
class CBuffer
{
private:
	fr_i32 DataSize = 0;
	TYPE* pLocalData = nullptr;

public:
	CBuffer() {}
	CBuffer(fr_i32 SizeToResize)
	{
		pLocalData = FastMemAlloc(SizeToResize * sizeof(TYPE));
		DataSize = SizeToResize;
	}

	void Resize(fr_i32 SizeToResize)
	{
		if (SizeToResize > DataSize) {
			if (pLocalData) FreeFastMemory(pLocalData);
			pLocalData = FastMemAlloc(SizeToResize * sizeof(TYPE));
			DataSize = SizeToResize;
		}
	}

	void Push(TYPE* pData, fr_i32 SizeToResize)
	{
		if (SizeToResize > DataSize) Resize(SizeToResize);
		memcpy(pLocalData, pData, SizeToResize * sizeof(TYPE));
	}

	fr_i32 Size()
	{
		return DataSize;
	}

	TYPE* Data()
	{
		return pLocalData;
	}

	~CBuffer()
	{
		if (pLocalData) FreeFastMemory(pLocalData);
	}
};

template
<typename TYPE>
class CRingBuffer
{
private:
	fr_i32 CurrentBuffer = 0;
	fr_i32 BuffersCount = 0;
	fr_i32 BuffersSize = 0;
	CBuffer<TYPE>** ppBuffers = nullptr;

public:
	CRingBuffer() {}
	CRingBuffer(fr_i32 CountOfBuffers, fr_i32 SizeOfBuffers)
	{
		SetBuffersCount(CountOfBuffers);
		Resize(SizeOfBuffers);
	}

	void SetBuffersCount(fr_i32 CountOfBuffers)
	{
		CBuffer<TYPE>** ppTempBuffers = nullptr;
		if (BuffersCount < CountOfBuffers) {
			ppTempBuffers = FastMemAlloc(sizeof(CBuffer*) * CountOfBuffers);
			if (ppBuffers) {
				for (size_t i = 0; i < BuffersCount; i++) {
					if (ppBuffers[i]) ppTempBuffers[i] = ppBuffers[i];
				}
			}

			ppBuffers = ppTempBuffers;
			BuffersCount = CountOfBuffers;
		}
	}

	void Resize(fr_i32 SizeToResize)
	{
		if (ppBuffers) {
			for (size_t i = 0; i < BuffersCount; i++) {
				if (!ppBuffers[i]) ppBuffers[i] = new CBuffer<TYPE>(SizeToResize);
				else ppBuffers[i]->Resize(SizeToResize);
			}
		}
	}

	void PushBuffer(TYPE* InData, fr_i32 SizeOfData)
	{
		ppBuffers[CurrentBuffer]->Push(InData, SizeOfData);
	}

	void PushToNextBuffer(TYPE* InData, fr_i32 SizeOfData)
	{
		fr_i32 BufIndex = CurrentBuffer;
		if (++BufIndex >= BuffersCount) {
			BufIndex = 0;
		}

		ppBuffers[BufIndex]->Push(InData, SizeOfData);
	}

	void NextBuffer()
	{
		if (++CurrentBuffer >= BuffersCount) {
			CurrentBuffer = 0;
		}
	}

	TYPE* GetData()
	{
		return ppBuffers[CurrentBuffer]->Data();
	}

	fr_i32 BufferSize() 
	{
		return BuffersSize;
	}

	~CRingBuffer()
	{
		if (ppBuffers) {
			for (size_t i = 0; i < BuffersCount; i++) {
				if (ppBuffers[i]) delete ppBuffers[i];
			}

			FreeFastMemory(ppBuffers);
		}
	}
};

typedef CBuffer<fr_f32> CFloatBuffer;
typedef CBuffer<fr_i32> CIntBuffer;
typedef CBuffer<fr_i16> CShortBuffer;
typedef CBuffer<fr_i8>  CByteBufffer;

typedef CRingBuffer<fr_f32> CRingFloatBuffer;
typedef CRingBuffer<fr_i32> CRingIntBuffer;
typedef CRingBuffer<fr_i16> CRingShortBuffer;
typedef CRingBuffer<fr_i8>  CRingByteBufffer;

class IAudioNotificationCallback : public IBaseInterface
{
protected:
	bool bDefaultInputDevice = false;
	bool bDefaultOutputDevice = false;

public:
	virtual bool SetCurrentDevice(fr_i32 DeviceType, bool IsDefaultDevice, void* pDevicePointer) = 0;
	virtual bool DeviceDisabled(void* pDeviceDisabled) = 0;
	virtual bool DefaultDeviceChanged(fr_i32 DeviceType) = 0;
	virtual bool ListOfDevicesChanded() = 0;
};

class IAudioHardware : public IBaseInterface
{
protected:
	IAudioEnumerator* pAudioEnumerator = nullptr;
	IAudioEndpoint* pInputEndpoint = nullptr;
	IAudioEndpoint* pOutputEndpoint = nullptr;
	IAudioCallback* pAudioCallback = nullptr;
	IAudioNotificationCallback* pNotificationCallback = nullptr;
	CRingFloatBuffer InputBuffer;
	CRingFloatBuffer OutputBuffer;

public:
	virtual bool Enumerate() = 0;

	virtual bool Open(fr_i32 DeviceType, fr_f32 DelayTime) = 0;
	virtual bool Open(fr_i32 DeviceType, fr_f32 DelayTime, char* pUUID) = 0;
	virtual bool Open(fr_i32 DeviceType, fr_f32 DelayTime, fr_i32 DeviceId) = 0;

	virtual bool Restart(fr_i32 DeviceType, fr_f32 DelayTime) = 0;
	virtual bool Restart(fr_i32 DeviceType, fr_f32 DelayTime, char* pUUID) = 0;
	virtual bool Restart(fr_i32 DeviceType, fr_f32 DelayTime, fr_i32 DeviceId) = 0;

	virtual bool PushData(fr_f32* pData, fr_i32 Frames) = 0;

	virtual bool Start() = 0;
	virtual bool Stop() = 0;

	virtual bool Close() = 0;
};
