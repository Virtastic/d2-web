// D2Common 1.13c batch. Retail: n = EAX (node), ctx = EDI (+0 root node, flag at +0x36fd0, seed ptr
// at +0x36fd4), tbl = DAT_6fddb960 (32 ints). Node: +6 count (i16), +0xc src ptr, +0x10 acc, +0x14 parent.
#include <cstdint>
#include <emscripten.h>

struct D2Node70840 { uint8_t p0[6]; int16_t cnt; int32_t p8; const int32_t* src; uint32_t acc; D2Node70840* parent; };

extern "C" {
void d2_fn_d2common_707e0(int32_t flag, uint32_t* seed, const int32_t* tbl, const int32_t* src, uint32_t* acc);

// 1.13c D2Common+0x70840: bump node count; on reaching 5 carry into the parent chain. Returns node or 0 at root.
EMSCRIPTEN_KEEPALIVE D2Node70840* d2_fn_d2common_70840(D2Node70840* n, D2Node70840* const* root, int32_t flag, uint32_t* seed, const int32_t* tbl) {
 if (n->cnt < 4) {
  n->src += 1;
  d2_fn_d2common_707e0(flag, seed, tbl, n->src, &n->acc);
 }
 n->cnt++;
 while (n->cnt == 5) {
  if (n == *root) return 0;
  n = n->parent;
  n->src += 1;
  d2_fn_d2common_707e0(flag, seed, tbl, n->src, &n->acc);
  n->cnt++;
 }
 return n;
}
}
