// Host test: g++ -o /tmp/t src/core/d2common/d2common_resetstkoflw_8a1d.test.cpp && /tmp/t
#include "d2common_resetstkoflw_8a1d.cpp"
#include <cassert>
static uint32_t prot_n, prot_val, alloc_n;
static uintptr_t prot_at;
static uintptr_t q(uintptr_t, D2cRstMbi* m, uintptr_t) {
    m->allocation_base = 0x100000; m->base = 0x100000; m->region_size = 0x1000; m->state = 0x1000; m->protect = 4; return 1; }
static void si(D2cRstSystemInfo* i) { i->page_size = 0x1000; }
static uintptr_t va(uintptr_t, uintptr_t, uint32_t, uint32_t) { alloc_n++; return 1; }
static int32_t vp(uintptr_t a, uintptr_t, uint32_t p, uint32_t*) { prot_n++; prot_at = a; prot_val = p; return 1; }
int main() {
    D2cRstEnv e{q, si, va, vp};
    // platform 1: guard = page below slot, PAGE_NOACCESS, no alloc
    assert(d2common_resetstkoflw(0x120800, 1, &e) == 1 && prot_at == 0x11f000 && prot_val == 1 && !alloc_n);
    // platform != 1: commits one page, PAGE_READWRITE|PAGE_GUARD
    assert(d2common_resetstkoflw(0x120800, 2, &e) == 1 && alloc_n == 1 && prot_val == 0x104);
    // guard below the limit (17 pages above base) -> 0
    prot_n = 0;
    assert(d2common_resetstkoflw(0x110800, 1, &e) == 0 && prot_n == 0);
    return 0;
}
