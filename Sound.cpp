#include <iostream>
#include <algorithm>
#include <string>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <tchar.h>
using namespace std;

class Volume
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

bool Volume::InitCom()
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

void Volume::UninitCom()
{
	endpointVolume->Release();
	CoUninitialize();
}

bool Volume::Mute()
{
	hr = endpointVolume->SetMute(TRUE, NULL);
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	return true;
}

bool Volume::Unmute()
{
	hr = endpointVolume->SetMute(FALSE, NULL);
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	return true;
}

bool Volume::GetMute()
{
	BOOL pbMute = FALSE;
	hr = endpointVolume->GetMute(&pbMute);
	return (bool)pbMute;
}

bool Volume::SetVolume(float changeTo)
{
	hr = endpointVolume->SetMasterVolumeLevelScalar(changeTo, NULL);
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	return true;
}

float Volume::GetVolume()
{
	hr = endpointVolume->GetMasterVolumeLevelScalar(&g_fVolLv);
	if (FAILED(hr))
	{
		UninitCom();
		return false;
	}
	return g_fVolLv;
}

void ShowHelp()
{
	TCHAR szPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	_tsplitpath_s(szPath, szDrive, szDir, szFName, szExt);
	wcout << szFName << " [-get][[-mute|-unmute] volume]" << endl;
	wcout << "  -get      Show Current Volume." << endl;
	wcout << "  -mute     To Mute the Volume." << endl;
	wcout << "  -unmute   To Unmute the Volume." << endl;
	wcout << "  volume    Specifies the Volume." << endl;
	wcout << endl;
	wcout << "Examples:" << endl;
	wcout << "    > " << szFName << " -m" << endl;
	wcout << "    > " << szFName << " -u 50" << endl;
	wcout << "    > " << szFName << " 10" << endl;
	wcout << "    > " << szFName << " +10" << endl;
	wcout << "    > " << szFName << " -10" << endl;
}


//
//bool isNumber(const std::string& s)
//{
//    std::string::const_iterator it = s.begin();
//    while (it != s.end() && isdigit(*it)) ++it;
//    return !s.empty() && it == s.end();
//}
//

bool is_number(const std::string &str)
{
	if ((str[0] == '-') || (str[0] == '+'))
	{
		return std::all_of(str.begin() + 1, str.end(), ::isdigit);
	}
	else
	{
		return std::all_of(str.begin(), str.end(), ::isdigit);
	}
}

bool is_digits(const std::string &str)
{
	//return str.find_first_not_of("0123456789") == std::string::npos;
	return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}

bool is_option(char *cstr, std::string option)
{
	string str = cstr;
	transform(str.begin(), str.end(), str.begin(), toupper);
	transform(option.begin(), option.end(), option.begin(), toupper);
	for (int i = 0; i<(int)str.size(); i++)
	{
		if (!(str[i] == option[i]))
		{
			return false;
		}
	}
	return true;
}

int main(int argc, char **argv)
{
	char *p;
	float fVolLv;
	float gap;
	bool initCom = false;
	bool toMute = false;
	bool toUnmute = false;
	bool toChangeVol = false;
	bool shCurrent = false;
	bool shHelp = false;
	bool shVolume = false;
	bool shMute = false;

	Volume sound;

	if (!sound.InitCom())
	{
		cerr << "Failed to Initialize COM." << endl;
		return 1;
	}

	if (argc>1)
	{
		for (int i = 1; i < argc; i++)
		{
			p = argv[i];
			if (sizeof(p) > 1)
			{
				//‚±‚±“®‚¢‚Ä‚È‚¢
				if (argc == 2 && sizeof(p)>4 && (is_option(p, "-getmute") || is_option(p, "/getmute")))
				{
					shMute = true;
					break;
				}
				else if (argc == 2 && (is_option(p, "-get") || is_option(p, "/get")))
				{
					shVolume = true;
					break;
				}
				else if (is_option(p, "-help") || is_option(p, "/help") || is_option(p, "/?") || is_option(p, "-?"))
				{
					shHelp = true;
				}
				else if (is_option(p, "-current") || is_option(p, "/current"))
				{
					shCurrent = true;
				}
				else if (is_option(p, "-mute") || is_option(p, "/mute"))
				{
					toMute = true;
				}
				else if (is_option(p, "-unmute") || is_option(p, "/unmute"))
				{
					toUnmute = true;
				}
				else if ((p[0] == '+') && is_number(p) && !toChangeVol)
				{
					toChangeVol = true;
					gap = fabs(stof((string)p)) / 100;
					fVolLv = sound.GetVolume();
					if ((fVolLv + gap) > 1)
					{
						fVolLv = 1;
					}
					else
					{
						fVolLv += gap;
					}
				}
				else if ((p[0] == '-') && is_number(p) && !toChangeVol)
				{
					toChangeVol = true;
					gap = fabs(stof((string)p)) / 100;
					fVolLv = sound.GetVolume();
					if ((fVolLv - gap) < 0)
					{
						fVolLv = 0;
					}
					else
					{
						fVolLv -= gap;
					}
				}
				else if (is_number(p) && !toChangeVol)
				{
					toChangeVol = true;
					fVolLv = stof((string)p);
					if (fVolLv < 0) fVolLv = 0;
					if (fVolLv > 100) fVolLv = 100;
					fVolLv = fVolLv / 100;
				}
				else
				{
					cerr << "Invalid Argument(s)." << endl;
					return 1;
				}
			}
			else if (is_number(p) && !toChangeVol)
			{
				toChangeVol = true;
				fVolLv = stof((string)p) / 100;
			}
			else
			{
				cerr << "Invalid Argument(s)." << endl;
				return 1;
			}
		}
		if (shMute)
		{
			if (sound.GetMute())
			{
				cout << "True" << endl;
			}
			else
			{
				cout << "False" << endl;
			}
			return 0;
		}
		else if (shVolume)
		{
			cout << sound.GetVolume() * 100 << endl;
			return 0;
		}
		if (shCurrent)
		{
			//cout << "Current Volume: " << sound.GetVolume() * 100 << endl;
			//cout << "         Muted: " << boolalpha << sound.GetMute() << endl;
			if (sound.GetMute())
			{
				cout << "Current volume is " << sound.GetVolume() * 100 << ", Muting." << endl;
			}
			else
			{
				cout << "Current volume is " << sound.GetVolume() * 100 << ", Unmuting." << endl;
			}
		}
		if (!(toMute && toUnmute))
		{
			if (toMute)
			{
				if (sound.Mute())
				{
					cout << "Turn mute on." << endl;
				}
				else
				{
					cerr << "Failed to turn mute on." << endl;
					return 1;
				}
			}
			else if (toUnmute)
			{
				if (sound.Unmute())
				{
					cout << "Turn mute off." << endl;
				}
				else
				{
					cerr << "Failed to turn mute off." << endl;
					return 1;
				}
			}
		}
		else
		{
			cerr << "Invalid Argument(s)." << endl;
			return 1;
		}
		if (toChangeVol)
		{
			if (sound.SetVolume(fVolLv))
			{
				cout << "Set Volume to " << fVolLv * 100 << "." << endl;
			}
			else
			{
				cerr << "Failed to Change The Volume." << endl;
				return 1;
			}
		}
		if (shHelp)
		{
			cerr << endl;
			ShowHelp();
		}
	}
	else
	{
		ShowHelp();
	}

	sound.UninitCom();
	return 0;
}
