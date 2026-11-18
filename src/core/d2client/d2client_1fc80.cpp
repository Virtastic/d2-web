// 1.13c D2Client+0x1FC80 -- fastcall-style ECX = unit, one unused stack word (ret 4). A type-1 unit with a +0x14 data block has the
// dword at data+0x18 cleared.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>
struct C1fc80Unit { int32_t kind; uint8_t p0[0x10]; uint32_t data; };
static_assert(offsetof(C1fc80Unit, data) == 0x14, "unit data");
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1fc80(C1fc80Unit* u, int32_t)
{
    if (u && u->kind == 1 && u->data) *(uint32_t*)(uintptr_t)(u->data + 0x18) = 0;
}
