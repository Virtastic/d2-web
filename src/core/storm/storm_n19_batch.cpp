// src/core/storm/storm_n19_batch.cpp -- 12 1.13c Storm.dll rows transcribed from the cached decompiles
// (build-wasm/ghidra-storm/decomp/0x<RVA>.c). Implementation plus marker only (batch lane).
// Win32/CRT services and not-yet-ported Storm callees are host-seam arguments (function pointers);
// retail globals are explicit state in n19::S. Register-only args are explicit parameters.
#include <cstdint>
#include <cstring>
#include <cstdio>

namespace n19 {

struct S {
  uint32_t lastError;      // 0x6FC45388
  void* provider;          // 0x6FC45834 SNet provider table (offset 0x54 = callback)
  uint32_t providerCtx;    // 0x6FC45238
  uint32_t baseCount;      // 0x6FC45A58
  uint8_t* baseTable;      // 0x6FC45A5C (128-byte records)
  int32_t provId;          // 0x6FC41B4C
  int32_t idBias;          // 0x6FC45830
  int32_t connHead;        // 0x6FC45A6C
  int32_t pending;         // 0x6FC41A98
  int32_t smallList[2][3]; // 0x6FC45A3C / 0x6FC45A64: [?, ?, count at +8]
  int moduleScanned;       // 0x6FC45844
  void* hEvent;            // 0x6FC4584C
  // host seam
  void (*setLastError)(uint32_t);
  void (*enter)(); void (*leave)();
  void* (*alloc)(uint32_t n, const char* f, int line, int flags);
  void (*free_)(void* p, const char* f, int line);
  void (*setEvent)(void*);
  void (*freeConn)();                                    // 0x11630
  int (*findConn)(int id);                               // 0xFEB0
  void (*sendTo)(int, char, const void*, uint32_t);      // 0x12F40
  void* (*firstBroadcast)();                             // 0x12690
};

static void fail(S& s, uint32_t e) { s.lastError = e; s.setLastError(e); }

// 1.13c Storm+0x116A0
void sub_116A0(S& s) {
  for (int i = 0; i < 2; i++)
    while (0 < s.smallList[i == 0 ? 1 : 0][2]) s.freeConn();
}

// 1.13c Storm+0x11760
uint32_t sub_11760(S& s, uint32_t first, int count, int len, uint32_t a4, uint32_t a5) {
  if (count == 0 || len == 0) { fail(s, 0x57); return 0; }
  s.enter();
  if (!s.provider) { fail(s, 0x4b4); s.leave(); return 0; }
  uint32_t* arr = (uint32_t*)s.alloc(count * 4, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x14b7, 8);
  uint32_t end = first + count, off = first << 7;
  for (uint32_t* p = arr; first < end; first++, p++, off += 0x80)
    if (first < s.baseCount) *p = (uint32_t)(uintptr_t)(s.baseTable + off);
  typedef void (*Cb)(uint32_t, int, uint32_t*, int, uint32_t, uint32_t);
  Cb cb = *(Cb*)((uint8_t*)s.provider + 0x54);
  if (cb) cb(s.providerCtx, count, arr, len, a4, a5);
  s.free_(arr, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x14c8);
  s.leave();
  return 1;
}

// 1.13c Storm+0x13930
uint32_t sub_13930(S& s, int id, const void* data, uint32_t len, int (*connInit)(const void*, uint32_t), int (*bcast)(S&)) {
  (void)connInit;
  if (!data || !len) { fail(s, 0x57); return 0; }
  s.enter();
  if (!s.provider) { fail(s, 0x4b4); s.leave(); return 0; }
  if (s.provId == -1) { fail(s, 0x85100070); s.leave(); return 0; }
  bool all = (id == -2);
  if (!all) {
    if (id != -1) id -= s.idBias;
    all = (id == -2 || id == -1);
  }
  if (all) {
    for (int c = s.connHead; 0 < c;) {
      // *(char*)(c+0x212) != -1 : per-connection active flag, walked via host seam
      s.sendTo(c, 0, data, len);
      break;  // ponytail: host seam walks the chain itself
    }
    if (id == -1 && bcast && !bcast(s)) goto done;
    if (id != -1) goto done;
  } else if (!s.findConn(id)) {
    fail(s, 0x8510006a); s.leave(); return 0;
  }
  s.sendTo(1, 0, data, len);
done:
  s.pending++;
  s.leave();
  s.setEvent(s.hEvent);
  return 1;
}

// Enumerates *.snp next to the module. Win32 find/load and the list helpers are the seam.
struct Snp {
  bool (*dirList)(const char* pat, char* outName, bool first);     // FindFirst/NextFileA
  const char* (*moduleDir)();                                      // GetModuleFileNameA minus file
  void* (*load)(const char* path);                                 // LoadLibraryA
  void* (*proc)(void* mod, const char* name);                      // GetProcAddress
  void (*unload)(void* mod);                                       // FreeLibrary
  void* (*newNode)();                                              // 0x4FC0 on list 0x6FC45A48
  void (*addProvider)(void* node);                                 // sorted insert (0xFFE0 compare)
  uint8_t* (*parse)(const char* path);                             // 0x168B0
  void (*fillNode)(void* node, const char* path, uint32_t idx, uint32_t id, const char* n1, const char* n2, const uint32_t* caps);
};
// 1.13c Storm+0x16D70
void sub_16D70(S& s, Snp& h) {
  if (s.moduleScanned) return;
  s.moduleScanned = 1;
  char dir[260], pat[270], name[260], path[600];
  strncpy(dir, h.moduleDir(), 259); dir[259] = 0;
  snprintf(pat, sizeof pat, "%s*.snp", dir);
  for (bool first = true; h.dirList(pat, name, first); first = false) {
    snprintf(path, sizeof path, "%s%s", dir, name);
    uint8_t* blob = h.parse(path);
    if (!blob) continue;
    void* mod = h.load(path);
    if (!mod) { s.free_(blob, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x22d); continue; }
    typedef int (*Query)(uint32_t, uint32_t*, const char**, uint32_t*, uint32_t**);
    Query q = (Query)h.proc(mod, "SnpQuery");
    if (!q) { s.free_(blob, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x235); h.unload(mod); continue; }
    uint32_t idx = 0, id = 0, cap = 0; const char* n1 = 0; uint32_t* caps = 0;
    const char* n2 = 0;
    // the blob (resource string pair + caps record) is walked by 0x168B0; per-entry query loop:
    while (q(idx, &id, &n1, &cap, &caps) && caps && *caps >= 0x24 && !(caps[1] & 0x10000000)) {
      void* node = h.newNode();
      h.fillNode(node, path, idx, id, n1, n2, caps);
      h.addProvider(node);
      idx++; id = 0; n1 = 0; cap = 0; caps = 0;
    }
    h.unload(mod);
    s.free_(blob, "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP", 0x281);
  }
}

// DllMain wrapper: DLL_PROCESS_ATTACH inits 4 critical sections and CRT-pre-init; CRT startup result
// returned; on detach (or failed attach) runs the three teardown routines.
// 1.13c Storm+0x3C470
int entry(void* hinst, int reason, void* reserved, void (*initCs)(int), void (*preInit)(),
          int (*crtStartup)(void*, int, void*), void (*t1)(), void (*t2)(), void (*t3)()) {
  if (reason == 1) { for (int i = 0; i < 4; i++) initCs(i); preInit(); }
  int r = crtStartup(hinst, reason, reserved);
  if (reason == 0 || (reason == 1 && r == 0)) { t1(); t2(); t3(); }
  return r;
}

// CRT float-format seam (__fptostr / __cftof2 / __cftoe2 / FUN_3F3B9 digit conversion).
struct Flt {
  // 0x3F3B9: double -> {sign, decpt, digits...} record
  void (*convert)(uint32_t lo, uint32_t hi, int* rec /*[4]: sign,decpt,?,digits*/, uint32_t* buf);
  void (*fptostr)(char* dst, int ndigits, int* rec);
  void (*cftof2)(char* buf, int ndigits, char g);
  void (*cftoe2)(int ndigits, int caps, char g);
};

// 1.13c Storm+0x3D852
char* sub_3D852(Flt& f, const uint32_t* dbl, char* out, uint32_t digits) {
  int rec[4]; uint32_t buf[6];
  f.convert(dbl[0], dbl[1], rec, buf);
  f.fptostr(out + (rec[0] == 0x2d), (int)(rec[2] + digits), rec);
  f.cftof2(out, digits, 0);
  return out;
}

// 1.13c Storm+0x3D8B9
void sub_3D8B9(Flt& f, const uint32_t* dbl, char* out, uint32_t digits, int caps) {
  int rec[4]; uint32_t buf[6];
  f.convert(dbl[0], dbl[1], rec, buf);
  int e0 = rec[2] - 1;
  f.fptostr(out + (rec[0] == 0x2d), (int)digits, rec);
  int e = rec[2] - 1;
  if (e < -4 || (int)digits <= e) { f.cftoe2(digits, caps, 1); return; }
  char* p = out + (rec[0] == 0x2d);
  if (e0 < e) { while (*p++) {} p[-2] = 0; }  // trim the extra digit after a rounding carry
  f.cftof2(out, digits, 1);
}

// 1.13c Storm+0x3DA24
uint32_t sub_3DA24() { return 1; }

// SSE2 detect. cpuid is unavailable in wasm; leaf-1 EDX and the vendor string are arguments.
// Returns 0; writes the two CRT flags.
// 1.13c Storm+0x3DA59
uint32_t sub_3DA59(uint32_t leaf1Eax, uint32_t leaf1Edx, const char vendor[12], bool hasCpuid,
                   uint32_t* sse2Flag /*0x6FC46900*/, uint32_t* notAmdK8 /*0x6FC468FC*/) {
  uint32_t edx = hasCpuid ? leaf1Edx : 0, eax = hasCpuid ? leaf1Eax : 0;
  *sse2Flag = 0; *notAmdK8 = 0;
  if ((edx & 0x4000000) && sub_3DA24()) {
    *sse2Flag = 1;
    uint32_t fam = (eax >> 8) & 0xf;
    if (memcmp("AuthenticAMD", vendor, 12) != 0 ||
        ((~-(uint32_t)(fam != 0xf) & ((eax >> 20) & 0xff)) + fam) != 0xf)
      *notAmdK8 = 1;
  }
  return 0;
}

// 1.13c Storm+0x3F1FE
void sub_3F1FE(double* out, const char* str, double (*parse)(const char*)) { *out = parse(str); }

// 1.13c Storm+0x3F243
void sub_3F243(double* out, const char* str, double (*parse)(const char*)) { *out = parse(str); }

// double -> {sign char, decpt, ndigits, buf*}: __dtold, FUN_30E87 (digit gen, 17 digits), copy of
// the digit buffer through 0x83B0. The 17-digit generator is the seam (gen).
// 1.13c Storm+0x3F3B9
int* sub_3F3B9(uint32_t lo, uint32_t hi, int* rec, uint32_t* digitsBuf,
               int (*gen)(uint32_t lo, uint32_t hi, int ndig, int* sign, int* decpt, uint32_t* d)) {
  int sign = 0, decpt = 0;
  rec[2] = gen(lo, hi, 0x11, &sign, &decpt, digitsBuf);
  rec[0] = sign;
  rec[1] = decpt;
  rec[3] = (int)(intptr_t)digitsBuf;
  return rec;
}

}  // namespace n19
