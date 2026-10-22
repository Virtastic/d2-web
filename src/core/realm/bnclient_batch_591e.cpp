// src/core/realm/bnclient_batch_591e.cpp -- 1.13c BNClient.dll __ValidateEH3RN (Stage A: implementation only).
// MSVC SEH3 registration-record validator. wasm has no SEH; this is a pure-data port: the TIB stack bounds,
// the 16-entry page cache (DAT_6ff3e608 count, DAT_6ff3e610[16] pages, DAT_6ff3e650 spin flag) and the
// VirtualQuery + PE-header section check (a Win32 seam) arrive as arguments. Addresses are 32-bit values.
#include <cstdint>
#include <emscripten.h>

struct bn_eh3_cache { int32_t count; uint32_t pages[16]; volatile int32_t busy; };

// 1.13c BNClient+0x0591E (553 B): returns 1 valid, 0 invalid, -1 (0xffffffff) unqueryable.
// scope: the record's +8 scope table (3 dwords per entry: prev level, filter, handler); trylevel: +0xC;
// saved_esp: dword at record-8; rec: record address. pe_check(page_addr): 1 = continue to cache insert,
// 0 = address inside a writable-or-executable image section (invalid), -1 = VirtualQuery failed/not image.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_bnclient_591e(uint32_t rec, uint32_t scope_addr, const uint32_t* scope,
                                                 uint32_t trylevel, uint32_t saved_esp,
                                                 uint32_t stack_base, uint32_t stack_limit,
                                                 bn_eh3_cache* c, int32_t (*pe_check)(uint32_t)) {
    if ((scope_addr & 3) != 0 || (stack_limit <= scope_addr && scope_addr < stack_base)) return 0;
    if (trylevel == 0xffffffffu) return 1;
    uint32_t n = 0, lvl = 0;
    do {
        const uint32_t* e = scope + lvl * 3;
        if (e[0] != 0xffffffffu && lvl <= e[0]) return 0;
        if (e[1] != 0) n++;
        lvl++;
    } while (lvl <= trylevel);
    if (n != 0 && (saved_esp < stack_limit || rec <= saved_esp)) return 0;

    uint32_t page = scope_addr & 0xfffff000u;
    auto rotate = [&](int32_t last) {   // shift pages[0..last] up, page into slot 0
        uint32_t p = page;
        for (int32_t i = 0; i <= last; i++) { uint32_t t = c->pages[i]; c->pages[i] = p; p = t; }
    };
    for (int32_t i = 0; i < c->count; i++) {
        if (c->pages[i] != page) continue;
        if (i < 1) return 1;
        if (__atomic_exchange_n(&c->busy, 1, __ATOMIC_SEQ_CST) != 0) return 1;
        int32_t k = i;
        if (c->pages[i] != page) {   // retail re-checks after taking the flag
            k = c->count - 1;
            while (k >= 0 && c->pages[k] != page) k--;
            if (k < 0) { if (c->count < 16) c->count++; k = c->count - 1; }
        }
        if (k != 0) rotate(k);
        __atomic_store_n(&c->busy, 0, __ATOMIC_SEQ_CST);
        return 1;
    }
    int32_t q = pe_check(page);
    if (q <= 0) return q;
    int32_t cnt = c->count;
    if (__atomic_exchange_n(&c->busy, 1, __ATOMIC_SEQ_CST) != 0) return 1;
    int32_t j = cnt;
    while (j > 0 && c->pages[j - 1] != page) j--;
    if (j == 0) {
        rotate(cnt < 16 ? (cnt < 15 ? cnt : 15) : 15);
        if (cnt < 16) c->count = cnt + 1;
    }
    __atomic_store_n(&c->busy, 0, __ATOMIC_SEQ_CST);
    return 1;
}
