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
#include <windows.h>
#include <stdio.h>
#include "Fresponze.h"
#include "FresponzeFileSystemWindows.h"
#include "FresponzeWavFile.h"
#include "FresponzeListener.h"
#include "FresponzeMixer.h"
#include "FresponzeMasterEmitter.h"

IFresponze* pFresponze = nullptr;

#define BUF_SIZE 4410
#include <process.h>
#include <avrt.h>

void loop()
{
	while (true) {
		Sleep(5);
	}
}

void test1()
{
	ListenersNode* listNode = nullptr;
	PcmFormat format = {};
	EndpointInformation* InputList = nullptr;
	EndpointInformation* OutputList = nullptr;
	EndpointInformation OutputLists = {};
	IBaseEmitter* pBaseEmitter = nullptr;
	IBaseEmitter* pBaseEmitterSecond = nullptr;
	IAudioHardware* pAudioHardware = nullptr;
	IAdvancedMixer* pAdvancedMixer = nullptr;
	IAudioCallback* pAudioCallback = nullptr;

	if (FrInitializeInstance((void**)&pFresponze) != 0) return;
	pFresponze->GetMixerInterface(eMixerAdvancedType, (void**)&pAdvancedMixer);
	pAudioCallback = new CMixerAudioCallback(pAdvancedMixer);
	//pBaseEmitter = new CSteamAudioEmitter;
	//pBaseEmitterSecond = new CSteamAudioEmitter;
	pAdvancedMixer->CreateEmitter(pBaseEmitter);
	//pAdvancedMixer->CreateEmitter(pBaseEmitterSecond);

	pFresponze->GetHardwareInterface(eEndpointWASAPIType, pAudioCallback, (void**)&pAudioHardware);
	pAudioHardware->GetDevicesList(InputList, OutputList);

	char* pPtr = OutputList->EndpointName;
	if (pAudioHardware->Open(RenderType, 50.f)) {
		pAudioHardware->GetEndpointInfo(RenderType, OutputLists);
		pAdvancedMixer->SetBufferSamples(OutputLists.EndpointFormat.Frames);
		/* Create base listener with emitter to play */
		if (pAdvancedMixer->CreateListener((void*)"X:\\WhitePlace.opus", listNode, OutputLists.EndpointFormat)) {
			pAdvancedMixer->AddEmitterToListener(listNode, pBaseEmitter);
			pBaseEmitter->SetState(eReplayState);
			//fr_f32 x = -2.25;
			//fr_f32 y = 2.75;
			//pBaseEmitter->SetOption(eXAxis, &x, sizeof(fr_f32));
			//pBaseEmitter->SetOption(eZAxis, &y, sizeof(fr_f32));
			//pAdvancedMixer->CreateListener((void*)"C:\\sem\\fasa_one.wav", listNode, OutputLists.EndpointFormat);
			//pAdvancedMixer->AddEmitterToListener(listNode, pBaseEmitterSecond);
			//pBaseEmitterSecond->SetState(eReplayState);
			pAdvancedMixer->SetMixFormat(OutputLists.EndpointFormat);
			pAudioHardware->Start();
		}
	}

	loop();
	FrDestroyInstance(pFresponze);
}

int main()
{
	//if constexpr (false) {
	//	Fresponze::InitMemory();
	//	int64_t reads = 0;
	//	DWORD dwp = 0;
	//	CFloatBuffer floatBufs[2];
	//	fr_f32* outFloat[2] = {};
	//	IFreponzeMapFile* mapFile = new CWindowsMapFile;
	//	CRIFFMediaResource res(mapFile);
	//	PcmFormat inFormat = {};
	//	PcmFormat outFormat = {};
	//
	//	outFormat.Bits = 32;
	//	outFormat.Channels = 2;
	//	outFormat.Frames = BUF_SIZE;
	//	outFormat.IsFloat = true;
	//	outFormat.SampleRate = 44100;
	//	res.OpenResource((void*)"I:/Downloads/ehren-paper_lights-96.wav");
	//	res.SetFormat(outFormat);
	//	res.GetFormat(inFormat);
	//
	//	for (size_t i = 0; i < 2; i++) {
	//		floatBufs->Resize(BUF_SIZE);
	//		outFloat[i] = floatBufs->Data();
	//	}
	//
	//	HANDLE hFile = CreateFileW(L"I:\\out.raw", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
	//	if (hFile == INVALID_HANDLE_VALUE || !hFile) return false;
	//	CalculateFrames64(inFormat.Frames, inFormat.SampleRate, outFormat.SampleRate, reads);
	//	reads *= sizeof(fr_f32);
	//
	//	while (reads > 0) {
	//		size_t writesize = (reads / sizeof(fr_f32)) < BUF_SIZE ? (reads / sizeof(fr_f32)) : BUF_SIZE;
	//		res.Read(writesize, outFloat);
	//		WriteFile(hFile, outFloat[0], (writesize * sizeof(fr_f32)), &dwp, nullptr);
	//		reads -= writesize * sizeof(fr_f32);
	//	}
	//	Fresponze::DestroyMemory();
	//} else

	test1();

	return 0;
}
