// 1.13c Fog+0x142D0 -- map a code address to (1-based PE section index, offset in section).
#include <cstdint>
#include <cstring>

namespace {
struct Rd { const uint8_t* p; uint32_t u32(uint32_t o) const { uint32_t v; memcpy(&v, p + o, 4); return v; }
            uint16_t u16(uint32_t o) const { uint16_t v; memcpy(&v, p + o, 2); return v; } };
}

// image: module base (PE image in memory); modulePath: GetModuleFileName result ("unknown" if none).
// Outputs are zeroed first; section/offset are written only on a hit.
void fog_sub_142D0(uint32_t addr, const uint8_t* image, const char* modulePath, char* buf, uint32_t bufSize,
                   int32_t* section, int32_t* offset) {
    const char* name = modulePath && *modulePath ? modulePath : "unknown";
    if (bufSize) { strncpy(buf, name, bufSize - 1); buf[bufSize - 1] = 0; }
    *section = 0; *offset = 0;
    if (!image || !modulePath || !*modulePath) return;
    Rd r{image};
    if (r.u16(0) != 0x5A4D) return;
    uint32_t nt = r.u32(0x3C);
    if (!nt || r.u32(nt) != 0x4550) return;
    uint32_t n = r.u16(nt + 6);
    uint32_t rel = addr - (uint32_t)(uintptr_t)image;
    uint32_t sh = nt + 0x18 + r.u16(nt + 0x14);
    for (uint32_t i = 0; i < n; ++i, sh += 40) {
        uint32_t va = r.u32(sh + 12), sz = r.u32(sh + 8), raw = r.u32(sh + 16);
        if (raw > sz) sz = raw;
        if (va <= rel && rel <= sz + va) { *section = i + 1; *offset = rel - va; return; }
    }
}
