// D2Lang string-key lookup (ordinal 10010). Retail's three globals become the LangTables argument; the
// unloaded-table / NULL-key paths (error print + _exit(-1)) become the host seam `fatal`.
#include "../lang/strtable.h"
#include <cstdint>

// 1.13c D2Lang+0xA7B0
extern "C++" uint32_t d2lang_a7b0(const d2::LangTables* t, const char* key, const std::u16string** out,
                                  const std::u16string* empty, const std::u16string* missing, void (*fatal)(int32_t)) {
    if (!t) { fatal(700); return 0; }
    if (!key) { fatal(0x2c2); return 0; }
    if (*key == 0) { *out = empty; return 0; }
    const std::u16string* v = nullptr;
    uint32_t idx = t->tblIndex(key, &v);   // patch +10000, expansion +20000, base raw: same order as retail
    *out = v ? v : missing;                // retail 9FB0 missing-string entry, index 0
    return v ? idx : 0;
}
