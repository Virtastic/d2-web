// CRT stdio helper. Not a FILE walker: the second-chance unlock used by the stream-walk helpers.
#include <cstdint>
#include <emscripten.h>

extern "C" void d2_client_47ab(uint32_t idx);   // _unlock(n)

// 1.13c D2Client+0x5611: cdecl __unlock_file2(int index, void* file). index < 0x14: _unlock(index + 0x10).
// Otherwise LeaveCriticalSection(file + 0x20) through the import slot 0x6FB7EFB8 (the caller supplies it).
typedef void (*LeaveCs)(void*);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_5611(int32_t index, void* file, LeaveCs leave) {
    if (index < 0x14) { d2_client_47ab((uint32_t)(index + 0x10)); return; }
    leave((uint8_t*)file + 0x20);
}
