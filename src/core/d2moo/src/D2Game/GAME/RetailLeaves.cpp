// 1.13c D2Game leaf routines with no 1.10f D2MOO counterpart, taken from the 1.13c disassembly.
#include <cstdint>

// 1.13c D2Game+0x13230: register-arg leaf, EAX = value, ESI = floor, [esp+4] = percent (ret 4). value * percent / 100 (low 32 bits of
// the product, rounded toward zero), raised to the floor.
int32_t GAME_PercentOfWithFloor(int32_t nValue, int32_t nFloor, int32_t nPercent)
{
	const int32_t nProduct = (int32_t)((uint32_t)nValue * (uint32_t)nPercent);
	const int32_t nQuot = (int32_t)((int64_t)nProduct / 100);
	return nQuot < nFloor ? nFloor : nQuot;
}

// 1.13c D2Game+0x49A20: EAX + 0xD. Retail logs and _exit(-1) on a null argument; ours does not model _exit.
uint32_t GAME_Plus0xD(uint32_t p)
{
	return p + 0xd;
}

// 1.13c D2Game+0x5F90 (_strcmp): compare NUL-terminated strings as unsigned bytes, -1 / 0 / 1.
extern "C" int32_t d2game_strcmp_5f90(const uint8_t* a, const uint8_t* b)
{
	for (;; ++a, ++b)
	{
		if (*a != *b) return *a < *b ? -1 : 1;
		if (*a == 0) return 0;
	}
}
