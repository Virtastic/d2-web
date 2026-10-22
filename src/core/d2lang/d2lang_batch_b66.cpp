// D2Lang batch b66. Skipped (CRT/SEH/direct-Win32/unaff locals): 16E4 1E2D 1EA1 260C 272E 1C25 25FE 22CA 2478 7C00.
#include <cstdint>
#include <cstring>
#include <initializer_list>
struct Unicode;
extern "C" int d2lang_unicodeWidth(const char* s, int n);  // Unicode::unicodeWidth
extern "C" void d2lang_toUnicode(Unicode* d, const char* s, int n);  // Unicode::toUnicode

extern "C" __attribute__((weak)) void* Ordinal_10042(int line, int zero);  // Fog alloc(size, file, line) as called in retail
extern "C" __attribute__((weak)) void Ordinal_10043(int line, int zero);
extern "C" void d2lang_crt_amsg_exit(int code);

struct LangCacheNode { Unicode* wide; char* key; LangCacheNode* next; };
static LangCacheNode* g_langCache;  // D2Lang+0x10A60

// 1.13c D2Lang+0x9fb0: EAX=key (hidden register) -> explicit leading param. `suffix` is the
// string at D2Lang+0x10568 appended to the key before conversion.
Unicode* d2lang_cacheLookup(const char* key, const char* suffix)
{
    for (LangCacheNode* n = g_langCache; n; n = n->next)
        if (!strcmp(key, n->key)) return n->wide;
    auto* n = (LangCacheNode*)Ordinal_10042(0x25b, 0);
    n->next = g_langCache;
    g_langCache = n;
    n->key = (char*)Ordinal_10042(0x266, 0);
    strcpy(n->key, key);
    size_t kl = strlen(key), sl = strlen(suffix);
    char* buf = (char*)Ordinal_10042(0x26e, 0);
    memcpy(buf, key, kl + 1);
    memcpy(buf + kl, suffix, sl + 1);
    int w = d2lang_unicodeWidth(buf, (int)(kl + sl));
    n->wide = (Unicode*)Ordinal_10042(0x273, 0);
    d2lang_toUnicode(n->wide, buf, w);
     Ordinal_10043(0x279, 0);
    return n->wide;
}

// 1.13c D2Lang+0x152c: EBP frame/EDI (hidden registers) -> explicit params.
void d2lang_checkFrame(int frameField, int expected)
{
    if (frameField != expected) d2lang_crt_amsg_exit(8);
}
