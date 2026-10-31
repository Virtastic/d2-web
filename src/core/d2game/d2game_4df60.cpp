// D2Game+0x4DF60 -- divide a value by the difficulty record's divisor.
// al = difficulty, esi = value. r = difficulty record; if r and its dword +0x1c are non-zero, value / r[+0x1c] (signed), else value.
#include <cstdint>
#include <cstring>
#include "../tables/shared.h"
#include "../d2moo/compat/datatbls_bridge.h"
#include "D2DataTbls.h"

// 1.13c D2Game+0x4DF60
extern "C" int32_t d2game_4df60_impl(uint8_t difficulty, int32_t value)
{
    const uint8_t* r = (const uint8_t*)DATATBLS_GetDifficultyLevelsTxtRecord(difficulty);
    if (r == nullptr) return value;
    int32_t d; std::memcpy(&d, r + 0x1c, 4);
    if (d == 0) return value;
    return value / d;
}

extern "C" int d2game_4df60_tables()
{
    return d2::drlgLoadTables(d2::sharedVfs(), d2::sharedLang()) ? 1 : 0;
}
