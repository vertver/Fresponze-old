#include "FresponzeXAudio2Endpoint.h"

void
CXAudio2AudioEndpoint::ThreadProc()
{

}

void
CXAudio2AudioEndpoint::SetDeviceInfo(EndpointInformation& DeviceInfo)
{
	memcpy(&EndpointInfo, &DeviceInfo, sizeof(EndpointInformation));
}

void
CXAudio2AudioEndpoint::GetDeviceInfo(EndpointInformation& DeviceInfo)
{
	memcpy(&DeviceInfo, &EndpointInfo, sizeof(EndpointInformation));
}

void
CXAudio2AudioEndpoint::SetCallback(IAudioCallback* pCallback)
{
	pCallback->Clone((void**)&pAudioCallback);
}

bool
CXAudio2AudioEndpoint::Open(fr_f32 Delay)
{

}

bool
CXAudio2AudioEndpoint::Close()
{

}

bool
CXAudio2AudioEndpoint::Start()
{

}

bool
CXAudio2AudioEndpoint::Stop()
{

}
