// Storm's registry value delete (ordinal 428) on retail's layout: stdcall (key, name, flags), ret 0xC. The key and name are
// read before any test; an empty one sets the SErr error to 0x57 and answers 0. Otherwise the registry delete gets
// (key, name) with the flags in EAX and its answer is returned.
#include <cstdint>

struct Storm3C3E0Env {
    uint32_t (*del)(uint32_t flags, const char* key, const char* name);
    void (*setError)(uint32_t code);
};

uint32_t Storm_SRegDeleteValue32(const char* key, const char* name, uint32_t flags, const Storm3C3E0Env& e)
{
    if (!*key || !*name) { e.setError(0x57); return 0; }
    return e.del(flags, key, name);
}
