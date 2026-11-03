// 1.13c D2Common+0x547D0 -- build the item-type pools: for every ItemTypes row whose flag byte (+0x1D) is set, and for
// every level step 3, 6 .. 96, create one named record "<type name><level>" and link into it every item (ItemsTxt row,
// stride 0x1A8) of that type whose level byte (+0xFD) lies in (step-3, step]. Seams: the named-record lookup (0x53780,
// name in EAX), the name table lookup (Storm #10214), the name formatter (Storm #578), the item-type predicate (0x24500),
// the template lookup (0x1300, id in EAX), the object allocator (0x54020) and the linker (0x53650, record in ECX).
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2Common547D0Env {
    uint8_t*       dt;           // sgptDataTables (DAT +0x99E1C): +0xBF8 ItemTypes rows (stride 0xE4), +0xBFC count
    uint8_t*       items;        // DAT +0x9FB98: ItemsTxt rows, stride 0x1A8
    int32_t        itemCount;    // DAT +0x9FB94
    uint32_t       handle;       // DAT +0xA0ACC: the name table handle
    uint16_t*      counter;      // DAT +0xA1460
    const char*    fmt;          // module literal at +0x926D0
    const char*    anchor;       // module literal at +0x8A728, the first lookup's name
    uint8_t*     (*find)(const char* name);                                           // 0x53780
    int32_t      (*lookup)(uint32_t handle, int32_t index, char* out8);               // Storm #10214
    void         (*format)(char* buf, uint32_t size, const char* fmt, const char* name, int32_t level); // Storm #578
    int32_t      (*cond)(int32_t item, int32_t type);                                 // 0x24500
    uint8_t*     (*tmpl)(int32_t id);                                                 // 0x1300
    uint8_t*     (*alloc)();                                                          // 0x54020
    void         (*link)(uint8_t* rec, uint8_t* obj, int32_t weight, int32_t flag);   // 0x53650
    void         (*fatal)(uint32_t code);                                             // line 0x1EF report, then _exit
};

static inline int32_t  rd32(const uint8_t* p) { int32_t v; std::memcpy(&v, p, 4); return v; }
static inline uint16_t rd16(const uint8_t* p) { uint16_t v; std::memcpy(&v, p, 2); return v; }
static inline void     wr32(uint8_t* p, int32_t v) { std::memcpy(p, &v, 4); }
static inline void     wr16(uint8_t* p, uint16_t v) { std::memcpy(p, &v, 2); }

extern "C" void D2Common_547D0_BuildPools(const D2Common547D0Env* e)
{
    e->find(e->anchor);
    const int32_t nTypes = rd32(e->dt + 0xBFC);
    if (nTypes <= 0) return;
    int32_t idx = 0, off = 0;
    for (;;) {
        if (off < 0 || idx >= nTypes) return;
        uint32_t base; std::memcpy(&base, e->dt + 0xBF8, 4);
        const uint8_t* row = (const uint8_t*)(uintptr_t)(base + (uint32_t)off);
        if (base + (uint32_t)off == 0) return;
        if (row[0x1D] != 0) {
            wr16((uint8_t*)e->counter, (uint16_t)(rd16((const uint8_t*)e->counter) + 1));
            for (int32_t lv = 3; lv <= 0x60; lv += 3) {
                char name[8]; char buf[32];
                if (!e->lookup(e->handle, idx, name)) continue;
                e->format(buf, 0x20, e->fmt, name, lv);
                uint8_t* rec = e->find(buf);
                if (!rec) continue;
                wr32(rec + 0x10, 1);
                wr16(rec, 0);
                wr16(rec + 2, (uint16_t)(lv - 3));
                for (int32_t i = 0; i < e->itemCount; ++i) {
                    if (!e->items) { e->fatal(0x1EF); return; }
                    const uint8_t* it = e->items + (size_t)i * 0x1A8;
                    if (it[0x12A] != 0 || it[0x133] == 0) continue;
                    if (!e->cond(i, idx)) continue;
                    if (off != 0x21D8 && e->cond(i, 0x26) != 0) continue;
                    const int32_t lvl = it[0xFD];
                    if (lvl <= lv - 3 || lvl > lv) continue;
                    const uint8_t* t = e->tmpl((int16_t)rd16(it + 0x11E));
                    if (!t) continue;
                    int32_t w = t[0x1E]; if (w <= 1) w = 1;
                    uint8_t* obj = e->alloc();
                    wr16(obj + 8, (uint16_t)i);
                    const bool big = rd16(it + 0xF6) >= 100;
                    if (big) obj[0xC] |= 0x10;
                    e->link(rec, obj, w, big ? 1 : 0);
                }
            }
        }
        const int32_t n = rd32(e->dt + 0xBFC);
        ++idx; off += 0xE4;
        if (!(idx < n)) return;
    }
}
