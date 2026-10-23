// 1.13c D2Lang+0x049A6: CRT exception-frame helper; saves EAX, [EBP+8], EBP into three globals.
#include <cstdint>

static uint32_t g_f580, g_f584, g_f588; // 6fc0f580 / 6fc0f584 / 6fc0f588

extern "C" void d2lang_49a6_save(uint32_t eax, uint32_t arg8, uint32_t ebp)
{
	g_f584 = arg8;
	g_f580 = eax;
	g_f588 = ebp;
}
