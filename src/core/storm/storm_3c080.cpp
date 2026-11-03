// Storm's registry dword save (ordinal 426) on retail's layout: stdcall (key, name, flags, value), ret 0x10. Both
// strings are read before any test, so neither may be null; an empty one sets the SErr error to 0x57 and answers 0.
// Otherwise the shared registry writer gets (key, name, type 4, &value, 4) with the flags in EBX and its answer is returned.
#include <cstdint>

struct Storm3C080Env {
    uint32_t (*write)(uint32_t flags, const char* key, const char* name, uint32_t type, const uint32_t* data, uint32_t len);
    void (*setError)(uint32_t code);
};

uint32_t Storm_SRegSaveValue32(const char* key, const char* name, uint32_t flags, uint32_t value, const Storm3C080Env& e)
{
    if (!*key || !*name) { e.setError(0x57); return 0; }
    return e.write(flags, key, name, 4, &value, 4);
}
