// D2Common 1.13c batch. Retail globals are passed in: t51a8 = DAT_6fde51a8 (int table),
// tA/tB = DAT_6fde5158/DAT_6fde50d8 (walker 0 tables), tC/tD = DAT_6fde5118/DAT_6fde5098 (walker 1).
// Walker state is 210 words: [0]=cursor [1]=count [2]=start pt [3]=cur pt [7]=match idx [8]=done
// [9..]=path pts (x=low16, y=high16). Callees are ports of 0x186D0 (advance) / 0x18400 (finish).
#include <cstdint>
#include <emscripten.h>

extern "C" {
uint32_t d2_fn_d2common_186d0(uint32_t* w, const int32_t* t5, const int32_t* t6);
int32_t d2_fn_d2common_18400(uint32_t* w, int32_t* buf, int32_t* idx);

// 1.13c D2Common+0x18830: bidirectional path walk, copies the shorter path into buf.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_18830(const uint32_t* p1, uint32_t p2, int32_t* buf, int32_t* idx, int32_t* end,
                                                  int32_t cap, int32_t dir, const int32_t* t51a8,
                                                  const int32_t* tA, const int32_t* tB, const int32_t* tC, const int32_t* tD) {
 uint32_t W[2][210] = {};
 const int32_t* T5[2] = {tA, tC};
 const int32_t* T6[2] = {tB, tD};
 const uint32_t u2 = p2 >> 16, u4 = p2 & 0xffff;
 const uint32_t v = static_cast<uint32_t>(buf[*idx]);
 const int32_t base = t51a8[(((v >> 16) - u2) * 3 - u4) + (v & 0xffff)];
 W[0][0] = base + 1; W[1][0] = base + 7;
 W[0][2] = W[1][2] = p2;
 uint32_t* A = W[0];  // this
 uint32_t* B = W[1];  // piVar12
 uint32_t* cur = A;   // local_69c
 int ai = 0, bi = 1;
 for (;;) {
  uint32_t* w = A; uint32_t* o = B;
  if (w[8] == 0) {
   if (!d2_fn_d2common_186d0(w, T5[ai], T6[ai])) w[8] = 1;
   else {
    if (static_cast<int32_t>(w[1]) > 0) {
     int32_t d; bool ok = true;
     const int32_t cy = w[3] >> 16, cx = w[3] & 0xffff;
     switch (dir) {
      case 0: d = u2 - cy; break;
      case 1: d = cx - u4; break;
      case 2: d = cy - u2; break;
      case 3: d = u4 - cx; break;
      default: ok = false; d = 0;
     }
     if (ok && d > 0) {
      d = d - 1 + *idx;
      if (d < *end && static_cast<uint16_t>(w[3]) == (static_cast<uint32_t>(buf[d]) & 0xffff) &&
          (w[3] >> 16) == (static_cast<uint32_t>(buf[d]) >> 16)) {
       if (cap <= static_cast<int32_t>(w[1])) __builtin_trap();  // retail: error report + _exit(-1)
       w[9 + w[1]] = w[3];
       reinterpret_cast<uint16_t*>(&w[10 + w[1]])[0] = 0;
       w[1]++;
       d2_fn_d2common_18400(w, buf, idx);
       return static_cast<int32_t>(cur[1]);
      }
     }
    }
    if (static_cast<int32_t>(o[1]) > 1) {
     const int32_t m = static_cast<int32_t>(w[7]);
     if (m != 0) {
      if (static_cast<uint16_t>(w[3]) == static_cast<uint16_t>(o[m + 7]) && (w[3] >> 16) == (o[m + 7] >> 16)) {
       buf[*idx] &= ~0xffff; *end = *idx; return 0;
      }
      w[7] = 0;
     }
     if (static_cast<uint16_t>(w[3]) == static_cast<uint16_t>(o[2]) && (w[3] >> 16) == (o[2] >> 16)) w[7] = o[1];
    }
    w[9 + w[1]] = w[3];
    const int32_t c = static_cast<int32_t>(w[1]);
    w[2] = w[3];
    w[0] = static_cast<uint32_t>(T5[ai][w[0]]);
    w[1] = c + 1;
    if (cap - *end - 1 <= c + 1) w[8] = 1;
   }
  }
  uint32_t *nA = A, *nB = B; int nai = ai, nbi = bi;
  if (A[7] == 0) { nA = B; nB = A; nai = bi; nbi = ai; cur = B; }
  A = nA; B = nB; ai = nai; bi = nbi;
  if (W[0][8] != 0 || W[1][8] != 0) break;
 }
 if (cap > 0x50) { buf[*idx] &= ~0xffff; *end = *idx; return 0; }
 const uint32_t ref = p1[1];
 auto dist = [&](uint32_t pt) {
  int32_t dx = (pt & 0xffff) - (ref & 0xffff), dy = (pt >> 16) - (ref >> 16);
  return dy * dy + dx * dx;
 };
 const int32_t n0 = static_cast<int32_t>(W[0][1]), n1 = static_cast<int32_t>(W[1][1]);
 const int32_t d0 = dist(W[0][8 + n0]), d1 = dist(W[1][8 + n1]), ds = dist(p1[0]);
 int32_t n; const uint32_t* src;
 if (d0 < d1) { if (ds < d0) return 0; n = n0; src = &W[0][9]; }
 else { if (ds < d1) return 0; n = n1; src = &W[1][9]; }
 for (int32_t i = 0; i < n; ++i) buf[*idx + i] = static_cast<int32_t>(src[i]);
 const int32_t i0 = *idx;
 *idx = i0 + n; *end = i0 + n;
 buf[*idx] &= ~0xffff;
 return static_cast<int32_t>(A[1]);
}
}
