#pragma once
#include <mmdeviceapi.h>
#include <endpointvolume.h>

class Sound
{
private:
	HRESULT hr;
	IMMDeviceEnumerator *deviceEnumerator;
	IMMDevice *defaultDevice;
	IAudioEndpointVolume *endpointVolume;

public:
	bool InitCom();
	bool Mute();
	bool Unmute();
	bool GetMute();
	bool SetVolume(float changeTo);
	float GetVolume();
	void UninitCom();
	float g_fVolLv;
};
