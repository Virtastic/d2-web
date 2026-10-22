// Fog+0x17C40 (__fastcall): create every directory component of a path (each prefix ending before a '\\' after char 0).
// Returns 1 ok, 0 on null/empty path, prefix >= 0x104 bytes, or CreateDirectoryA failure other than already-exists.
#include <cstdint>
#include <cstring>
typedef int (*mkdir_fn)(const char*, void*);
typedef uint32_t (*u32_fn)();
typedef uint32_t (*attr_fn)(const char*);
// 1.13c Fog+0x17C40
extern "C" uint32_t fog_sub_17c40(const char* p, mkdir_fn CreateDirectoryA, u32_fn GetLastError, attr_fn GetFileAttributesA) {
    if (!p || !*p) return 0;
    char buf[0x104];
    for (const char* s = strchr(p + 1, '\\'); s; s = strchr(s + 1, '\\')) {
        uint32_t n = (uint32_t)(s - p);
        if (n >= 0x104) return 0;
        memcpy(buf, p, n);
        buf[n] = 0;
        if (!CreateDirectoryA(buf, nullptr)) {
            uint32_t e = GetLastError();
            if ((e != 0xb7 && e != 5) || !(GetFileAttributesA(buf) & 0x10)) return 0;
        }
    }
    return 1;
}
