#include "Sound.h"

bool Sound::InitCom()
{
	CoInitialize(NULL);
	deviceEnumerator = NULL;
	defaultDevice = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}

	endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	defaultDevice->Release();
	defaultDevice = NULL;
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	return true;
}

void Sound::UninitCom()
{
	endpointVolume->Release();
	CoUninitialize();
}

bool Sound::Mute()
{
	hr = endpointVolume->SetMute(TRUE, NULL);
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	return true;
}

bool Sound::Unmute()
{
	hr = endpointVolume->SetMute(FALSE, NULL);
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	return true;
}

bool Sound::GetMute()
{
	BOOL pbMute = FALSE;
	hr = endpointVolume->GetMute(&pbMute);
	return (bool)pbMute;
}

bool Sound::SetVolume(float changeTo)
{
	hr = endpointVolume->SetMasterVolumeLevelScalar(changeTo, NULL);
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	return true;
}

float Sound::GetVolume()
{
	hr = endpointVolume->GetMasterVolumeLevelScalar(&g_fVolLv);
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	return g_fVolLv;
}
