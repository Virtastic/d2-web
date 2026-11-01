// D2Common 1.13c 0x1C020: parse one cubemain.txt input cell (EAX = text, EBX = 8-byte output record) into
// flags (+0), class mask (+1), item word (+2), link index (+4), quality (+6) and quantity (+7).
// The text is edited in place (quotes and separators become NULs). The three table lookups (the item-type
// linker 0x9240, the by-name linker 0x923A, the item-code resolver 0x71960) are retail callees and come in
// through the environment; the string helpers (1B9D0, strcspn, stricmp, atol) run here.
#include <cstdint>
#include <cstring>
#include <strings.h>

void D2Common_1B9D0_CopyFieldChars(char* dst, const char* src);

struct D2Common1C020Env {
    uint32_t typeLinker;                 // global 0xA0ACC
    uint32_t setLinker, setRows;         // globals 0xA0AEC, 0xA0AF0 (row stride 0x1B8)
    uint32_t uniqLinker, uniqRows;       // globals 0xA0AF8, 0xA0AFC (row stride 0x14C)
    int32_t (*byCode)(uint32_t linker, uint32_t code);              // 0x9240 (linker, code, 0)
    int32_t (*byName)(uint32_t linker, const char* name);           // 0x923A (linker, name, 0)
    uint32_t (*itemWord)(uint32_t code, uint32_t* out);             // 0x71960 (code, &out)
};

namespace {
int32_t atol32(const char* s) {   // the C runtime's atol: blanks, sign, decimal digits, 32-bit wrap
    while (*s == ' ' || (*s >= 9 && *s <= 13)) ++s;
    bool neg = false;
    if (*s == '-' || *s == '+') neg = *s++ == '-';
    uint32_t v = 0;
    while (*s >= '0' && *s <= '9') v = v * 10 + uint32_t(*s++ - '0');
    return int32_t(neg ? 0u - v : v);
}
uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void wr16(uint8_t* p, uint32_t v) { p[0] = uint8_t(v); p[1] = uint8_t(v >> 8); }
}

// 1.13c D2Common+0x1C020
extern "C" uint32_t D2Common_1C020_CubeInputParse(const D2Common1C020Env* e, char* s, uint8_t* o) {
    if (*s == '"') ++s;
    for (char* p = s; *p; ++p) if (*p == '"') *p = 0;
    if (!*s) return 1;
    size_t n = strcspn(s, ",");
    char* tail;
    if (s[n]) { s[n] = 0; tail = s + n + 1; } else tail = nullptr;
    const bool shortHead = strlen(s) < 5;
    uint32_t code = 0, w = 0;
    bool done = false;
    if (shortHead && strcasecmp(s, "any") == 0) { o[0] |= 1; o[2] = 0xFF; o[3] = 0xFF; done = true; }
    if (!done && shortHead) {
        D2Common_1B9D0_CopyFieldChars((char*)&code, s);
        int32_t i = e->byCode(e->typeLinker, code);
        if (i >= 0) { o[0] |= 2; wr16(o + 2, uint32_t(i)); done = true; }
    }
    if (!done && shortHead) {
        D2Common_1B9D0_CopyFieldChars((char*)&code, s);
        if (e->itemWord(code, &w)) { o[0] |= 1; wr16(o + 2, w); done = true; }
    }
    if (!done) {
        int32_t i;
        if (e->uniqLinker && (i = e->byName(e->uniqLinker, s)) >= 0) {
            e->itemWord(rd32(e->uniqRows + uint32_t(i) * 0x14C + 0x28), &w);
            o[0] |= 0x41; o[6] = 7; wr16(o + 4, uint32_t(i) + 1); wr16(o + 2, w);
        } else if (e->setLinker && (i = e->byName(e->setLinker, s)) >= 0) {
            e->itemWord(rd32(e->setRows + uint32_t(i) * 0x1B8 + 0x28), &w);
            o[0] |= 0x41; o[6] = 5; wr16(o + 4, uint32_t(i) + 1); wr16(o + 2, w);
        } else return 0;
    }
    while (tail) {
        char* name = tail;
        size_t k = strcspn(name, "=,");
        char* val;
        if (name[k]) { name[k] = 0; val = name + k + 1; } else val = nullptr;
        if (strcmp(name, "qty") == 0) {
            if (!val) val = name + k;   // retail would read through a null here; an empty value stands in
            size_t m = strcspn(val, ",");
            if (val[m]) { val[m] = 0; tail = val + m + 1; } else tail = nullptr;
            o[7] = uint8_t(atol32(val));
            continue;
        }
        static const struct { const char* nm; int slot; uint8_t v; } kMods[] = {
            {"low", 6, 1}, {"nor", 6, 2}, {"hiq", 6, 3}, {"mag", 6, 4}, {"set", 6, 5}, {"rar", 6, 6}, {"uni", 6, 7},
            {"crf", 6, 8}, {"tmp", 6, 9}, {"nos", 0, 4}, {"sock", 0, 8}, {"noe", 0, 0x20}, {"eth", 0, 0x10},
            {"upg", 0, 0x80}, {"bas", 1, 1}, {"exc", 1, 2}, {"eli", 1, 4}, {"nru", 1, 8} };
        bool hit = false;
        for (const auto& m : kMods) {
            if (strcmp(name, m.nm)) continue;
            if (m.slot == 6) o[6] = m.v; else o[m.slot] |= m.v;
            hit = true; break;
        }
        if (!hit) return 1;
        tail = val;
    }
    return 1;
}
