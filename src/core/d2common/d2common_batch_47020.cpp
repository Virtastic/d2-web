// D2Common 1.13c batch. Stat-list child allocation: find a reusable child of the owner list's
// +0x5c parent, else allocate a new one and link it in. Retail registers: this (ECX) and EAX both
// name a list; EAX is used only when this==0. Callees are seams (retail addresses as uint32_t).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 0x37F80 find child by owner id; 0x37F40 find child by filter (list in EAX); 0x38100 allocate
// (pool, filter); 0x3A7D0 link (list, child, 1); fatal = Ordinal_10265(0x201)/10024 then _exit(-1).
uint32_t (*g_d2common_fn_47020_find_id)(uint32_t, uint32_t) = nullptr;
uint32_t (*g_d2common_fn_47020_find_flt)(uint32_t list, uint32_t flt) = nullptr;
uint32_t (*g_d2common_fn_47020_alloc)(uint32_t, uint32_t) = nullptr;
void (*g_d2common_fn_47020_link)(uint32_t, uint32_t, int32_t) = nullptr;
uint32_t (*g_d2common_fn_47020_mem)(uint32_t addr) = nullptr;       // read u32 at retail address
void (*g_d2common_fn_47020_wr)(uint32_t addr, uint32_t v) = nullptr; // write u32
void (*g_d2common_fn_47020_fatal)() = nullptr;

// 1.13c D2Common+0x47020
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_47020(uint32_t pThis, uint32_t pEax, uint32_t id, uint32_t flt) {
 const uint32_t list = pThis ? pThis : pEax;
 const uint32_t par = g_d2common_fn_47020_mem(list + 0x5c);
 if (par && (int32_t)g_d2common_fn_47020_mem(par + 0x10) < 0) {
  const uint32_t r = id ? g_d2common_fn_47020_find_id(par, id) : g_d2common_fn_47020_find_flt(par, flt);
  if (r) return r;
 }
 const uint32_t n = g_d2common_fn_47020_alloc(g_d2common_fn_47020_mem(list + 8), flt);
 g_d2common_fn_47020_link(list, n, 1);
 if (n) { g_d2common_fn_47020_wr(n + 0x14, id); return n; }
 g_d2common_fn_47020_fatal();
 __builtin_trap(); // retail: _exit(-1)
}
}
