// Fog+0x17D60 (__fastcall): thin forwarder to kernel32 ReadFile(h, buf, n, &read, overlapped); returns void.
#include <cstdint>
typedef int (*readfile_fn)(void*, void*, uint32_t, uint32_t*, void*);
// 1.13c Fog+0x17D60
extern "C" void fog_sub_17d60(void* h, void* buf, uint32_t n, uint32_t* read, void* ov, readfile_fn ReadFile) {
    ReadFile(h, buf, n, read, ov);
}
