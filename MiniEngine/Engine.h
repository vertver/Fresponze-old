#pragma once
#include "FresponzeTypes.h"
#include "FresponzeAdvancedMixer.h"


class IAudioEngine
{
protected:
	CAdvancedMixer* pAdvancedMixer = nullptr;

public:
};

class IGameEngine
{
protected:
	IAudioEngine* pAudioEngine = nullptr;
	void* pGraphicsContext = nullptr;


public:
};
