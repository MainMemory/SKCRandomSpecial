// SKCRandomSpecial.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "SKCModLoader.h"
#include "IniFile.hpp"

using std::unordered_map;
using std::string;
using std::transform;

DataArray(short, word_69EC1F, 0x69EC1F, 1);
DataPointer(void*, dword_8FFE446, 0x8FFE446);
DataPointer(short, word_8FFFFAC, 0x8FFFFAC);
void SetSSPalette(int ssnum)
{
	AReg a1 = { *(void **)((char *)&SS_Pal_Map_Ptrs[1] + 16 * (ssnum & 7)) };
	if (ssnum & 8)
		a1.Byte += 304;
	dword_8FFE446 = a1.Unknown;
	memcpy(&Target_palette[56], a1.Word, 16);
	memcpy(&Target_palette[40], &a1.Word[16], 6);
}

void SetCurSS()
{
	char emecnt = Emerald_count;
	char flag;
	if (SK_alone_flag)
		flag = 0;
	else
	{
		flag = SK_special_stage_flag;
		if (SK_special_stage_flag)
			emecnt = Super_emerald_count;
	}
	char curss;
	if (Debug_cheat_flag && Ctrl_1.Held & 0x40)
		curss = (char)Sound_test_sound;
	else
	{
		curss = Current_special_stage;
		if (emecnt < 7u)
		{
			int i = 5;
			flag *= 2;
			while (flag != Collected_emeralds_array[curss])
			{
				if (++curss >= 7u)
				{
					curss = 0;
					if (--i == -1)
					{
						curss = 7;
						break;
					}
				}
			}
		}
	}
	Current_special_stage = curss & 7;
}

bool usestagepal;
void LoadBSStageMap()
{
	Kosinski_Decomp(SSCompressedLayoutPointers[1], RAM_start);
	memset(Target_water_palette, 0, 0x600);
	if (BS_special_stage_flag)
		Current_special_stage = 0;
	else
		SetCurSS();
	SS_start_angle = 0x4000;
	SS_start_x = 0x1000;
	SS_start_y = 0x300;
	SS_perfect_count = 0;
	word_8FFFFAC = 0;
	char *stgid = (char *)&Blue_sphere_stage_number;
	short *wordthing = word_69EC1F;
	for (int part = 0; part < 4; part++)
	{
		char v22 = *(stgid++);
		SS_perfect_count += (unsigned __int8)RAM_start[v22 & 0x7F];
		word_8FFFFAC += (unsigned __int8)RAM_start[(v22 & 0x7F) + 128];
		char *src = &RAM_start[(signed __int16)((v22 & 0x7F) << 8) + 256];
		short v23 = *(wordthing++);
		short v24 = *(wordthing++);
		short v25 = *(wordthing++);
		char *dst = &Plane_buffer[*(wordthing++)];
		for (int y = 0; y < 16; y++)
		{
			short off = v24;
			for (int x = 0; x < 16; x++)
			{
				*(dst++) = src[off];
				off += v23;
			}
			dst += 16;
			v24 += v25;
		}
	}
	int ssnum;
	if (BS_special_stage_flag || usestagepal)
		ssnum = ((char *)Blue_sphere_stage_number)[2];
	else
	{
		ssnum = Current_special_stage;
		if (SK_alone_flag || SK_special_stage_flag)
			ssnum += 8;
	}
	SetSSPalette(ssnum);
}

void LoadS3StageMap()
{
	if (BS_special_stage_flag)
	{
		LoadBSStageMap();
		return;
	}
	SetCurSS();
	int ssnum = rand() & 7;
	memset(Target_water_palette, 0, 0x600);
	memcpy(Plane_buffer, SStageLayoutPtrs[ssnum], 0x400);
	unsigned short *src = (unsigned short*)((char*)SStageLayoutPtrs[ssnum] + 0x400);
	SS_start_angle = _byteswap_ushort(*(src++));
	SS_start_x = _byteswap_ushort(*(src++));
	SS_start_y = _byteswap_ushort(*(src++));
	SS_perfect_count = _byteswap_ushort(*(src++));
	if (!usestagepal)
	{
		ssnum = Current_special_stage;
		if (SK_alone_flag || SK_special_stage_flag)
			ssnum += 8;
	}
	SetSSPalette(ssnum);
}

void LoadSKStageMap()
{
	if (BS_special_stage_flag)
	{
		LoadBSStageMap();
		return;
	}
	Kosinski_Decomp(SSCompressedLayoutPointers[0], RAM_start);
	SetCurSS();
	int ssnum = rand() & 7;
	memset(Target_water_palette, 0, 0x600);
	memcpy(Plane_buffer, SSLayoutOffs_RAM[ssnum], 0x400);
	unsigned short *src = (unsigned short*)((char*)SSLayoutOffs_RAM[ssnum] + 0x400);
	SS_start_angle = _byteswap_ushort(*(src++));
	SS_start_x = _byteswap_ushort(*(src++));
	SS_start_y = _byteswap_ushort(*(src++));
	SS_perfect_count = _byteswap_ushort(*(src++));
	if (usestagepal)
		ssnum += 8;
	else
	{
		ssnum = Current_special_stage;
		if (SK_alone_flag || SK_special_stage_flag)
			ssnum += 8;
	}
	SetSSPalette(ssnum);
}

void LoadS3SKStageMap()
{
	if (rand() & 1)
		LoadSKStageMap();
	else
		LoadS3StageMap();
}

void LoadS3BSStageMap()
{
	if (rand() & 1)
		LoadBSStageMap();
	else
		LoadS3StageMap();
}

void LoadSKBSStageMap()
{
	if (rand() & 1)
		LoadBSStageMap();
	else
		LoadSKStageMap();
}

void LoadAllStageMap()
{
	switch (rand() % 3)
	{
	case 0:
		LoadS3StageMap();
		break;
	case 1:
		LoadSKStageMap();
		break;
	case 2:
		LoadBSStageMap();
		break;
	}
}

unordered_map<string, void (__cdecl *)()> optfuncmap = {
	{ "s3", LoadS3StageMap },
	{ "sk", LoadSKStageMap },
	{ "bs", LoadBSStageMap },
	{ "s3+sk", LoadS3SKStageMap },
	{ "s3+bs", LoadS3BSStageMap },
	{ "sk+bs", LoadSKBSStageMap },
	{ "all", LoadAllStageMap }
};

string s3mode, skmode;

void LoadSpecialStageMap_r()
{
	if (BS_special_stage_flag)
		LoadBSStageMap();
	else
	{
		unsigned char level = Current_zone_and_act >> 8;
		switch (GameSelection)
		{
		case 0: // S3K
			if (level == flying_battery_zone || level >= mushroom_hill_zone)
				optfuncmap[skmode]();
			else
				optfuncmap[s3mode]();
			break;
		case 1: // S&K
			optfuncmap[skmode]();
			break;
		case 2:
			optfuncmap[s3mode]();
			break;
		}
	}
}

extern "C"
{
	__declspec(dllexport) void OnFrame()
	{
		if (ModeSelection != 1)
		{
			if (Blue_sphere_stage_number == 0)
				Blue_sphere_stage_number = 0x3020100;
			char *tmp = (char*)&Blue_sphere_stage_number;
			Blue_sphere_stage_number = _byteswap_ulong(_byteswap_ulong(Blue_sphere_stage_number) + 0x1030507);
			tmp[0] &= 0x7F;
			if (tmp[1] >= 0x7Fu)
				tmp[1] -= 0x7F;
			if (tmp[2] >= 0x7Eu)
				tmp[2] -= 0x7E;
			if (tmp[3] >= 0x7Du)
				tmp[3] -= 0x7D;
		}
	}

	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		srand(_time32(nullptr));
		const IniFile *settings = new IniFile(std::string(path) + "\\config.ini");
		s3mode = settings->getString("", "S3Mode", "s3");
		transform(s3mode.begin(), s3mode.end(), s3mode.begin(), ::tolower);
		skmode = settings->getString("", "SKMode", "sk");
		transform(skmode.begin(), skmode.end(), skmode.begin(), ::tolower);
		delete settings;
		WriteJump(LoadSpecialStageMap, LoadSpecialStageMap_r);
	}

	__declspec(dllexport) ModInfo SKCModInfo = { ModLoaderVer };
}