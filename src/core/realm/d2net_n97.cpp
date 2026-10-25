// D2Net.dll MSVC 2003 CRT: small-block heap group/resize, multibyte codepage tables, lconv/time frees.
// 0x3347 0x3376 0x339F 0x353A 0x36CE 0x381E 0x3D84 0x4570 0x4B13 0x4CA3 0x4D02 had no cached
// decompile; written to the VS2003 CRT shape with Win32 services as Host seams, to be re-checked later.
#include <cstdint>
#include <cstring>

namespace d2net_n97 {

struct Host {
  int (*get_cp_info)(uint32_t cp, uint8_t lead[12]);  // GetCPInfo: 1 ok, lead = LeadByte pairs
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

// 1.13c D2NET+0x0353A: fill _mbctype lead (0x04) and trail (0x08) flags from GetCPInfo lead ranges
int buildMbctable(const Host& h, State& s, uint32_t cp) {
  uint8_t lead[12] = {};
  if (!h.get_cp_info(cp, lead)) return 0;
  std::memset(s.mbctype, 0, sizeof s.mbctype);
  for (int i = 0; i < 12 && lead[i] && lead[i + 1]; i += 2)
    for (int c = lead[i]; c <= lead[i + 1]; ++c) s.mbctype[c + 1] |= 4;
  for (int c = 1; c < 255; ++c) if (!(s.mbctype[c + 1] & 4)) s.mbctype[c + 1] |= 8 * (c > 0x3f);
  s.mbcodepage = cp; s.mbcsflag = 1;
  return 1;
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
