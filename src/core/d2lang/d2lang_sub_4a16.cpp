// 1.13c D2Lang+0x04A16: CRT setlocale ctype-table init (builds _ctype flags and case-swap table).
#include <cstdint>

// Retail asks GetCPInfo/GetStringTypeA/LCMapStringA (lead bytes blanked to ' ' first); the OS
// answers are inputs here. pTypes==nullptr is the GetCPInfo-failed branch (ASCII C locale).
// pFlags: DAT 6fc11ae1[256] (OR-ed in); pSwap: DAT 6fc11c00[256] (case swap, 0 if not a letter).
extern "C" void d2lang_4a16_init_ctype(uint8_t* pFlags, uint8_t* pSwap, const uint16_t* pTypes,
                                       const uint8_t* pLower, const uint8_t* pUpper)
{
	for (uint32_t i = 0; i < 0x100; ++i)
	{
		if (pTypes)
		{
			if (pTypes[i] & 1) { pFlags[i] |= 0x10; pSwap[i] = pLower[i]; }
			else if (pTypes[i] & 2) { pFlags[i] |= 0x20; pSwap[i] = pUpper[i]; }
			else pSwap[i] = 0;
		}
		else if (i >= 0x41 && i <= 0x5a) { pFlags[i] |= 0x10; pSwap[i] = uint8_t(i + 0x20); }
		else if (i >= 0x61 && i <= 0x7a) { pFlags[i] |= 0x20; pSwap[i] = uint8_t(i - 0x20); }
		else pSwap[i] = 0;
	}
}
