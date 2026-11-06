// D2Common 0x3DB20: stdcall(room), ret 4. The cell store arrives through the pointer at 0x9B250 (0x36C20: ECX = cell, EDX = value).
#pragma once
#include <cstdint>
extern "C" void D2Common_3DB20_With(uint32_t room, void (*set)(uint32_t ecx, uint32_t edx));
extern "C" void D2Common_3DB20(uint32_t room);
