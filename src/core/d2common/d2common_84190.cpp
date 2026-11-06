// 1.13c D2Common+0x84190: run the preset-room workers of one room index.
// Records are 0x15C bytes at table + map[index]*0x15C; while a record's first dword equals the index the
// worker 0x83CA0 (this, record) then 0x84110 (unit, record) run and the next record is considered.
#include <cstdint>
#include <cstring>

struct D2Common84190Seams { void (*pfnFatal)(uint32_t); void (*pfnWorker)(uintptr_t, uintptr_t); void (*pfnRoom)(uintptr_t, uintptr_t); };

extern "C" void D2Common_84190_Run(uintptr_t unit, uint32_t table, uint32_t map, const D2Common84190Seams* s) {
    int32_t index; std::memcpy(&index, (const void*)(unit + 0x18), 4);
    if (index < 0) { s->pfnFatal(0x402); return; }
    if (!table) { s->pfnFatal(0x403); return; }
    if (!map) { s->pfnFatal(0x404); return; }
    int32_t m; std::memcpy(&m, (const void*)((uintptr_t)map + (uint32_t)index * 4), 4);
    uint32_t g, b, self;
    std::memcpy(&g, (const void*)unit, 4);
    std::memcpy(&b, (const void*)((uintptr_t)g + 0x1B4), 4);
    std::memcpy(&self, (const void*)((uintptr_t)b + 0x464), 4);
    uintptr_t rec = (uintptr_t)table + (uint32_t)m * 0x15C;
    for (;;) {
        int32_t first; std::memcpy(&first, (const void*)rec, 4);
        if (first != index) break;
        s->pfnWorker(self, rec);
        s->pfnRoom(unit, rec);
        rec += 0x15C;
    }
}
