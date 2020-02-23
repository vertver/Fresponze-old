#include <windows.h>
#include <stdio.h>
#include "Fresponze.h"
#include "FresponzeFileSystemWindows.h"
#include "FresponzeAdvancedMixer.h"
#include "FresponzeWavFile.h"

IFresponze* pFresponze = nullptr;

#define BUF_SIZE 4410

void test1()
{
	if (FrInitializeInstance((void**)&pFresponze) != 0) return;
	pFresponze->GetHardwareInterface(eEndpointWASAPIType, nullptr, nullptr);
}

int main()
{
	InitMemory();
	{
		int64_t reads = 0;
		DWORD dwp = 0;
		CFloatBuffer floatBufs[2];
		fr_f32* outFloat[2] = {};
		IFreponzeMapFile* mapFile = new CWindowsMapFile;
		CRIFFMediaResource res(mapFile);
		PcmFormat inFormat = {};
		PcmFormat outFormat = {};

		outFormat.Bits = 32;
		outFormat.Channels = 2;
		outFormat.Frames = BUF_SIZE;
		outFormat.IsFloat = true;
		outFormat.SampleRate = 44100;
		res.OpenResource((void*)"I:/Downloads/ehren-paper_lights-96.wav");
		res.SetFormat(outFormat);

		for (size_t i = 0; i < 2; i++) {
			floatBufs->Resize(BUF_SIZE);
			outFloat[i] = floatBufs->Data();
		}

		res.GetFormat(inFormat);
		HANDLE hFile = CreateFileW(L"I:\\out.raw", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
		if (hFile == INVALID_HANDLE_VALUE || !hFile) return false;
		CalculateFrames64(inFormat.Frames, inFormat.SampleRate, outFormat.SampleRate, reads);
		reads *= sizeof(fr_f32);

		while (reads > 0) {
			size_t writesize = (reads / sizeof(fr_f32)) < BUF_SIZE ? (reads / sizeof(fr_f32)) : BUF_SIZE;
			res.Read(writesize, outFloat);
			WriteFile(hFile, outFloat[0], (writesize * sizeof(fr_f32)), &dwp, nullptr);
			reads -= writesize * sizeof(fr_f32);
		}
	}
	DestroyMemory();
	return 0;
}
