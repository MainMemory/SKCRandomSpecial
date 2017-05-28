// SKCRandomSpecial.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <cstdlib>
#include "..\skc-mod-loader\SKCModLoader\SKCModLoader.h"

DataArray(short, word_69EC1F, 0x69EC1F, 1);
DataPointer(int, dword_8FFE446, 0x8FFE446);
DataPointer(short, word_8FFFFAC, 0x8FFFFAC);
void LoadSpecialStageMap_r()
{
	Kosinski_Decomp(SSCompressedLayoutPointers[1], RAM_start);
	memset(Target_water_palette, 0, 0x600);
	if (BS_special_stage_flag)
		Current_special_stage = 0;
	else
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
	if (BS_special_stage_flag)
		ssnum = ((char *)Blue_sphere_stage_number)[2];
	else
	{
		ssnum = Current_special_stage;
		if (SK_alone_flag || SK_special_stage_flag)
			ssnum += 8;
	}
	void *a2 = Target_palette;
	void *a1 = *(void **)((char *)&SS_Pal_Map_Ptrs[1] + 16 * (ssnum & 7));
	if (ssnum & 8)
		a1 = (char *)a1 + 304;
	dword_8FFE446 = (int)a1;
	*((int *)a2 + 28) = *(int *)a1;
	a1 = (char *)a1 + 4;
	*((int *)a2 + 29) = *(int *)a1;
	a1 = (char *)a1 + 4;
	*((int *)a2 + 30) = *(int *)a1;
	a1 = (char *)a1 + 4;
	*((int *)a2 + 31) = *(int *)a1;
	a1 = (char *)a1 + 4;
	*((int *)a2 + 20) = *((int *)a1 + 4);
	*((short *)a2 + 42) = *((short *)a1 + 10);
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
		WriteJump(LoadSpecialStageMap, LoadSpecialStageMap_r);
	}

	__declspec(dllexport) ModInfo SKCModInfo = { ModLoaderVer };
}