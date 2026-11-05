// 1.13c D2Common+0x36030 (#11003): allocate the unit's dynamic path (unit+0x2C), seed position, room and size, pick
// the path type defaults by unit type, register in the room and derive the half extents. Stdcall, six stack words
// (ret 0x18): memory pool (handed to the Fog 10045 allocator in ECX, with EDX = 0x200, the Path.cpp file string and
// line 0x453), room, x, y, unit, flag. Retail does not check the allocation. Every callee is a seam and every address
// is a 32-bit (i686 layout) address with raw offsets, so the FN probe can drive it on hook blobs.
#include <cstdint>
#include <cstring>

struct D2Common36030Env {
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line, int32_t zero); // Fog 10045 (thunk 0x9222)
    uint32_t (*f30F80)(uint32_t unit);                       // unit size class
    uint32_t (*f35360)(uint32_t unit, uint32_t size);
    void     (*f34F80)(uint32_t path, int32_t type);
    uint32_t (*f35100)(uint32_t unit);                       // EAX = unit: monster path type
    void     (*f359D0)(uint32_t unit);
    void     (*f66800)(uint32_t unit, uint32_t room, int32_t one);
    void     (*fatal)(uint32_t line);                        // 10265 + 10024 + exit in retail
    uint32_t dataTables;                                     // sgptDataTables 0x99E1C
};

namespace {
uint32_t r32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void w32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
void w16(uint32_t a, uint16_t v) { std::memcpy((void*)(uintptr_t)a, &v, 2); }
void w8(uint32_t a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; }
}

extern "C" void D2Common_36030_AllocPath(const D2Common36030Env* e, uint32_t pool, uint32_t room, int32_t x, int32_t y,
                                         uint32_t unit, int32_t flag) // 1.13c D2Common+0x36030
{
    const uint32_t p = e->alloc(pool, 0x200, 0x453, 0);
    std::memset((void*)(uintptr_t)p, 0, 0x200);
    w32(unit + 0x2C, p);
    w32(p + 0x30, unit);
    const uint32_t sz = e->f30F80(unit);
    w32(p + 0x44, sz);
    w32(p + 0x48, e->f35360(unit, sz));
    w32(p, (uint32_t)x * 0x10000u + 0x8000u);
    w16(p + 0x1D8, (uint16_t)x);
    w32(p + 4, (uint32_t)y * 0x10000u + 0x8000u);
    w32(p + 0x7C, 0x800);
    w32(p + 0x1C, room);
    w8(p + 0x93, 0);
    w32(p + 0x1D4, 1);
    w16(p + 0x1DA, (uint16_t)y);
    const uint32_t type = r32(unit);
    if (type == 0) {
        w32(p + 0x4C, 0x80);
        w32(p + 0x50, 0x1C09);
        e->f34F80(p, 7);
        w8(p + 0x91, 0x49);
        w8(p + 0x92, 0x46);
    } else if (type == 1) {
        w32(p + 0x4C, 0x100);
        w32(p + 0x34, r32(p + 0x34) & 0xFFF800FFu);
        w32(p + 0x3C, 2);
        w32(p + 0x98, 0);
        if (r32(p + 0x40) == 0xB) { e->fatal(0x871); return; }
        if (r32(p + 0x40) == 8) { e->fatal(0x872); return; }
        bool special = false;
        int32_t cls = (int32_t)r32(unit + 4);
        if (cls >= 0) {
            const uint32_t dt = e->dataTables;
            const int32_t n = (int32_t)r32(dt + 0xA80);
            if (cls < n) {
                const uint32_t rec = (uint32_t)cls * 0x1A8u + r32(dt + 0xA78);
                if (rec) { int16_t c; std::memcpy(&c, (const void*)(uintptr_t)(rec + 2), 2); cls = c; }
            }
            special = cls >= 0 && cls < n && cls == 0x26;
        }
        if (special) {
            w32(r32(unit + 0x2C) + 0x48, 5);
            w32(p + 0x50, 0x804);
        } else {
            w32(p + 0x50, e->f35100(unit));
        }
        w8(p + 0x91, 0xE);
    } else if (type == 3) {
        w32(p + 0x4C, 0);
        w32(p + 0x50, 0);
        e->f34F80(p, 4);
    }
    if (room) {
        e->f359D0(unit);
        e->f66800(unit, r32(p + 0x1C), 1);
    }
    const uint32_t ax = r32(p) >> 11, ay = r32(p + 4) >> 11;
    w32(p + 8, (uint32_t)((int32_t)(ax - ay) >> 1));
    w32(p + 0xC, (uint32_t)((int32_t)(ay + ax) >> 2));
    if (flag) w32(p + 0x34, r32(p + 0x34) | 0x10);
}
