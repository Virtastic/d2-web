// 1.13c D2Common+0x5D800: fastcall object-name lookup. ECX = object id, EDX = output
// buffer. A valid record (base DAT_6FDF0B94 + id*0x1C0) with a non-empty name at +0 is copied, NUL included; any other
// valid id gets "object %d". Ids outside 0..count-1 are fatal in retail (DataTbls error 0xDD / 0xDE): the port returns them.
#include <cstdint>

struct D2Common5D800Table {
    uintptr_t pBase;       // DAT_6FDF0B94 (0xA0B94): 0x1C0-byte records
    int32_t   nCount;      // DAT_6FDF0B98 (0xA0B98), compared signed
};

// 1.13c D2Common+0x5D800. Returns 0 on success, 0xDD (id >= count) or 0xDE (id < 0) for the retail fatal paths.
extern "C" int32_t D2Common_5D800_Name(const D2Common5D800Table* t, int32_t nId, char* szOut)
{
    if (nId >= t->nCount) return 0xDD;
    if (nId < 0) return 0xDE;
    const char* p = reinterpret_cast<const char*>(t->pBase + (uintptr_t)nId * 0x1C0);
    if (p != nullptr && p[0] != '\0') {
        for (;;) {
            const char c = *p++;
            *szOut++ = c;
            if (c == '\0') break;
        }
        return 0;
    }
    // "object %d" with a non-negative id (the negative case returned above)
    char digits[12];
    int n = 0;
    uint32_t v = (uint32_t)nId;
    do { digits[n++] = (char)('0' + v % 10); v /= 10; } while (v != 0);
    const char* prefix = "object ";
    for (; *prefix; ++prefix) *szOut++ = *prefix;
    while (n > 0) *szOut++ = digits[--n];
    *szOut = '\0';
    return 0;
}
