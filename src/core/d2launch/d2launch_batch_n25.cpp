// D2Launch batch n25. Externs are weak; hidden register inputs are leading params.
#include <cstdint>
#include <cstddef>

#define WK __attribute__((weak))
extern "C" {
WK extern uint32_t DAT_6fa65e5c, DAT_6fa65ad8, DAT_6fa656c4, DAT_6fa64d94, DAT_6fa654dc[7];
WK extern uint32_t DAT_6fa65ad4, DAT_6fa65eb0, DAT_6fa653b8, DAT_6fa65abc;
WK extern const uint16_t* _toUpperTable_exref;
WK int wsprintfA(char*, const char*, ...);
WK void q_d2launch_batch_n25_Ordinal_10128(const char*, const char*);
WK int Ordinal_10151(); WK int Ordinal_10227(); WK int Ordinal_10134(unsigned); WK void Ordinal_10019();
WK void Ordinal_10070(); WK void Ordinal_10131(void); WK void Ordinal_10053(int); WK void Ordinal_10097();
WK const char* BNGatewayAccess_Name(int, int);   // BNGatewayAccess::Name
WK void Ordinal_501(void*, const char*, unsigned); WK int32_t Ordinal_10003(); WK void Ordinal_10192(const void*, int);
WK void Unicode_toUnicode(void*, const char*, int);
WK void Unicode_strcpy(void*, const void*);
WK void Unicode_sprintf(int, void*, const void*, ...);
WK int Unicode_strlen(const void*);
WK void FUN_6fa521b0(int, unsigned); WK void FUN_6fa51aa0(char);
WK int FUN_6fa541b0(int); WK int FUN_6fa541f0(void*);
}

// 1.13c D2Launch+0x12BF0  (EAX = path string)
extern "C" uint32_t sub_12BF0(const char* in_eax, const char* name) {
  char a[260], b[260];
  DAT_6fa65e5c = 1;
  wsprintfA(a, "%s\\%s", "DATA\\GLOBAL");
  wsprintfA(b, "%s\\%s", "DATA\\GLOBAL", name);
  const char* k = "palette\\fechar\\pal.dat";  // 0x17 bytes incl. NUL
  int i = 0; while (i < 0x17 && in_eax[i] == k[i]) i++;
  DAT_6fa65ad8 = i == 0x17;
  q_d2launch_batch_n25_Ordinal_10128(a, b);
  return 1;
}

// 1.13c D2Launch+0x13D60
extern "C" void sub_13D60() {
  uint16_t name[0x100] = {}, fmt[0x100] = {}, out[0x100] = {};
  char narrow[0x100]; uint8_t tmp[244];
  int gw = ((int (*)())(uintptr_t)(*(uint32_t*)(DAT_6fa656c4 + 0x98)))();
  Ordinal_501(tmp, BNGatewayAccess_Name(gw, 0x100), 0);
  Unicode_toUnicode(name, narrow, 0x100);
  Unicode_strcpy(fmt, (const void*)(intptr_t)Ordinal_10003());
  Unicode_sprintf(0x100, out, fmt, name);
  for (int i = 0; i < Unicode_strlen(out); i++)
    if (out[i] < 0x100) out[i] = _toUpperTable_exref[out[i]];
  Ordinal_10192(out, 0);
}

// 1.13c D2Launch+0x141B0
extern "C" uint32_t sub_141B0(int p) { if (!Ordinal_10227()) return 0; FUN_6fa521b0(*(int*)(p + 0xc), 0); return 1; }
// 1.13c D2Launch+0x141F0
extern "C" uint32_t sub_141F0(int p) { if (!Ordinal_10227()) return 0; FUN_6fa521b0(*(int*)(p + 0xc), 0); return 1; }

// 1.13c D2Launch+0x14290
extern "C" uint32_t sub_14290(int* p) {
  int mode = Ordinal_10151();
  int n = Ordinal_10227() ? 7 : 5;
  for (int i = 0; i < n; i++) if (!Ordinal_10134(0)) return 0;
  int prev = (int)DAT_6fa64d94, sel = -1;
  DAT_6fa64d94 = (uint32_t)-1;
  if (prev != -1) { Ordinal_10019(); if (prev >= 0 && prev <= 6) Ordinal_10070(); }
  static const int map[7] = {4, 2, 0, 1, 3, 5, 6};
  for (int i = 0; i < 7; i++) if ((uint32_t)p[0] == DAT_6fa654dc[i]) {
    sel = map[i];
    if (i < 5) FUN_6fa521b0(p[3], 0); else if (i == 5) FUN_6fa541b0((int)(intptr_t)p); else FUN_6fa541f0((void*)p);
    break;
  }
  if (sel == prev || mode != 1) { if (mode == 2 || mode == 3 || mode == 4) DAT_6fa64d94 = sel; }
  else { if (sel >= 0 && sel <= 6) Ordinal_10070(); DAT_6fa64d94 = sel; }
  FUN_6fa51aa0((char)DAT_6fa64d94);
  if ((int)DAT_6fa64d94 == -1 && mode != 1) {
    for (int i = 0; i < 6; i++) Ordinal_10131();
    if (DAT_6fa65ad4 == 1) { Ordinal_10131(); Ordinal_10131(); }
    if (Ordinal_10227()) { Ordinal_10131(); Ordinal_10131(); Ordinal_10131(); }
    Ordinal_10053(0);
  } else {
    Ordinal_10131(); Ordinal_10131();
    if (DAT_6fa65eb0) { Ordinal_10131(); Ordinal_10131(); }
    if (DAT_6fa65ad4 == 1) { Ordinal_10131(); Ordinal_10131(); }
    Ordinal_10131();
    Ordinal_10053(DAT_6fa653b8);
    Ordinal_10131();
  }
  if (DAT_6fa65ad4 == 1) Ordinal_10131();
  if (Ordinal_10227()) {
    int s = (int)DAT_6fa64d94;
    if (s == 6 || s == 5) { Ordinal_10131(); Ordinal_10131(); Ordinal_10131(); }
    else if (s != -1) { Ordinal_10131(); Ordinal_10131(); Ordinal_10131(); Ordinal_10097(); }
    if (s != -1) *(uint8_t*)(DAT_6fa65abc + 0x1eb) |= 0x20;
  }
  if (DAT_6fa65ad4 == 1) { Ordinal_10097(); *(uint8_t*)(DAT_6fa65abc + 0x1eb) |= 0x40; }
  return 1;
}
