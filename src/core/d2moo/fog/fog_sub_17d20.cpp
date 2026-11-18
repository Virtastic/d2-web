// Fog+0x17D20 (__fastcall): thin forwarder to kernel32 SetFilePointer(h, lo, &hi, method); returns void.
#include <cstdint>
typedef uint32_t (*setfilepointer_fn)(void*, int32_t, int32_t*, uint32_t);
// 1.13c Fog+0x17D20
extern "C" void fog_sub_17d20(void* h, int32_t lo, int32_t* hi, uint32_t method, setfilepointer_fn SetFilePointer) {
    SetFilePointer(h, lo, hi, method);
}
