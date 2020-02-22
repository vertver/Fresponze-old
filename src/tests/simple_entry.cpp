#include <windows.h>
#include <stdio.h>
#include "Fresponze.h"
#include "FresponzeOpusFile.h"
#include "FresponzeAdvancedMixer.h"
#include "FresponzeWavFile.h"
#include "FresponzeFileSystemWindows.h"

IFresponze* pFresponze = nullptr;

void test1()
{
	if (FrInitializeInstance((void**)&pFresponze) != 0) return;
	pFresponze->GetHardwareInterface(eEndpointWASAPIType, nullptr, nullptr);
}

int main()
{
	size_t reads = 0;
	DWORD dwp = 0;
	CFloatBuffer floatBufs[2];
	fr_f32* outFloat[2] = {};
	IFreponzeMapFile* mapFile = new CWindowsMapFile;
	CRIFFMediaResource res(mapFile);
	PcmFormat inFormat = {};
	PcmFormat outFormat = {};

	InitMemory();
	
	outFormat.Bits = 32;
	outFormat.Channels = 2;
	outFormat.Frames = 0;
	outFormat.IsFloat = true;
	outFormat.SampleRate = 44100;
	res.SetFormat(outFormat);
	res.OpenResource((void*)"I:/Downloads/ehren-paper_lights-96.wav");

	for (size_t i = 0; i < 2; i++){
		floatBufs->Resize(44100);
		outFloat[i] = floatBufs->Data();
	}

	res.GetFormat(inFormat);
	HANDLE hFile = CreateFileW(L"I:\\out.raw", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE || !hFile) return false;
	reads = inFormat.Frames * sizeof(fr_i32) * 2;

	while (reads) {
		size_t writesize = (reads / sizeof(fr_i32) * 2 ) < 44100 ? (reads / sizeof(fr_i32) * 2) : 44100;
		res.Read(writesize, outFloat);
		WriteFile(hFile, outFloat[0], (writesize * sizeof(fr_i32) * 2), &dwp, nullptr);
		reads -= writesize * sizeof(fr_i32) * 2;
	}
	
	DestroyMemory();
	return 0;
}
