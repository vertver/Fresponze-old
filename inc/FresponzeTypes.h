/**************************************************************************
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
#ifdef WINDOWS_PLATFORM
#include <windows.h>
#else

#endif

#include <stdlib.h>
#include <math.h>

#define MAX_CHANNELS 64

inline
float
dbtol(float Decibels)
{
	return powf(10.0f, Decibels / 20.0f);
}

inline
float
ltodb(float Volume)
{
	if (Volume == 0) return -3.402823466e+38f; 
	return 20.0f * log10f(Volume);
}

inline
double
fastatan(double x)
{
	return (x / (1.0 + 0.28 * (x * x)));
}

typedef signed char fr_i8;
typedef unsigned char fr_u8;
typedef signed short fr_i16;
typedef unsigned short fr_u16;
typedef signed int fr_i32;
typedef unsigned int fr_u32;
typedef signed long long fr_i64;
typedef unsigned long long fr_u64;
typedef float fr_f32;
typedef double fr_f64;

typedef signed char fr_s8;
typedef signed short fr_s16;
typedef signed int fr_s32;
typedef signed long long fr_s64;

typedef fr_u64 uint64;
typedef fr_i8 int8;
typedef fr_i16 int16;
typedef fr_i64 int64;
typedef fr_f32 r32;
typedef fr_f64 r64;
typedef fr_i32 b32;
typedef fr_f64 real64;
typedef fr_i32 fr_err;

typedef char fr_utf8;
typedef fr_utf8* fr_PStr;
typedef fr_utf8 const* fr_PConstStr;
typedef fr_utf8 fr_string16[16];
typedef fr_utf8 fr_string32[32];
typedef fr_utf8 fr_string64[64];
typedef fr_utf8 fr_string128[128];
typedef fr_utf8 fr_string256[256];
typedef fr_utf8 fr_string512[512];
typedef fr_utf8 fr_string1k[1024];
typedef fr_utf8 fr_string2k[2048];
typedef fr_utf8 fr_string4k[4096];
typedef fr_utf8 fr_string8k[8192];

#ifndef NO_WIDECHAR
typedef wchar_t fr_utf16;
typedef fr_utf16* fr_PWStr;
typedef fr_utf16 const* fr_PWConstStr;
typedef fr_utf16 fr_wstring16[16];
typedef fr_utf16 fr_wstring32[32];
typedef fr_utf16 fr_wstring64[64];
typedef fr_utf16 fr_wstring128[128];
typedef fr_utf16 fr_wstring256[256];
typedef fr_utf16 fr_wstring512[512];
typedef fr_utf16 fr_wstring1k[1024];
typedef fr_utf16 fr_wstring2k[2048];
typedef fr_utf16 fr_wstring4k[4096];
typedef fr_utf16 fr_wstring8k[8192];
#endif

bool InitMemory();
void DestroyMemory();
void* FastMemAlloc(fr_i32 SizeToAllocate);
void* VirtMemAlloc(fr_i64 SizeToAllocate);
void FreeFastMemory(void* Ptr);
void FreeVirtMemory(void* Ptr, size_t Size);

typedef struct
{
	fr_i32 top;
	fr_i32 left;
	fr_i32 bottom;
	fr_i32 right;
} SRect;

typedef struct
{
	fr_f32 x, y;
} Vec2;

typedef struct
{
	fr_f32 x, y, z;
} Vec3;

typedef struct
{
	fr_f32 x, y, z, w;
} Vec4;

typedef struct
{
	Vec4 mat[2];
} VMatrix4x2;

typedef struct
{
	Vec4 mat[3];
} VMatrix4x3;

typedef struct
{
	Vec4 mat[4];
} VMatrix4x4;

typedef struct
{
	Vec3 mat[2];
} VMatrix3x2;

typedef struct
{
	Vec3 mat[3];
} VMatrix3x3;

typedef struct
{
	Vec3 mat[4];
} VMatrix3x4;

typedef struct
{
	Vec2 mat[2];
} VMatrix2x2;

typedef struct
{
	Vec2 mat[3];
} VMatrix2x3;

typedef struct
{
	Vec2 mat[4];
} VMatrix2x4;


typedef enum
{
	ChannelUndefined = 0x7fffffff,
	ChannelMono = 0,
	ChannelLeft,
	ChannelRight,
	ChannelCenter,
	ChannelLFE,
	ChannelLeftSurround,
	ChannelRightSurround,
	ChannelLeftCenter,
	ChannelRightCenter,
	ChannelSurround,
	ChannelCenterSurround = ChannelSurround,
	ChannelSideLeft,
	ChannelSideRight,
	ChannelTopMiddle,
	ChannelTopFL,
	ChannelTopFC,
	ChannelTFR,
	ChannelTRL,
	ChannelTRC,
	ChannelTRR,
	ChannelLFE2
} BaseSpeakerType;

typedef enum
{
	ArrangementChannelsUserDefined = -2,
	ArrangementChannelsEmpty = -1,
	ArrangementChannelsMono = 0,
	ArrangementChannelsStereo,
	ArrangementChannelsStereoSurround,
	ArrangementChannelsStereoCenter,
	ArrangementChannelsStereoSide,
	ArrangementChannelsStereoCLfe,
	ArrangementChannels30Cine,
	ArrangementChannels30Music,
	ArrangementChannels31Cine,
	ArrangementChannels31Music,
	ArrangementChannels40Cine,
	ArrangementChannels40Music,
	ArrangementChannels41Cine,
	ArrangementChannels41Music,
	ArrangementChannels50,
	ArrangementChannels51,
	ArrangementChannels60Cine,
	ArrangementChannels60Music,
	ArrangementChannels61Cine,
	ArrangementChannels61Music,
	ArrangementChannels70Cine,
	ArrangementChannels70Music,
	ArrangementChannels71Cine,
	ArrangementChannels71Music,
	ArrangementChannels80Cine,
	ArrangementChannels80Music,
	ArrangementChannels81Cine,
	ArrangementChannels81Music,
	ArrangementChannels102,
	ArrangementChannelsNULL
} BaseSpeakerArrangementType;

typedef struct
{
	int ChannelsCount;
	int ArrangmentType;
	int SpeakersTypes[MAX_CHANNELS];		// REAPER can support 64 channels
} BaseSpeakerArrangement;

typedef enum
{
	CategoryUnknown = 0,
	CategoryEffect,
	CategorySynth,
	CategoryAnalysis,
	CategoryMastering,
	CategorySpacializer,
	CategoryRoomFx,
	CategorySurroundFx,
	CategoryRestoration,
	CategoryOfflineProcess,
	CategoryShell,
	CategoryGenerator,
	CategoryMaxCount
} BasePluginCategory;

typedef struct
{
	fr_i32 NoteLength;
	fr_i32 NoteOffset;
	fr_i8 Data[4];
	fr_i8 DetuneLevel;
	fr_i8 NoteOffVelocity;
} MIDIEvent;

typedef struct
{
	fr_i32 EventsCount;
	MIDIEvent* pEventsArray;
}  MIDIEvents;

typedef struct
{
	fr_i16 Index;
	fr_i16 Bits;
	fr_i16 IsFloat;
	fr_i16 Channels;
	fr_i32 Frames;
	fr_i32 SampleRate;
} PcmFormat;

enum ETypeEndpoint : fr_i32
{
	NoneType,
	RenderType,
	CaptureType,
	ProxyType
};

struct EndpointInformation
{
	fr_i32 Type;
	fr_i32 EndpointId;
	PcmFormat EndpointFormat;
	fr_string256 EndpointName;
	fr_string256 EndpointUUID;
};

/*
	ex.
	4410 buffer length - 100ms
	44100 hz samplerate

	4410 / 44100 = 0.1 s
	48000 * 0.1s = 4800 frames
*/
inline
void
CalculateFrames(
	fr_i32	FramesCount,
	fr_i32	InputSampleRate,
	fr_i32	OutputSampleRate,
	fr_i32& OutputFramesCount
)
{
	fr_f32 fLatency = (fr_f32)FramesCount / (fr_f32)InputSampleRate;
	OutputFramesCount = fLatency * OutputSampleRate;
}

template
<typename TYPE>
class CBuffer
{
private:
	fr_i32 DataSize = 0;
	TYPE* pLocalData = nullptr;

public:
	CBuffer() {}
	CBuffer(CBuffer&& TempBuffer) = default;
	CBuffer(fr_i32 SizeToResize)
	{
		pLocalData = (TYPE*)FastMemAlloc(SizeToResize * sizeof(TYPE));
		DataSize = SizeToResize;
	}

	CBuffer(TYPE* pData, fr_i32 SizeToResize)
	{
		pLocalData = (TYPE*)FastMemAlloc(SizeToResize * sizeof(TYPE));
		memcpy(pLocalData, pData, SizeToResize * sizeof(TYPE));
		DataSize = SizeToResize;
	}

	void Free()
	{
		if (pLocalData) FreeFastMemory(pLocalData);
		DataSize = 0;
	}

	void Resize(fr_i32 SizeToResize)
	{
		if (SizeToResize > DataSize) {
			if (pLocalData) FreeFastMemory(pLocalData);
			pLocalData = (TYPE*)FastMemAlloc(SizeToResize * sizeof(TYPE));
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
		Free();
	}
};

typedef CBuffer<fr_f64> CDoubleBuffer;
typedef CBuffer<fr_f32> CFloatBuffer;
typedef CBuffer<fr_i32> CIntBuffer;
typedef CBuffer<fr_i16> CShortBuffer;
typedef CBuffer<fr_i8>  CByteBufffer;

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
			ppTempBuffers = FastMemAlloc(sizeof(void*) * CountOfBuffers);
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

typedef CRingBuffer<fr_f64> CRingDoubleBuffer;
typedef CRingBuffer<fr_f32> CRingFloatBuffer;
typedef CRingBuffer<fr_i32> CRingIntBuffer;
typedef CRingBuffer<fr_i16> CRingShortBuffer;
typedef CRingBuffer<fr_i8>  CRingByteBufffer;

enum SoundState : fr_i32
{
	NoneState,
	PlayingState,
	PausedState,
	StoppedState
};
class IBaseInterface
{
protected:
	long Counter = 0;

public:
	virtual bool Clone(void** ppOutInterface)
	{
		if (!ppOutInterface) return false;
		*ppOutInterface = this;
		AddRef();
		return true;
	}

	virtual long AddRef()
	{
		return _InterlockedIncrement(&Counter);
	}

	virtual void Release()
	{
		if (_InterlockedDecrement(&Counter) <= 0) {
			delete this;
		}
	}

	virtual ~IBaseInterface() = default;
};

class CBaseSound : public IBaseInterface
{
private:
	bool IsLooped = false;
	SoundState CurrentSoundState = NoneState;
	fr_i32 SamplePosition;
	PcmFormat DataFormat;
	CFloatBuffer Buffer;

public:
	CBaseSound() { _InterlockedIncrement(&Counter); }
	CBaseSound(CBaseSound&& parent) = default;
	CBaseSound(fr_f32* pData, fr_i32 SizeToResize, PcmFormat Fmt) : Buffer(pData, SizeToResize), DataFormat(Fmt) { _InterlockedIncrement(&Counter); }

	void SetLooped(bool bLooped) { IsLooped = bLooped; }
	void Load(fr_f32* pData, fr_i32 Frames, PcmFormat& Fmt)
	{
		DataFormat = Fmt;
		Buffer.Push(pData, Frames * DataFormat.Channels);
	}

	bool GetData(fr_f32*& pOutData, fr_i32 Frames, fr_i32& OutFrames)
	{
		fr_i32 BufSize = Buffer.Size();
		fr_i32 BufDelta = BufSize - SamplePosition;

		if (!BufDelta) {
			if (IsLooped) Reset();
			else return false;
		}
		if (!BufSize) return false;

		OutFrames = Frames;
		if (BufDelta < Frames) OutFrames = BufDelta;
		pOutData = (fr_f32*)(((fr_u8*)(Buffer.Data())) + OutFrames);
		SamplePosition += OutFrames;

		return true;
	}

	void Reset() { SamplePosition = 0; }
	void SetState(SoundState ThisState) { CurrentSoundState = ThisState; }
	SoundState GetState() { return CurrentSoundState; }
	PcmFormat& Format() { return DataFormat; }

	void Free()
	{
		memset(&DataFormat, 0, sizeof(PcmFormat));
		Buffer.Free();
	}
};

class IBaseEvent
{
public:
	virtual void Raise() = 0;
	virtual void Reset() = 0;
	virtual void Wait() = 0;
	virtual bool Wait(fr_i32 TimeToWait) = 0;
	virtual bool IsRaised() = 0;
};

class IAudioCallback : public IBaseInterface
{
public:
	virtual fr_err EndpointCallback(fr_f32* pData, fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate, fr_i32 CurrentEndpointType) = 0;
	virtual fr_err RenderCallback(fr_i32 Frames, fr_i32 Channels, fr_i32 SampleRate) = 0;
};

inline 
bool
ConvertComplexToArray(
	fr_f32* pComplex,
	fr_f32** ppArray,
	fr_i32 Channels,
	fr_i32 FramesToConvert
)
{
	if (!pComplex || !ppArray || !*ppArray) return false;

	for (size_t i = 0; i < FramesToConvert / Channels; i++) {
		for (size_t o = 0; o < Channels; o++) {
			ppArray[o][i] = pComplex[i * Channels + o];
		}
	}
}

inline
bool
MixComplexToArray(
	fr_f32* pComplex,
	fr_f32** ppArray,
	fr_i32 Channels,
	fr_i32 FramesToConvert
)
{
	if (!pComplex || !ppArray || !*ppArray) return false;

	for (size_t i = 0; i < FramesToConvert / Channels; i++) {
		for (size_t o = 0; o < Channels; o++) {
			ppArray[o][i] += pComplex[i * Channels + o];
		}
	}
}

inline char* utf16_to_utf8(const wchar_t* _src) {
	char* dst;
	size_t  len;
	size_t  si;
	size_t  di;
	len = wcslen(_src);
	dst = (char*)malloc(sizeof(*dst) * (3 * len + 1));
	if (dst == NULL)return dst;
	for (di = si = 0; si < len; si++) {
		unsigned c0;
		c0 = _src[si];
		if (c0 < 0x80) {
			/*Can be represented by a 1-byte sequence.*/
			dst[di++] = (char)c0;
			continue;
		}
		else if (c0 < 0x800) {
			/*Can be represented by a 2-byte sequence.*/
			dst[di++] = (char)(0xC0 | c0 >> 6);
			dst[di++] = (char)(0x80 | c0 & 0x3F);
			continue;
		}
		else if (c0 >= 0xD800 && c0 < 0xDC00) {
			unsigned c1;
			/*This is safe, because c0 was not 0 and _src is NUL-terminated.*/
			c1 = _src[si + 1];
			if (c1 >= 0xDC00 && c1 < 0xE000) {
				unsigned w;
				/*Surrogate pair.*/
				w = ((c0 & 0x3FF) << 10 | c1 & 0x3FF) + 0x10000;
				/*Can be represented by a 4-byte sequence.*/
				dst[di++] = (char)(0xF0 | w >> 18);
				dst[di++] = (char)(0x80 | w >> 12 & 0x3F);
				dst[di++] = (char)(0x80 | w >> 6 & 0x3F);
				dst[di++] = (char)(0x80 | w & 0x3F);
				si++;
				continue;
			}
		}
		/*Anything else is either a valid 3-byte sequence, an invalid surrogate
		   pair, or 'not a character'.
		  In the latter two cases, we just encode the value as a 3-byte
		   sequence anyway (producing technically invalid UTF-8).
		  Later error handling will detect the problem, with a better
		   chance of giving a useful error message.*/
		dst[di++] = (char)(0xE0 | c0 >> 12);
		dst[di++] = (char)(0x80 | c0 >> 6 & 0x3F);
		dst[di++] = (char)(0x80 | c0 & 0x3F);
	}
	dst[di++] = '\0';
	return dst;
}

#define _RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr;} }
#define ELEMENTSCOUNT(x) sizeof(x) / sizeof(sizeof(x[0]))

#ifdef WINDOWS_PLATFORM
#define IsInvalidHandle(x) (x == 0 || x == INVALID_HANDLE_VALUE)

inline 
void
PcmFormatToWaveFormatEx(
	PcmFormat& pcmFmt, 
	WAVEFORMATEX& waveEx
)
{
	waveEx.cbSize = sizeof(WAVEFORMATEX);
	waveEx.nChannels = pcmFmt.Channels;
	waveEx.nSamplesPerSec = pcmFmt.SampleRate;
	waveEx.wBitsPerSample = pcmFmt.Bits;
	waveEx.wFormatTag = pcmFmt.IsFloat ? 3 : 1;
	waveEx.nBlockAlign = waveEx.wBitsPerSample * waveEx.nChannels;
	waveEx.nAvgBytesPerSec = waveEx.nBlockAlign * waveEx.nSamplesPerSec;
}

inline
void
WaveFormatExToPcmFormat(
	WAVEFORMATEX& waveEx,
	PcmFormat& pcmFmt
)
{
	pcmFmt.Bits = waveEx.wBitsPerSample;
	pcmFmt.Channels = waveEx.nChannels;
	pcmFmt.IsFloat = waveEx.wFormatTag == 3 ? true : false;
	pcmFmt.SampleRate = waveEx.nSamplesPerSec;
}

class CWinEvent : public IBaseEvent
{
private:
	HANDLE hEvent = nullptr;

public:
	CWinEvent();
	~CWinEvent();

	void Raise() override;
	void Reset() override;
	void Wait() override;
	bool Wait(fr_i32 TimeToWait) override;
	bool IsRaised() override;
};
#else
class CPosixEvent : public IBaseEvent
{

};
#endif

inline 
void
DebugAssert(
	bool value,
	const char* Text
)
{
	if (!value) {
		if (!Text) Text = "Assert for debug things";
#ifdef WINDOWS_PLATFORM
		OutputDebugStringA(Text);
#ifdef SHOW_MESSAGEBOXES
		MessageBoxA(nullptr, Text, "Fresponze debug error", MB_OK | MB_ICONHAND);
#endif
		DebugBreak();
#else
		printf(Text);
#endif

	}
}

#define BugAssert(xx, yy) DebugAssert(!!xx, yy)
#define BugAssert1(xx) DebugAssert(!!xx, nullptr)
