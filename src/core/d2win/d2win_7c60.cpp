// 1.13c D2Win+0x7C60 -- sub_7C60.
// Stdcall, ret 8: EAX = name, stack word 0 is not read, stack word 1 = out-pointer. The open helper (0x76BA) writes
// the handle over stack word 1, so the handle is the check value after the size read.
// Open (failure: report, return null); size = 0x7A60; holder = Fog alloc 0x7678 (base size + 0x320; null returns null);
// size check 0x7970 (ESI = handle, EDI = size; it pops its one stack word); close (0x76D2); *out = size when set;
// return the holder. A zero handle after the check is the retail exit path, unreachable once the open succeeds.
#include <cstdint>

struct D2Win7c60Env {
    uint32_t (*open)(const char* name);                      // 0x76BA: handle, 0 = failure
    void (*reportOpenError)(const char* name);               // GetLastError + Ordinal_10029 print
    uint32_t (*sub7a60)(uint32_t handle);                    // 0x7A60 -> size
    int32_t* (*alloc)(uint32_t base);                        // 0x7678 (0xCF tag)
    void (*sub7970)(uint32_t handle, uint32_t size);         // 0x7970 size check (status ignored)
    void (*close)(void);                                     // 0x76D2
};

extern "C" int32_t* d2win_7C60(const char* name, uint32_t* out, const D2Win7c60Env* e) {
    uint32_t handle = e->open(name);
    if (handle == 0) {
        e->reportOpenError(name);
        return nullptr;
    }
    uint32_t size = e->sub7a60(handle);
    int32_t* holder = e->alloc(size + 0x320);
    if (holder == nullptr) return nullptr;
    e->sub7970(handle, size);
    e->close();
    if (out != nullptr) *out = size;
    return holder;
}
