// Storm batch e6bd: CRT locale helpers, net-layer entry points, rect-list merge. Retail globals/callees are seams.
#include <cstdint>
#include <cstdlib>
#include <cstring>
typedef uint16_t wchar16;
#define A32(a) ((uintptr_t)(uint32_t)(a))
#define I8(a, o) (*(int8_t*)(A32(a) + (o)))
#define I16(a, o) (*(int16_t*)(A32(a) + (o)))
#define U16(a, o) (*(uint16_t*)(A32(a) + (o)))
#define U32(a, o) (*(uint32_t*)(A32(a) + (o)))
#define I32(a, o) (*(int32_t*)(A32(a) + (o)))

// ---- seams ----
extern "C" {
int Win_GetLocaleInfoW(uint32_t lcid, uint32_t type, wchar16* buf, int n);
int Win_GetLocaleInfoA(uint32_t lcid, uint32_t type, char* buf, int n);
uint32_t Win_GetLastError();
int Win_MultiByteToWideChar(uint32_t cp, uint32_t fl, const char* s, int n, wchar16* d, int dn);
int Win_WideCharToMultiByte(uint32_t cp, uint32_t fl, const wchar16* s, int n, char* d, int dn, const char* def, int* used);
void Win_SetLastError(uint32_t);
void Win_EnterCS(void* cs);
void Win_LeaveCS(void* cs);
void Win_SetEvent(uint32_t h);
int32_t Win_InterlockedIncrement(int32_t* p);
extern int32_t g_locW_6fc42904, g_locA_6fc42908, g_acp_6fc42788;       // locale probe state / default codepage
extern uint32_t g_lastErr_6fc45388;                                     // DAT_6fc45388
extern void* g_cs_6fc45a24;                                             // net CS
extern void* g_cs_6fc45940;                                             // rect CS
extern int32_t g_init_6fc45834, g_self_6fc41b4c, g_base_6fc45830, g_head_6fc45a6c;
extern uint32_t g_ver_6fc45344, g_event_6fc4584c;
extern int32_t g_cnt_6fc41a78, g_cnt_6fc41a98;
uint32_t* Storm_Sub12690();                                             // current net ctx
void Storm_Sub12F40(uint32_t, int8_t, uint32_t* data, uint32_t len);   // queue msg
uint32_t Storm_Sub10CB0();
uint32_t Storm_SubFEB0(int32_t id);                                    // find peer node
void Storm_Grow(uint32_t* list, uint32_t n);                           // FUN_6fbf2950 (ecx = list)
void* Storm_Sub1C580(uint32_t sz, const char* f, uint32_t line, uint32_t fl);
void Storm_Sub1B960(void* p, const char* f, uint32_t line);
void Storm_SubF2E0(void* a, void* b, void* c, void* d, void* e, void* f);
extern void *g_cb_6fc11c70, *g_cb_6fc11c30;
int Storm_Sub1FD50(float* r);
void Storm_Sub20A90(uint32_t a, uint32_t id, uint32_t fl);
void Storm_Sub20C90(uint32_t* list, uint32_t, uint32_t, uint32_t, float* r, uint32_t a, uint32_t id);
void Storm_Sub1FEA0(int);
void Storm_Sub207F0();
uint32_t* Storm_Sub2690(uint32_t ecx, uint32_t tbl);
void Storm_Qsort(uint32_t base, uint32_t n, uint32_t sz, int (*cmp)(const void*, const void*));
extern int (*g_cmp_6fc1fcf0)(const void*, const void*);
}

static int fail(uint32_t e) { g_lastErr_6fc45388 = e; Win_SetLastError(e); return 0; }

// 1.13c Storm+0xE6BD: ___crtGetLocaleInfoW
int Storm_crtGetLocaleInfoW(uint32_t lcid, uint32_t type, wchar16* out, int n, int cp) {
  if (g_locW_6fc42904 == 0) {
    if (Win_GetLocaleInfoW(0, 1, 0, 0) == 0) { if (Win_GetLastError() == 0x78) g_locW_6fc42904 = 2; }
    else g_locW_6fc42904 = 1;
  }
  if (g_locW_6fc42904 == 1) return Win_GetLocaleInfoW(lcid, type, out, n);
  if (g_locW_6fc42904 == 2 || g_locW_6fc42904 == 0) {
    int ret = 0; bool heap = false;
    if (cp == 0) cp = g_acp_6fc42788;
    int sz = Win_GetLocaleInfoA(lcid, type, 0, 0);
    if (sz != 0) {
      char stk[0x400]; char* buf = stk;
      if ((uint32_t)sz >= 0x400) { buf = (char*)malloc(sz); if (!buf) return 0; heap = true; }
      if (Win_GetLocaleInfoA(lcid, type, buf, sz) != 0)
        ret = Win_MultiByteToWideChar(cp, 1, buf, -1, n ? out : 0, n ? n : 0);
      if (heap) free(buf);
      return ret;
    }
  }
  return 0;
}

// 1.13c Storm+0xE7ED: ___crtGetLocaleInfoA
int Storm_crtGetLocaleInfoA(uint32_t lcid, uint32_t type, char* out, int n, int cp) {
  if (g_locA_6fc42908 == 0) {
    if (Win_GetLocaleInfoW(0, 1, 0, 0) == 0) { if (Win_GetLastError() == 0x78) g_locA_6fc42908 = 2; }
    else g_locA_6fc42908 = 1;
  }
  if (g_locA_6fc42908 == 2 || g_locA_6fc42908 == 0) return Win_GetLocaleInfoA(lcid, type, out, n);
  if (g_locA_6fc42908 == 1) {
    int ret = 0; bool heap = false;
    if (cp == 0) cp = g_acp_6fc42788;
    int sz = Win_GetLocaleInfoW(lcid, type, 0, 0);
    if (sz != 0) {
      wchar16 stk[0x200]; wchar16* buf = stk;
      if ((uint32_t)sz * 2 >= 0x400) { buf = (wchar16*)malloc(sz * 2); if (!buf) return 0; heap = true; }
      if (Win_GetLocaleInfoW(lcid, type, buf, sz) != 0)
        ret = Win_WideCharToMultiByte(cp, 0, buf, -1, n ? out : 0, n ? n : 0, 0, 0);
      if (heap) free(buf);
      return ret;
    }
  }
  return 0;
}

// walk the 32-bit peer list: node+4 = next
#define PEERS(n) for (int32_t n = g_head_6fc45a6c; 0 < n; ) { 
#define PEERS_END(n) n = I32(n, 4); if (!(0 < n && n != 0)) break; }

// 1.13c Storm+0x12AF0
uint32_t Storm_Sub12AF0(uint32_t* out) {
  if (!out) return fail(0x57);
  Win_EnterCS(g_cs_6fc45a24);
  *out = 0;
  uint32_t e;
  if (g_init_6fc45834 == 0) e = 0x4b4;
  else if (g_self_6fc41b4c == -1) e = 0x85100070;
  else {
    uint32_t* c = Storm_Sub12690();
    if (c) { *out = (uint16_t)((int16_t)c[0x74] - (int16_t)c[0x76]); Win_LeaveCS(g_cs_6fc45a24); return 1; }
    e = 8;
  }
  fail(e); Win_LeaveCS(g_cs_6fc45a24); return 0;
}

// 1.13c Storm+0x135D0
uint32_t Storm_Sub135D0(uint32_t v) {
  Win_EnterCS(g_cs_6fc45a24);
  if (g_init_6fc45834 == 0) fail(0x57);
  else if (g_self_6fc41b4c == -1) fail(0x85100070);
  else {
    uint32_t* c = Storm_Sub12690();
    if (c && c[0x80] != 0) {
      if (v == g_ver_6fc45344) { Win_LeaveCS(g_cs_6fc45a24); return 1; }
      g_ver_6fc45344 = v;
      PEERS(n) if (I8(n, 0x212) != -1) Storm_Sub12F40(0, 0x0e, &g_ver_6fc45344, 4); PEERS_END(n)
      if (Storm_Sub10CB0()) { Win_LeaveCS(g_cs_6fc45a24); return 1; }
    } else fail(0x120);
  }
  Win_LeaveCS(g_cs_6fc45a24); return 0;
}

// 1.13c Storm+0x136C0
uint32_t Storm_Sub136C0(uint32_t* data, uint32_t len) {
  if (!data || len == 0) return fail(0x57);
  Win_EnterCS(g_cs_6fc45a24);
  if (g_init_6fc45834 == 0) fail(0x4b4);
  else if (g_self_6fc41b4c == -1) fail(0x85100070);
  else {
    uint32_t* c = Storm_Sub12690();
    if (!c) { fail(0x85100070); Win_LeaveCS(g_cs_6fc45a24); return 0; }
    PEERS(n) if (I8(n, 0x212) != -1 && I16(n, 0x1d0) == (int16_t)c[0x74]) Storm_Sub12F40(2, 0, data, len); PEERS_END(n)
    Storm_Sub12F40(2, 0, data, len);
    Win_InterlockedIncrement(&g_cnt_6fc41a78);
    Win_LeaveCS(g_cs_6fc45a24);
    Win_SetEvent(g_event_6fc4584c);
    return 1;
  }
  Win_LeaveCS(g_cs_6fc45a24); return 0;
}

// 1.13c Storm+0x137F0
uint32_t Storm_Sub137F0(int32_t id, uint32_t* data, uint32_t len) {
  if (!data || len == 0) return fail(0x57);
  Win_EnterCS(g_cs_6fc45a24);
  if (g_init_6fc45834 == 0) { fail(0x4b4); Win_LeaveCS(g_cs_6fc45a24); return 0; }
  if (g_self_6fc41b4c == -1) { fail(0x85100070); Win_LeaveCS(g_cs_6fc45a24); return 0; }
  bool broadcast = (id == -2);
  if (!broadcast) {
    if (id != -1) id -= g_base_6fc45830;
    broadcast = (id == -2 || id == -1);
  }
  if (broadcast) {
    PEERS(n) if (I8(n, 0x212) != -1) Storm_Sub12F40(1, 0, data, len); PEERS_END(n)
    if (id == -1 && Storm_Sub12690()) Storm_Sub12F40(1, 0, data, len);
  } else {
    if (Storm_SubFEB0(id) == 0) { fail(0x8510006a); Win_LeaveCS(g_cs_6fc45a24); return 0; }
    Storm_Sub12F40(1, 0, data, len);
  }
  Win_InterlockedIncrement(&g_cnt_6fc41a98);
  Win_LeaveCS(g_cs_6fc45a24);
  Win_SetEvent(g_event_6fc4584c);
  return 1;
}

// 1.13c Storm+0x13CB0
uint32_t Storm_Sub13CB0(uint32_t id, uint32_t val) {
  Win_EnterCS(g_cs_6fc45a24);
  if (g_init_6fc45834 == 0) { fail(0x4b4); Win_LeaveCS(g_cs_6fc45a24); return 0; }
  if (id == 0xff || id == (uint32_t)g_self_6fc41b4c) fail(0x57);
  else if ((uint32_t)g_self_6fc41b4c == 0xff) fail(0x85100070);
  else {
    uint32_t p = Storm_SubFEB0(id);
    if (p == 0) { fail(0x8510006a); Win_LeaveCS(g_cs_6fc45a24); return 0; }
    U32(p, 0x118) |= 8;
    U16(p, 0x210) = U16(p, 0x1d8);
    U32(p, 0x20c) = val;
    PEERS(n) if ((uint32_t)n != p && I8(n, 0x212) != -1) {
      uint32_t m[3] = {id, U16(p, 0x1d8), val};
      Storm_Sub12F40(0, 0x0c, m, 0xc);
    } PEERS_END(n)
    Win_LeaveCS(g_cs_6fc45a24); return 1;
  }
  Win_LeaveCS(g_cs_6fc45a24); return 0;
}

// 1.13c Storm+0x22B40 (fastcall: ecx = param_1, edx = param_2)
void Storm_Sub22B40(uint32_t p1, uint32_t* p2, uint32_t p3, uint32_t p4, int32_t* p5) {
  void* h = Storm_Sub1C580(0x8dd8, "..\\3rdParty\\STORM\\SOURCE\\SCOMP.CPP", 0x3f1, 0);
  uint32_t a[8];  // local_18.. local_4 : {p1, 0(out), p2[0], p3, 0, p4}
  uint32_t flag = (*p5 == 2);
  uint32_t lim = p4 < 0xc00 ? ((p4 < 0x600 ? 0xfffffc00u : 0u) + 0x800) : 0x1000;
  a[0] = p1; a[1] = 0; a[2] = *p2; a[3] = p3; a[4] = 0; a[5] = p4;
  Storm_SubF2E0(g_cb_6fc11c70, g_cb_6fc11c30, h, a, &flag, &lim);
  Storm_Sub1B960(h, "..\\3rdParty\\STORM\\SOURCE\\SCOMP.CPP", 0x40f);
  *p2 = a[1];
  *p5 = 0;
}

// ---- rect list: {cap, count, data(32-bit ptr, 16 bytes/rect), align} ----
#define RF(list, i) ((float*)(A32((list)[2]) + (size_t)(i) * 16))
static const uint32_t FMAX = 0x7f7fffff;
static void clr(float* r) { uint32_t* u = (uint32_t*)r; u[0] = u[1] = u[2] = u[3] = FMAX; }
static void reserve1(uint32_t* l) {
  uint32_t need = l[1] + 1;
  if (l[0] < need) {
    uint32_t al = l[3];
    if (al == 0) {
      if (need < 0x10) { al = need; for (uint32_t u = l[1] & need; u; u = (u - 1) & u) al = u; if (!al) al = 1; }
      else { l[3] = 0x10; al = 0x10; }
    }
    if (need % al) need += al - need % al;
    Storm_Grow(l, need);
  }
}
static void push(uint32_t* l, const float* r) {
  reserve1(l);
  float* d = RF(l, l[1]); l[1]++;
  d[0] = r[0]; d[1] = r[1]; d[2] = r[2]; d[3] = r[3];
}

// 1.13c Storm+0x31150: add rect, recursively clipping against rects [from,to)
void Storm_Sub31150(uint32_t* l, uint32_t from, uint32_t to, float* r) {
  for (uint32_t i = from; i < to; i++) {
    float* q = RF(l, i);
    if (r[0] < q[2] && r[1] < q[3] && q[0] < r[2] && q[1] < r[3]) {
      // r minus q: top, bottom, left, right strips (piece order inferred; the decompile dropped the stack fill)
      float lo = r[1] > q[1] ? r[1] : q[1], hi = r[3] < q[3] ? r[3] : q[3];
      float s[4][4] = {{r[0], r[1], r[2], q[1]}, {r[0], q[3], r[2], r[3]}, {r[0], lo, q[0], hi}, {q[2], lo, r[2], hi}};
      for (int k = 0; k < 4; k++)
        if (s[k][0] < s[k][2] && s[k][1] < s[k][3]) Storm_Sub31150(l, i + 1, to, s[k]);
      return;
    }
  }
  push(l, r);
}

// 1.13c Storm+0x31350: coalesce/split overlapping rects (see merge rules below)
void Storm_Sub31350(uint32_t* l) {
  for (uint32_t i = 1; i < l[1]; i++) {
    for (uint32_t j = 0; j < i; j++) {
      float* p = RF(l, j); float* c = RF(l, i);
      if (c[0] == p[0] && c[2] == p[2] && (c[1] == p[3] || p[1] == c[3])) {
        c[1] = p[1] <= c[1] ? p[1] : c[1];
        c[3] = c[3] <= p[3] ? p[3] : c[3];
        clr(p); break;
      }
      if (c[0] != p[2] && p[0] != c[2]) continue;
      if (c[1] == p[1] && c[3] == p[3]) {
        c[0] = p[0] <= c[0] ? p[0] : c[0];
        c[2] = c[2] <= p[2] ? p[2] : c[2];
        clr(p); break;
      }
      if (p[3] <= c[1] || c[3] <= p[1]) continue;
      float s[20] = {c[0], c[1], c[2], p[1],  p[0], p[1], p[2], c[1],  c[0], p[3], c[2], c[3],
                     p[0], c[3], p[2], p[3],
                     p[0] <= c[0] ? p[0] : c[0], c[1] <= p[1] ? p[1] : c[1],
                     c[2] <= p[2] ? p[2] : c[2], p[3] <= c[3] ? p[3] : c[3]};
      // s[0] (x0 of first piece) is not assigned in the decompile; cur.x0 by symmetry with piece 3
      for (int k = 0; k < 5; k++)
        if (s[4 * k] < s[4 * k + 2] && s[4 * k + 1] < s[4 * k + 3]) push(l, &s[4 * k]);
      clr(RF(l, i)); clr(RF(l, j));
      break;
    }
  }
}

// 1.13c Storm+0x316D0
void Storm_Sub316D0(int32_t a, float* r, uint32_t b, int32_t mode) {
  if (!(a != 0 && r != 0 && mode >= 1 && mode <= 6)) { g_lastErr_6fc45388 = 0x57; Win_SetLastError(0x57); return; }
  Win_EnterCS(g_cs_6fc45940);
  uint32_t* c = Storm_Sub2690(0, 0x6fc458f0);
  if (c) {
    if (mode == 2 || mode == 6) {
      if (r[0] < r[2] && r[1] < r[3]) { uint32_t id = c[0x16]++; Storm_Sub20A90(b, id + 1, mode != 6 ? 0x10000 : 0); }
    } else {
      if (Storm_Sub1FD50(r) == 0) { uint32_t id = c[0x16]++; Storm_Sub20C90(c + 6, 0, c[7], 0, r, b, id + 1); }
      Storm_Sub1FEA0(mode);
      Storm_Sub207F0();
    }
    c[0x17] = 1; c[0x12] = c[0x13] = c[0x14] = c[0x15] = FMAX;
  }
  Win_LeaveCS(g_cs_6fc45940);
}

// 1.13c Storm+0x317F0: owner = struct with count@+0x1c, rects@+0x20 (28 B each, flags@+0x18), list@+0x28
void Storm_Sub317F0(uint32_t owner) {
  uint32_t* l = (uint32_t*)(A32(owner) + 0x28);
  uint32_t n = U32(owner, 0x1c);
  U32(owner, 0x2c) = 0;
  float* r = (float*)A32(U32(owner, 0x20));
  for (; n; n--, r += 7)
    if ((((uint32_t*)r)[6] & 0x10000) == 0) Storm_Sub31150(l, 0, U32(owner, 0x2c), r);
  Storm_Sub31350(l);
  Storm_Qsort(U32(owner, 0x30), U32(owner, 0x2c), 0x10, g_cmp_6fc1fcf0);
  uint32_t cnt = U32(owner, 0x2c);
  while (cnt) {
    uint32_t u = cnt - 1;
    float* q = RF(l, u);
    if (q[0] < q[2] && q[1] < q[3]) break;
    if (U32(owner, 0x2c) < u && l[0] < u) {
      uint32_t al = U32(owner, 0x34);
      if (al == 0) {
        if (u < 0x10) { al = u; for (uint32_t v = (cnt - 2) & u; v; v = (v - 1) & v) al = v; if (!al) al = 1; }
        else { U32(owner, 0x34) = 0x10; al = 0x10; }
      }
      Storm_Grow(l, u % al ? u + al - u % al : u);
    }
    U32(owner, 0x2c) = u;
    cnt = u;
  }
}
