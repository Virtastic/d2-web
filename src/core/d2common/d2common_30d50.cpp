// 1.13c D2Common+0x30D50: regs eax = row, ecx = kind. Kind 0 reads the byte at +0x40 of the charstats
// row (stride 0xC4) and kind 1 the short at +0x32 of the monstats row (stride 0x1A8); each is answered
// as a value in 8.8 fixed point. A row past its table, or any other kind, answers 0 (retail reads an
// absolute address there, which the capture never uses).
#include <cstdint>
#include "D2DataTbls.h"

extern "C" int32_t D2Common_30D50_SpecialVelocity(int32_t row, int32_t kind)
{
	if (!sgptDataTables)
		return 0;
	if (kind == 0) {
		if (row < 0 || row >= sgptDataTables->nCharStatsTxtRecordCount)
			return 0;
		const auto* base = reinterpret_cast<const uint8_t*>(sgptDataTables->pCharStatsTxt);
		return static_cast<int32_t>(base[row * 0xC4 + 0x40]) << 8;
	}
	if (kind == 1) {
		if (row < 0 || row >= sgptDataTables->nMonStatsTxtRecordCount)
			return 0;
		const auto* base = reinterpret_cast<const uint8_t*>(sgptDataTables->pMonStatsTxt);
		int16_t value = 0;
		__builtin_memcpy(&value, base + row * 0x1A8 + 0x32, sizeof(value));
		return static_cast<int32_t>(value) << 8;
	}
	return 0;
}
