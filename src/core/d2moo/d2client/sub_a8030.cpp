// 1.13c D2Client+0xA8030 (stdcall, 1 stack arg = bucket array): for each of the 128 unit-list buckets walk the chain (next = unit+0xE4); per unit
// call 0x2B1E0 (ECX = unit), find the same-id unit in the global hash (D2Client+0x109A08, [type*0x80 + (id & 0x7f)], chain next +0xE4, key +0xC) and
// dispatch by type: type 1 calls 0x17F70 (EAX = hash unit), type 2 reads the data record 0xC226(unit[+4]) byte +0x1B4 (>= 0x13 is fatal) and, when
// nonzero, calls the handler at D2Client+0xF54E8[s] with ECX = hash unit. A type mismatch is fatal. Returns 0, or 1 on the fatal paths (retail exits).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
static uint32_t rd(uintptr_t a) { uint32_t v; std::memcpy(&v, (const void*)a, 4); return v; }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a8030(uintptr_t buckets, uintptr_t hash, uintptr_t tbl, void (*unit_2b1e0)(uintptr_t unit),
                                                         void (*unit_17f70)(uintptr_t unit), uintptr_t (*data_c226)(uint32_t id),
                                                         void (*handler)(uint32_t fn, uintptr_t unit)) {
    for (uint32_t i = 0; i < 0x80; i++) {
        for (uintptr_t u = rd(buckets + i * 4); u;) {
            uintptr_t next = rd(u + 0xE4);
            uint32_t id = rd(u + 0xC), type = rd(u);
            unit_2b1e0(u);
            for (uintptr_t p = rd(hash + (type * 0x80 + (id & 0x7f)) * 4); p; p = rd(p + 0xE4)) {
                if (rd(p + 0xC) != id) continue;
                uint32_t pt = rd(p);
                if (pt != type) return 1;
                if (pt == 1) unit_17f70(p);
                else if (pt == 2) {
                    uint8_t s = *(const uint8_t*)(data_c226(rd(p + 4)) + 0x1B4);
                    if (s >= 0x13) return 1;
                    if (s) handler(rd(tbl + s * 4), p);
                }
                break;
            }
            u = next;
        }
    }
    return 0;
}
