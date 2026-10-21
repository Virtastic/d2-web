// D2Common 1.13c batch: A* path search driver. Retail passes hidden register state (EAX/ESI) to the
// callees; here it is explicit. a = param_1 (u32 start, u32 end packed x|y<<16), t/h1/h2 = 7B220 hash state.
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) uint32_t d2_fn_d2common_7b2c0(const uint32_t* a);
__attribute__((weak)) void d2_fn_d2common_7ae20(uint32_t* rec, uint32_t* l1, uint32_t* l2, uint32_t* t);
__attribute__((weak)) intptr_t d2_fn_d2common_7b220(uint32_t* t, const int32_t* h1, const int32_t* h2);
__attribute__((weak)) uint32_t d2_fn_d2common_7b3c0(const uint16_t* p, const int32_t* ctx, int32_t cur, uint32_t tgt, intptr_t chk, intptr_t ev);
__attribute__((weak)) uint32_t d2_fn_d2common_7ab50(const uint16_t* node, uint32_t* t);

// 1.13c D2Common+0x7B630: seed the open list with the start node, expand until the target or exhaustion, return the best node's path.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_7b630(const uint32_t* a, uint32_t* t, const int32_t* h1, const int32_t* h2,
  intptr_t chk, intptr_t ev) {
 if (!d2_fn_d2common_7b2c0(a)) return 0;
 const uint32_t s = a[0], e = a[1];
 uint32_t l1[128] = {}, l2[128] = {}, rec[16] = {};
 int32_t dx = static_cast<int32_t>(s & 0xffff) - static_cast<int32_t>(e & 0xffff);
 if (dx < 0) dx = -dx;
 int32_t dy = static_cast<int32_t>(s >> 16) - static_cast<int32_t>(e >> 16);
 if (dy < 0) dy = -dy;
 const int16_t h = static_cast<int16_t>(dx < dy ? dx + dy * 2 : dy + dx * 2);
 rec[1] = s;
 reinterpret_cast<int16_t*>(rec)[4] = h;
 reinterpret_cast<int16_t*>(rec)[5] = h;
 reinterpret_cast<uint16_t*>(rec)[6] = 0;
 d2_fn_d2common_7ae20(rec, l1, l2, t);
 const uint16_t* best = nullptr;
 const uint16_t* n = reinterpret_cast<const uint16_t*>(d2_fn_d2common_7b220(t, h1, h2));
 if (!n) return 0;
 do {
  if (!best || static_cast<int16_t>(n[3]) < static_cast<int16_t>(best[3]) ||
      (n[3] == best[3] && static_cast<int16_t>(best[4]) + 5 < static_cast<int16_t>(n[4])))
   best = n;
 } while (n[3] != 0 && d2_fn_d2common_7b3c0(n, reinterpret_cast<const int32_t*>(a), reinterpret_cast<intptr_t>(l1), e, chk, ev) != 0 &&
          (n = reinterpret_cast<const uint16_t*>(d2_fn_d2common_7b220(t, h1, h2))) != nullptr);
 return best ? d2_fn_d2common_7ab50(best, t) : 0;
}
}
