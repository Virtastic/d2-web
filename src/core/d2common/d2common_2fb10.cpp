// 1.13c D2Common+0x2FB10: regs eax = row, ecx = table kind (0 flat, 1 monstats row), edx = column
// (0xF selects the second short). Table 0 answers 0x65 for column 3 and 0xD5 otherwise; table 1 reads
// the monstats row's short at +0x36 or +0x38. A bad row or any other kind answers 0.
#include <cstdint>
#include "D2DataTbls.h"

extern "C" int32_t D2Common_2FB10_LevelCapOrDifficulty(int32_t row, int32_t kind, int32_t column)
{
	if (kind == 0)
		return (column == 3) ? 0x65 : 0xD5;
	if (kind != 1 || !sgptDataTables)
		return 0;
	if (row < 0 || row >= sgptDataTables->nMonStatsTxtRecordCount)
		return 0;
	const auto* base = reinterpret_cast<const uint8_t*>(sgptDataTables->pMonStatsTxt);
	int16_t value = 0;
	const int offset = (column != 0xF) ? 0x36 : 0x38;
	__builtin_memcpy(&value, base + row * 0x1A8 + offset, sizeof(value));
	return value;
}
