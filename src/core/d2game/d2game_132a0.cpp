// Item-code lookup (1.13c D2Game+0x132A0, split out of d2game_batch_131e0.cpp) in the D2Common global at
// ordinal 10480's address: a {count at +0, rows at +4} header over 0x20-byte rows. EAX is the unit (its +4
// class id selects the items.txt record; a null unit asks for class -1), EBX a selector: 0 compares the
// record's +0x80 code against row+0x1C, non-zero against row+0x18. A null header answers -1; a null
// items.txt record or row array answers the count; a miss answers the count.
#include "d2game_131e0_env.h"

// 1.13c D2Game+0x132A0
extern "C" int32_t d2g_132a0(const void* unit, uint32_t which, const D2GameBatch131E0Env* env)
{
    const void* list = env->ordinal10480();
    const void* rec = env->ordinal10695(unit != nullptr ? (int32_t)rd32(unit, 4) : -1);
    if (list == nullptr) return -1;

    const int32_t count = (int32_t)rd32(list, 0);
    const uint32_t rows = rd32(list, 4);
    if (rec == nullptr || rows == 0) return count;

    const uint32_t code = rd32(rec, 0x80);
    for (int32_t i = 0; i < count; ++i) {
        const void* row = (const void*)(uintptr_t)(rows + (uint32_t)i * 0x20);
        const uint32_t value = which == 0 ? rd32(row, 0x1C) : rd32(row, 0x18);
        if (code == value) return i;
    }
    return count;
}
