// 1.13c D2Win rows 0xC590 0xB8B0 0xCA00 from cached decompiles.
// Skipped: 0x7B30 (Win32 drive scan), 0x693C/0x2A1F (SEH CRT), 0x3A6C (CRT write), 0x5CB8 (CRT heap).
// Hidden register inputs are explicit leading params; callees/globals are weak seams.
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <initializer_list>

extern "C" {
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
__attribute__((weak)) uint32_t Ordinal_10102(void);
__attribute__((weak)) void Ordinal_10103(void);
__attribute__((weak)) void* Ordinal_10042(int line, int zero);
__attribute__((weak)) int32_t d2win_unicode2Win(char* dst, const uint16_t* src, int32_t n);
__attribute__((weak)) void d2win_win2Unicode(uint16_t* dst, const char* src, int32_t n);
__attribute__((weak)) uint8_t* (*d2win_glyph_lookup)(void);   // DAT_6f901508, returns glyph rec
__attribute__((weak)) uint32_t d2win_str_ellipsis;            // DAT_6f8fcedc (4 bytes)
__attribute__((weak)) uint32_t d2win_tbl_a[25];               // DAT_6f8ffd48
__attribute__((weak)) uint32_t d2win_tbl_b[64];               // DAT_6f8ffdb0
__attribute__((weak)) uint32_t d2win_tbl_b_count;             // DAT_6f8ffe00
__attribute__((weak)) int32_t* d2win_list_head;               // DAT_6f9a9e34
__attribute__((weak)) int32_t* sub_7C60(int, int32_t*);
}

static int32_t* g_head() { return d2win_list_head; }

// 1.13c D2Win+0x0c590  trim text to pixel width param_2; returns cut index
extern "C" int32_t sub_C590(uint16_t* text, int32_t width) {
  char buf[300]; int32_t cut = 0, i = 0; bool hit = false;
  d2win_unicode2Win(buf, text, 300);
  int32_t len = (int32_t)strlen(buf), at = 0;
  for (; i < len; i++) {
    int32_t w = 0, k = 0;
    for (const uint16_t* p = text; *p && k < i + 1; p++, k++)
      if (*p != 10) w += d2win_glyph_lookup()[3];
    if (width <= w) { hit = true; at = i; break; }
    at = i + 1;
  }
  cut = at;
  buf[cut] = 0;
  if (hit) { size_t l = strlen(buf); memcpy(buf + l, &d2win_str_ellipsis, 4); }
  d2win_win2Unicode(text, buf, 299);
  return cut;
}

// 1.13c D2Win+0xb8b0  build COF path; this=ecx(col), idx=esi, out=edi
extern "C" void d2win_exit(uint32_t code);
extern "C" void sub_B8B0(uint32_t col, uint32_t idx, char* out, uint32_t a, uint8_t) {
  uint32_t err;
  if (idx < 0x19) {
    if (col < d2win_tbl_b_count) {
      uint32_t u = d2win_tbl_a[idx], v = d2win_tbl_b[col];
      char x[4], y[4], z[4];
      auto f = [](char* d, uint32_t s) {
        for (int i = 0; i < 3; i++) { uint8_t c = (s >> (8 * i)) & 0xff; d[i] = c == 0x20 ? 0 : c; }
        d[3] = 0; };
      f(x, a); f(y, u); f(z, v);
      snprintf(out, 260, "%s\\%s\\COF\\%s%s%s.COF", idx > 6 ? "DATA\\GLOBAL\\MONSTERS" : "DATA\\GLOBAL\\CHARS", y, y, z, x);
      if (Ordinal_10102()) { Ordinal_10103(); return; }
      *out = 0; return;
    }
    err = 0x47e;
  } else err = 0x47d;
  Ordinal_10024("", Ordinal_10265(err));
  d2win_exit(UINT32_MAX);  // _exit(-1)
}

// 1.13c D2Win+0xca00  find/create refcounted node keyed (ebx,param_1,edi)
extern "C" int32_t* sub_CA00(int32_t key2, int32_t key1, int32_t key3) {
  int32_t* p = g_head();
  for (; p; p = (int32_t*)(intptr_t)p[6])
    if (p[1] == key1 && p[2] == key2 && p[3] == key3) { p[0]++; return p; }
  p = (int32_t*)Ordinal_10042(0x42c, 0);
  for (int i = 0; i < 8; i++) p[i] = 0;
  p[1] = key1; p[2] = key2; p[0]++; p[3] = key3;
  p[6] = (int32_t)(intptr_t)d2win_list_head; p[7] = 0;
  if (d2win_list_head) d2win_list_head[7] = (int32_t)(intptr_t)p;
  d2win_list_head = p;
  int32_t* r = sub_7C60(0, p + 4);
  uint32_t e = !r ? 0x3e1 : 0x3ea;
  if (r && ((uint8_t*)r)[3] == 0x14) { p[5] = (int32_t)(intptr_t)r; return p; }
  Ordinal_10024("", Ordinal_10265(e));
  d2win_exit(UINT32_MAX);  // _exit(-1), does not return
  return p;
}
