// 1.13c D2Client+0x67F20 (stdcall this, a1..a9; ret 0x28; SEH frame). The list-window wrapper ctor:
// this[0] = the base vtable (D2Client+0xD5048), then the derived one (+0xD4FC8); mem = Storm SMemAlloc (thunk 0xBE66, Ordinal_401)
// (0x180776, the "new" file string at +0xD04F0, line -1, flags 0); this[2] = mem ? 0x67D00(mem, a1..a9) : 0 (one dword); returns this.
// Retail is 32-bit: the vtables and the file string are module addresses, passed in as words.
#include <cstdint>
#include <cstring>
struct D2c67f20Env {
    uint32_t (*alloc)(uint32_t size, uint32_t file, int32_t line, uint32_t flags);   // thunk 0xBE66
    uint32_t (*init)(uint32_t mem, const uint32_t a[9]);                             // D2Client+0x67D00 (stdcall, ret 0x28)
    uint32_t vtBase, vtList, fileNew;                                                // D2Client+0xD5048, +0xD4FC8, +0xD04F0
};
extern "C" uint8_t* d2c_port_67f20(uint8_t* self, const uint32_t a[9], const D2c67f20Env& e) {
    memcpy(self, &e.vtBase, 4);
    memcpy(self, &e.vtList, 4);
    const uint32_t mem = e.alloc(0x180776, e.fileNew, -1, 0);
    const uint32_t o = mem ? e.init(mem, a) : 0;
    memcpy(self + 8, &o, 4);
    return self;
}
