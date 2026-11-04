// D2Net.dll MSVC 2003 CRT: small-block heap group/resize, multibyte codepage tables, lconv/time frees.
// 0x3347 0x3376 0x339F 0x353A 0x36CE 0x381E 0x3D84 0x4570 0x4B13 0x4CA3 0x4D02 had no cached
// decompile; written to the VS2003 CRT shape with Win32 services as Host seams, to be re-checked later.
#include <cstdint>
#include <cstring>

namespace d2net_n97 {

struct Host {
  int (*get_cp_info)(uint32_t cp, uint8_t cpinfo[20]);  // GetCPInfo: 1 ok; cpinfo = MaxCharSize(4), DefaultChar(2), LeadByte(12), 2 pad
  void (*reset_sbcs)();                               // 0x3376
  void (*build_casemap)();                            // 0x339F: the ctype/case tables from the new codepage
  uint32_t (*get_acp)();                              // GetACP
  uint32_t (*get_oemcp)();                            // GetOEMCP
  void (*lock)(int);
  void (*unlock)(int);
  void (*free_)(void*);                               // _free
  int (*is_default_ptr)(const void*);                 // pointer is a static __lconv_c / __lc_time_c default
};

struct State {
  uint8_t mbctype[257];     // _mbctype
  uint32_t mbcodepage, mbulinfo[6], mbcsflag;
  uint8_t sbcs_ext[256];
  uint32_t lang;            // 0xB454 language id of the codepage
  uint8_t tbl[0xf8];        // 0xA5C0: flag bytes (+0..3) and five 0x30-byte codepage entries from +8 (cp, 3 info dwords, four 8-byte lead-range groups)
  uint32_t sbcs_fallback;   // 0xB0D0: nonzero = an unknown codepage falls back to SBCS instead of failing
};

// 1.13c D2NET+0x03347
uint32_t getmbcp(const State& s) { return s.mbcodepage; }

// 1.13c D2NET+0x03376
void setSBCS(State& s) {
  std::memset(s.mbctype, 0, sizeof s.mbctype);
  s.mbcodepage = 0; s.mbcsflag = 0;
  std::memset(s.mbulinfo, 0, sizeof s.mbulinfo);
}

// 1.13c D2NET+0x0339F  (-3 ANSI, -4 OEM, -2 thread = ANSI; 0 = SBCS; else the codepage)
int getSystemCP(const Host& h, int cp) {
  if (cp == -2 || cp == -3) return (int)h.get_acp();
  if (cp == -4) return (int)h.get_oemcp();
  return cp;
}

// 1.13c D2NET+0x03347 (eax = codepage): the language id of the four East Asian codepages, else 0
uint32_t cpLangId(uint32_t cp) {
  switch (cp) { case 932: return 0x411; case 936: return 0x804; case 949: return 0x412; case 950: return 0x404; default: return 0; }
}

// 1.13c D2NET+0x0353A: the CRT codepage table builder (cdecl cp, 0 ok / -1 unknown codepage with no fallback).
// A codepage in the five-entry static table (0xA5C8 + 0x30 * i) takes its ranges from the table; any other asks GetCPInfo.
int buildMbctable(const Host& h, State& s, uint32_t cp) {
  auto sbcs = [&]() { h.reset_sbcs(); h.build_casemap(); return 0; };
  if (cp == 0) return sbcs();
  uint32_t i = 0;
  for (; i < 5; ++i) { uint32_t v; std::memcpy(&v, s.tbl + 8 + 0x30 * i, 4); if (v == cp) break; }
  if (i < 5) {
    std::memset(s.mbctype, 0, sizeof s.mbctype);
    const uint8_t* e = s.tbl + 8 + 0x30 * i;
    for (uint32_t g = 0; g < 4; ++g) {
      const uint8_t* p = e + 0x10 + 8 * g;
      for (uint8_t lo = *p; lo; lo = *p) {
        uint8_t hi = p[1];
        if (!hi) break;
        if (lo <= hi) for (uint32_t c = lo; c <= hi; ++c) s.mbctype[1 + c] |= s.tbl[g];
        p += 2;
      }
    }
    s.mbcodepage = cp; s.mbcsflag = 1;
    s.lang = cpLangId(cp);
    std::memcpy(s.mbulinfo, e + 4, 12);
    h.build_casemap();
    return 0;
  }
  uint8_t ci[20] = {};
  if (h.get_cp_info(cp, ci) != 1) return s.sbcs_fallback ? sbcs() : -1;
  std::memset(s.mbctype, 0, sizeof s.mbctype);
  s.mbcodepage = cp; s.lang = 0;
  uint32_t maxc; std::memcpy(&maxc, ci, 4);
  if (maxc > 1) {
    for (const uint8_t* p = ci + 7; p[-1] && p[0]; p += 2)
      for (uint32_t c = p[-1]; c <= p[0]; ++c) s.mbctype[1 + c] |= 4;
    for (uint32_t c = 1; c < 255; ++c) s.mbctype[1 + c] |= 8;
    s.lang = cpLangId(cp);
    s.mbcsflag = 1;
  } else s.mbcsflag = 0;
  std::memset(s.mbulinfo, 0, 12);
  h.build_casemap();
  return 0;
}

// 1.13c D2NET+0x036CE: __setmbcp, 0 on success, -1 on a codepage with no table
int setmbcp(const Host& h, State& s, int cp) {
  h.lock(0x19);
  int r = -1;
  int c = getSystemCP(h, cp);
  if (c == 0) { setSBCS(s); r = 0; }
  else if (buildMbctable(h, s, (uint32_t)c)) r = 0;
  h.unlock(0x19);
  return r;
}

// 1.13c D2NET+0x0381E: ___initmbctable, seeds the table from the ANSI codepage once
int initmbctable(const Host& h, State& s) { return s.mbcsflag ? 0 : setmbcp(h, s, -3); }

// 1.13c D2NET+0x03D84: ___sbh_resize_block; 1 when the block can change to newParas in place
int sbh_resize_block(uint32_t* sizeParas, uint32_t newParas) {
  if (newParas > 0x3F8) return 0;       // beyond the SBH ceiling
  if (newParas < *sizeParas) *sizeParas = newParas;
  else if (newParas > *sizeParas) return 0;  // ponytail: growth into the following free run not modelled
  return 1;
}

// 1.13c D2NET+0x04570: memcpy (backward-safe in the retail overlap path; memmove semantics here)
void* d2_memcpy(void* dst, const void* src, size_t n) { return std::memmove(dst, src, n); }

// 1.13c D2NET+0x04B13: ___free_lc_time, frees the 43 string pointers unless they are the C defaults
void free_lc_time(const Host& h, void** lc) {
  if (!lc) return;
  for (int i = 0; i < 43; ++i) if (!h.is_default_ptr(lc[i])) h.free_(lc[i]);
  h.free_(lc);
}

// 1.13c D2NET+0x04CA3: ___free_lconv_num, decimal_point, thousands_sep, grouping (slots 0,1,2)
void free_lconv_num(const Host& h, void** lc) {
  if (!lc) return;
  for (int i = 0; i < 3; ++i) if (!h.is_default_ptr(lc[i])) h.free_(lc[i]);
}

// 1.13c D2NET+0x04D02: ___free_lconv_mon, the monetary string slots 3..12
void free_lconv_mon(const Host& h, void** lc) {
  if (!lc) return;
  for (int i = 3; i < 13; ++i) if (!h.is_default_ptr(lc[i])) h.free_(lc[i]);
}

}  // namespace d2net_n97
