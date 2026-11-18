#pragma once

#include <cstdint>

extern char D2GAME_CRT_cDecimalPoint;
extern uint32_t D2GAME_CRT_dwStaticInitFlags;

//D2Game.0xF609C
void __cdecl CRT_ForceDecimalPoint_F609C(char* pszNumber);
//D2Game.0xF78A0
uint32_t CRT_Unwind_F78A0();
