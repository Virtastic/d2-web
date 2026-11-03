// Storm's registry string save (ordinal 425) on retail's layout: stdcall (key, name, flags, string), ret 0x10. The key
// and name are read before any test; an empty key or name, or a null string, sets the SErr error to 0x57 and answers 0.
// Otherwise the string length (Storm's checked strlen) plus one is the size handed to the registry writer with type 1
// and the flags in EBX; its answer is returned.
#include <cstdint>

struct Storm3C0E0Env {
    uint32_t (*strLen)(const char* s);
    uint32_t (*write)(uint32_t flags, const char* key, const char* name, uint32_t type, const char* data, uint32_t len);
    void (*setError)(uint32_t code);
};

uint32_t Storm_SRegSaveString32(const char* key, const char* name, uint32_t flags, const char* str, const Storm3C0E0Env& e)
{
    if (!*key || !*name || !str) { e.setError(0x57); return 0; }
    const uint32_t n = e.strLen(str) + 1;
    return e.write(flags, key, name, 1, str, n);
}
