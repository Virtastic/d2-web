// src/core/storm/storm_batch_b145.cpp -- Storm batch 145 (decompile-derived; no spec/vector yet).
// Retail 32-bit addresses stay uint32_t. Callees not yet mapped here are reached through hooks (null = unlinked).
// Retail rdata tables (DAT_6fc35554 etc.) are not in git: the statics below are zero until filled; unverified.
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cctype>

static uint8_t g_regCode[16];          // DAT_6fc35554 (reg id -> modrm bits)
static uint8_t g_regCode2[32];         // DAT_6fc3555f (indexed [reg*2+sub])
static uint8_t g_opTab[64];            // UNK_6fc35524 (opcode pairs, stride 2 per (kind*3+mode))
static uint32_t g_dat4535c;            // DAT_6fc4535c
static const char kRegCh[] = "?wdstabc01";      // s_?wdstabc01
static const char* g_opNames[16];      // PTR_s_move_6fc354f8 (mnemonic strings)
static const char* g_str688c = "0FFh"; // DAT_6fc3688c (unverified text)
static char g_decimalPoint = '.';      // DAT_6fc406b4
static double g_pow10Tab[21 * 10];     // DAT_6fc42918
static uint32_t g_lastError;           // DAT_6fc45388
static int32_t g_purecall;             // _ptiddata->_purecall of the single wasm thread

// hooks for unmapped callees
void (*Storm_Hook_249E0)(uint32_t) = nullptr;          // FUN_6fc249e0
void (*Storm_Hook_24BA0)(uint32_t) = nullptr;          // FUN_6fc24ba0
void (*Storm_Hook_1FB30)(const char*, const char*) = nullptr; // FUN_6fc1fb30
void (*Storm_Hook_2A330)(void*, void*, void*, uint32_t, uint32_t, int32_t*) = nullptr; // FUN_6fc2a330
void (*Storm_Hook_B3A0)() = nullptr;                   // FUN_6fc0b3a0
void (*Storm_Hook_BD00)() = nullptr;                   // FUN_6fc0bd00
void (*Storm_Hook_1FAB0)() = nullptr;                  // FUN_6fc1fab0
int (*Storm_Hook_CrtStartup)(uint32_t, int, int) = nullptr; // __DllMainCRTStartup@12
static uint32_t g_listHead[4];         // DAT_6fc42fc0 (pointers to nodes whose first dword is arg of 24BA0)

// 1.13c Storm+0x35170: shutdown; drain 4 handle lists (critical sections are no-ops, single thread)
extern "C" void Storm_Sub35170(uint32_t ecx) {
  if (Storm_Hook_249E0) Storm_Hook_249E0(ecx);
  for (int i = 0; i < 4; i++)
    while (g_listHead[i]) {
      uint32_t node = g_listHead[i];
      if (Storm_Hook_24BA0) Storm_Hook_24BA0(node);  // retail reads *node; node table is 32-bit guest memory
      if (Storm_Hook_1FB30) Storm_Hook_1FB30("HSLOG", nullptr);
      if (g_listHead[i] == node) break;  // ponytail: retail relies on 24BA0 unlinking; avoid spin when unhooked
    }
}

// 1.13c Storm+0x38220: p[0]=blk (out of 2A330), p[1]=data, p[2]=end; truncation: ints are retail 32-bit
extern "C" void Storm_Sub38220(int32_t* p, uint32_t eax) {
  if (Storm_Hook_2A330) Storm_Hook_2A330(nullptr, (void*)(uintptr_t)eax, nullptr, 0xa0, 0, p);
  int32_t blk = p[0];
  if (blk == 0) { g_lastError = 0x57; return; }  // SetLastError(0x57) not modelled
  p[1] = blk + 0x4c;
  p[2] = *(int32_t*)(uintptr_t)(blk + 0x48) - 4 + *(int32_t*)(uintptr_t)(blk + 0x44);  // 32-bit guest addr truncated
}

// 1.13c Storm+0x38b00: x86 instruction encoder. p = out buffer, o = operand record (32-bit fields)
extern "C" uint8_t* Storm_Sub38B00(uint8_t* out, uint8_t* o) {
  int32_t& kind = *(int32_t*)(o + 0x10); int32_t sz = *(int32_t*)(o + 0x14);
  if (kind == 4 && o[0xc] == 0) {
    out[0] = (uint8_t)(((sz < 2) - 1U & 0x82) - 1);
    out[1] = g_regCode[o[8]] | 0xc0;
    if (sz < 2) return out + 2;
    memcpy(out + 2, &sz, 4);
    return out + 6;
  }
  if (kind == 0 && o[8] == 8 && o[0xf] == 0 && o[0xe] == 0) { kind = 3; *(uint32_t*)(o + 8) = g_dat4535c; }
  char c = (char)o[0xc]; int m = 0;
  if (c == 8 || c == 9 || c == 0) m = 2; else if (o[0xb] || o[10]) m = 1;
  int idx = (m + kind * 3) * 2;
  uint8_t b2 = g_opTab[idx];
  if (sz == 1) b2 &= 0xfe;
  uint8_t loc = 0; bool ext = false;
  uint8_t *p6, *p8;
  if (m == 0) { p6 = o + 8; p8 = o + 0xc; } else { p8 = o + 8; p6 = o + 0xc; }
  uint8_t b7, b4;
  bool direct = false;
  if (p8[3] == 0) direct = true;
  else if (p8[2] == 0 && (p8[0] == 3 || p8[0] == 2)) { b7 = g_regCode[p8[0]]; goto L8d; }
  if (direct) {
    if (p8[2] == 0) {
      b7 = (p8[1] == 0) ? (g_regCode[p8[0]] | 0xc0) : (g_regCode2[p8[0] * 2 + p8[1]] | 0xc0);
      goto L8d;
    }
  }
  b7 = 4; ext = true;
  if (p8[0] == 4) { if (p8[2] == 0) b7 = 0x45; else loc = g_regCode[p8[2]] | 0x28; }
  else loc = (uint8_t)(g_regCode[p8[2]] << 3 | g_regCode[p8[0]]);
L8d:
  if (m == 2 || g_opTab[idx + 1] != 0) b4 = (uint8_t)(g_opTab[idx + 1] << 3);
  else if (p6[1] == 0) b4 = (uint8_t)(g_regCode[p6[0]] << 3);
  else b4 = (uint8_t)(g_regCode2[p6[0] * 2 + p6[1]] << 3);
  if (sz == 2) *out++ = 0x66;
  out[0] = b2; out[1] = b7 | b4;
  uint8_t* r = out + 2;
  if (ext) *r++ = loc;
  if (kind != 7) {
    c = (char)o[0xc];
    if (c == 8 || c == 9) { uint32_t v = (c != 9) - 1; memcpy(r, &v, 4); r += sz; }
    return r;
  }
  *r = 0x10;
  return r + 1;
}

// 1.13c Storm+0x38d50: disassembly text emitter
extern "C" char* Storm_Sub38D50(char* out, uint8_t* o) {
  int32_t kind = *(int32_t*)(o + 0x10), sz = *(int32_t*)(o + 0x14);
  if (kind == 4 && o[0xc] == 0) {
    int n = sprintf(out, "inc  %c,%u\n", kRegCh[o[8]], (unsigned)sz);
    return out + n;
  }
  const char* nm = g_opNames[kind] ? g_opNames[kind] : "";
  strcpy(out, nm); char* p = out + strlen(nm);
  for (int i = 1; i >= 0; i--) {
    uint8_t* q = i ? o + 8 : o + 0xc;
    if (*q == 0) continue;
    if (i == 0) *p++ = ',';
    if (q[3] || q[2]) *p++ = '[';
    if (*q == 9) {
      const char* s = sz == 1 ? g_str688c : sz == 2 ? "0FFFFh" : sz == 4 ? "0FFFFFFFFh" : "";
      strcpy(p, s); p += strlen(s);
    } else *p++ = kRegCh[*q];
    if (sz < 4 && q[3] == 0 && q[2] == 0) *p++ = (sz == 2) ? 'x' : (char)('h' + (q[1] == 1) * 4);
    if (q[2]) { *p++ = '+'; *p++ = kRegCh[q[2]]; }
    if (q[3] || q[2]) *p++ = ']';
  }
  *p++ = '\n';
  return p;
}

// 1.13c Storm+0x3bb50: CRT float-conversion power table, [exp -1..-20][digit 0..9]
extern "C" void Storm_Sub3BB50() {
  double* d = g_pow10Tab;
  for (int32_t e = -1; e > -0x15; e--)
    for (int32_t k = 0; k < 10; k++) {
      int32_t base = 10, r = 1; uint32_t u = (uint32_t)-e;
      for (;;) {
        if (u & 1) r = (int32_t)((uint32_t)r * (uint32_t)base);
        u >>= 1; if (!u) break;
        base = (int32_t)((uint32_t)base * (uint32_t)base);
      }
      r = (int32_t)(1 / (int64_t)r);  // e<0 always
      *d++ = (double)r * (double)k;
    }
}

// 1.13c Storm+0x3c470: DllMain wrapper; critical-section init is a no-op on wasm
extern "C" int Storm_Sub3C470(uint32_t h, int reason, int rsv) {
  if (reason == 1 && Storm_Hook_B3A0) Storm_Hook_B3A0();
  int r = Storm_Hook_CrtStartup ? Storm_Hook_CrtStartup(h, reason, rsv) : 0;
  if (reason == 0 || (reason == 1 && r == 0)) {
    if (Storm_Hook_BD00) Storm_Hook_BD00();
    if (Storm_Hook_1FAB0) Storm_Hook_1FAB0();
    Storm_Sub35170(0);  // retail passes stale ECX (extraout_ECX)
  }
  return r;
}

// 1.13c Storm+0x3c8b1: CRT __fptrap table init (6 code pointers at 6fc40b58..6c)
uint32_t g_fptrap[6];
extern "C" void Storm_Sub3C8B1() {
  // targets: __cfltcvt, 6fc2d5d8, __fassign, 6fc2d59c, 6fc2d623, __cfltcvt; guest addresses
  static const uint32_t t[6] = {0, 0x6fc2d5d8, 0, 0x6fc2d59c, 0x6fc2d623, 0};  // __cfltcvt/__fassign addrs unknown
  memcpy(g_fptrap, t, sizeof t);
}

// 1.13c Storm+0x3cdb8: --ptd->_purecall if > 0
extern "C" void Storm_Sub3CDB8() { if (g_purecall > 0) g_purecall--; }

// 1.13c Storm+0x3cf8c: C++ EH catch-epilogue fragment (unaff_EBP/ESI/EDI frame). Frame values passed explicitly.
struct Storm3CF8CFrame {
  uint32_t* edi4;        // *(EDI-4) destination
  uint32_t saved38, saved40, saved44;  // EBP-0x38/-0x40/-0x44
  int32_t* exc;          // ESI exception record (32-bit fields): [0]=code [4]=nparams [5]=magic [6]=obj
  int32_t flag48, flag20;
};
static uint32_t g_unexpected, g_translator;  // ptd->_unexpected / _translator
void (*Storm_Hook_DestructExc)(int32_t*) = nullptr;  // __abnormal_termination + ___DestructExceptionObject
int (*Storm_Hook_IsDestroyed)(int32_t) = nullptr;
extern "C" void Storm_Sub3CF8C(Storm3CF8CFrame* f) {
  *f->edi4 = f->saved38;
  // _FindAndUnlinkFrame(EBP-0x3c) not modelled: no SEH frame chain on wasm
  g_unexpected = f->saved40; g_translator = f->saved44;
  int32_t* e = f->exc;
  if (e[0] == -0x1f928c9d && e[4] == 3 && (e[5] == 0x19930520 || e[5] == 0x19930521) && f->flag48 == 0 && f->flag20 != 0) {
    if (Storm_Hook_IsDestroyed && Storm_Hook_IsDestroyed(e[6]) && Storm_Hook_DestructExc) Storm_Hook_DestructExc(e);
  }
}

// 1.13c Storm+0x3d59c: CRT _cropzeros-style: insert decimal point before exponent scan result
extern "C" void Storm_Sub3D59C(char* p) {
  bool stop = tolower((unsigned char)*p) == 'e';
  while (!stop) { p++; stop = !isdigit((unsigned char)*p); }
  char c2 = *p; *p = g_decimalPoint;
  do { p++; char c1 = *p; *p = c2; c2 = c1; } while (*p != 0);
}

// 1.13c Storm+0x3d746 / 0x3d852: _cftoe / _cftof; callees (3f3b9 _fltout2, __fptostr, __cftoe2/__cftof2) hooked
struct StormFltOut { int32_t sign; int32_t decpt; int32_t pad[2]; };
void (*Storm_Hook_Fltout)(uint32_t lo, uint32_t hi, StormFltOut*, uint32_t* buf) = nullptr;           // 6fc2f3b9
void (*Storm_Hook_Fptostr)(char* dst, int32_t digits, StormFltOut*) = nullptr;                        // __fptostr
void (*Storm_Hook_Cftoe2)(char* p, int32_t prec, int32_t caps, char) = nullptr;                        // __cftoe2
void (*Storm_Hook_Cftof2)(char* p, int32_t prec, char) = nullptr;                                      // __cftof2
extern "C" char* Storm_Sub3D746(uint32_t* dbl, char* buf, int32_t prec, int32_t caps) {
  StormFltOut fo{}; uint32_t tmp[6];
  if (Storm_Hook_Fltout) Storm_Hook_Fltout(dbl[0], dbl[1], &fo, tmp);
  if (Storm_Hook_Fptostr) Storm_Hook_Fptostr(buf + (prec > 0) + (fo.sign == 0x2d), prec + 1, &fo);
  if (Storm_Hook_Cftoe2) Storm_Hook_Cftoe2(buf, prec, caps, 0);
  return buf;
}
extern "C" char* Storm_Sub3D852(uint32_t* dbl, char* buf, size_t prec) {
  StormFltOut fo{}; uint32_t tmp[6];
  if (Storm_Hook_Fltout) Storm_Hook_Fltout(dbl[0], dbl[1], &fo, tmp);
  if (Storm_Hook_Fptostr) Storm_Hook_Fptostr(buf + (fo.sign == 0x2d), fo.decpt + (int32_t)prec, &fo);
  if (Storm_Hook_Cftof2) Storm_Hook_Cftof2(buf, (int32_t)prec, 0);
  return buf;
}
