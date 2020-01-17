#include "Sound.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <tchar.h>
using namespace std;

void ShowHelp()
{
	TCHAR szPath[_MAX_PATH];
	TCHAR szDir[_MAX_DIR];
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	_tsplitpath_s(szPath, szDrive, szDir, szFName, szExt);
	::wcout << "Usage:" << ::endl;
	::wcout << szFName << " [-g | -im | [[-m | -u] [VOLUME]]" << ::endl;
	::wcout << "Options:" << ::endl;
	::wcout << "  -g,  --get     Get the current volume." << ::endl;
	::wcout << "  -im, --ismute  Get the mute status." << ::endl;
	::wcout << "  -m,  --mute    Mute the volume." << ::endl;
	::wcout << "  -u,  --unmute  Unmute the volume." << ::endl;
	::wcout << "  VOLUME         Set specifies the volume." << ::endl;
	::wcout << ::endl;
	::wcout << "Examples:" << ::endl;
	::wcout << "    > " << szFName << " -m" << ::endl;
	::wcout << "    > " << szFName << " -u 50" << ::endl;
	::wcout << "    > " << szFName << " 10" << ::endl;
	::wcout << "    > " << szFName << " +10" << ::endl;
	::wcout << "    > " << szFName << " -10" << ::endl;
}

bool isNumber(const std::string &str)
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

int main(int argc, char **argv)
{
	float fVolLv;
	float gap;
	bool toMute = false;
	bool toUnmute = false;
	bool toChangeVol = false;
	Sound sound;

	if (!sound.InitCom())
	{
		::cerr << "Failed to Initialize COM." << ::endl;
		return 1;
	}
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			string p = argv[i];
			::transform(p.begin(), p.end(), p.begin(), tolower);
			if (sizeof(p) > 1)
			{
				if (argc == 2 && (p == "-?" || p == "/?" || p == "-h" || p == "/h" || p == "--help" || p == "/help"))
				{
					ShowHelp();
					break;
				}
				else if (argc == 2 && (p == "-im" || p == "/im" || p == "--ismute" || p == "/ismute"))
				{
					if (sound.GetMute())
					{
						::cout << "True" << ::endl;
					}
					else
					{
						::cout << "False" << ::endl;
					}
					break;
				}
				else if (argc == 2 && (p == "-g" || p == "/g" || p == "--get" || p == "/get"))
				{
					::cout << sound.GetVolume() * 100 << ::endl;
					break;
				}
				else if ((p == "-m" || p == "/m" || p == "--mute" || p == "/mute") && !toUnmute)
				{
					if (sound.Mute())
					{
						::cout << "Turn mute on." << ::endl;
					}
					else
					{
						::cerr << "Failed to turn mute on." << ::endl;
						return 1;
					}
					toMute = true;
				}
				else if ((p == "-u" || p == "/u" || p == "--unmute" || p == "/unmute") && !toMute)
				{
					if (sound.Unmute())
					{
						::cout << "Turn mute off." << ::endl;
					}
					else
					{
						::cerr << "Failed to turn mute off." << ::endl;
						return 1;
					}
					toUnmute = true;
				}
				else if (p[0] == '+' && isNumber(p) && !toChangeVol)
				{
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
					toChangeVol = true;
				}
				else if ((p[0] == '-') && isNumber(p) && !toChangeVol)
				{
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
					toChangeVol = true;
				}
				else if (isNumber(p) && !toChangeVol)
				{
					fVolLv = stof((string)p);
					if (fVolLv < 0) fVolLv = 0;
					if (fVolLv > 100) fVolLv = 100;
					fVolLv = fVolLv / 100;
					toChangeVol = true;
				}
			}
			else if (isNumber(p) && !toChangeVol)
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
		if (toChangeVol)
		{
			if (sound.SetVolume(fVolLv))
			{
				::cout << "Set Volume to " << fVolLv * 100 << "." << ::endl;
			}
			else
			{
				::cerr << "Failed to Change The Volume." << ::endl;
				return 1;
			}
		}
	}
	else
	{
		ShowHelp();
	}
	sound.UninitCom();
	return 0;
}