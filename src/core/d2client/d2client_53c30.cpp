// 1.13c D2Client+0x53C30 -- load a monster's palette-shift table on demand. fastcall(ECX = monster id). Quits when the record 0x1230(id) is missing or 0x537A0(id) already finds a table.
// Path "Data\Global\Monsters\" + rec[0x10] + rec[0x11] + "\Cof\palshift.dat" (a space byte counts as end of string, the string stops at the first NUL), opened by 0xD6A0.
// Then a zeroed 0x1008-byte node (Fog alloc 0xBF7A, ecx 0x1008, line 0xF4), file read into node+4 (0xD520, esi = handle, edi = 0x800), close (0xD670), remap 0x53B20(eax id, edx node),
// node[0] = id, node[0x1004] = old list head, head = node; then for every i below the 0x1690 count with i == id, slot table[i*8+4] = node (the count is re-read each pass).
#include <cstdint>
struct D2c53c30Env {
    const uint8_t* (*rec)(uint32_t id);
    uint32_t (*find)(uint32_t id);
    uint32_t (*open)(const char* path, uint32_t* hOut);
    uint32_t (*alloc)();
    void (*read)(uint32_t h, uint32_t dst);
    void (*close)(uint32_t h);
    void (*remap)(uint32_t id, uint32_t node);
    int32_t (*count)();
};
extern "C" void d2c_53c30r(uint32_t id, uint32_t* head, uint32_t table, const D2c53c30Env& e) {
    const uint8_t* rec = e.rec(id);
    if (!rec) return;
    if (e.find(id)) return;
    uint8_t c[4] = { rec[0x10] == 0x20 ? (uint8_t)0 : rec[0x10], rec[0x11] == 0x20 ? (uint8_t)0 : rec[0x11], 0, 0 };
    char path[64] = {};
    const char pre[] = "Data\\Global\\Monsters\\", suf[] = "\\Cof\\palshift.dat";
    uint32_t n = 0;
    for (const char* p = pre; *p; ++p) path[n++] = *p;
    for (uint32_t k = 0; k < 4 && c[k]; ++k) path[n++] = (char)c[k];
    for (const char* p = suf; *p; ++p) path[n++] = *p;
    uint32_t h = 0;
    if (!e.open(path, &h)) return;
    const uint32_t node = e.alloc();
    uint32_t* nd = (uint32_t*)(uintptr_t)node;
    for (int i = 0; i < 0x402; ++i) nd[i] = 0;
    e.read(h, node + 4);
    e.close(h);
    e.remap(id, node);
    nd[0] = id;
    nd[0x401] = *head;
    *head = node;
    for (int32_t i = 0; i < e.count(); ++i)
        if ((int32_t)nd[0] == i) *(uint32_t*)(uintptr_t)(table + 4 + (uint32_t)i * 8) = node;
}
