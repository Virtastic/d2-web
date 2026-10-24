#include <cstdio>
// D2Net CRT leftovers (n116): small-block-heap free, code-page conversion, string-type/map wrappers,
// security error handler, locale ANSI code page. Win32 calls are hooks on one struct. Shape from the
// MSVC 2003 CRT, checked against the 1.13c disassembly of each routine.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>

namespace d2net_n116 {

struct Hooks {
  int (*GetCPInfoMax)(unsigned cp) = nullptr;                                   // MaxCharSize, 0 = failed
  int (*MBToWC)(unsigned cp, unsigned fl, const char*, int, uint16_t*, int) = nullptr;   // 0x6fbf80dc
  int (*WCToMB)(unsigned cp, unsigned fl, const uint16_t*, int, char*, int, const char*, int*) = nullptr; // 0x6fbf8100
  int (*GetStringTypeW)(unsigned ty, const uint16_t*, int, uint16_t*) = nullptr;         // 0x6fbf80e0
  int (*GetStringTypeA)(unsigned lcid, unsigned ty, const char*, int, uint16_t*) = nullptr; // 0x6fbf80d8
  int (*LCMapW)(unsigned lcid, unsigned fl, const uint16_t*, int, uint16_t*, int) = nullptr;
  int (*LCMapA)(unsigned lcid, unsigned fl, const char*, int, char*, int) = nullptr;
  int (*GetLocaleInfoA)(unsigned lcid, unsigned ty, char*, int) = nullptr;       // 0x6fbf8188
  int (*GetLastError)() = nullptr;                                               // 0x6fbf8138
  void (*VirtualOp)(void* addr, size_t sz, int decommit) = nullptr;              // VirtualFree 0x4000 / 0x8000
  void (*HeapFreeHdr)(void*) = nullptr;
  const char* (*ModuleName)() = nullptr;                                         // GetModuleFileNameA or null
  void (*MsgBox)(const char* text, const char* caption, unsigned fl) = nullptr;
  void (*Exit)(int) = nullptr;
  void (*Handler)(int, int) = nullptr;                                           // DAT_6fbfb16c
  int stype_mode = 0;                                                            // DAT_6fbfb140: 0 unknown 1 W 2 A
  unsigned lc_codepage = 0, lc_handle = 0;                                       // DAT_6fbfb138 / 128
  void* (*calloc_)(size_t, size_t) = nullptr;
  void (*free_)(void*) = nullptr;
};
inline Hooks g;

// ---- ___sbh_free_block (D2Net+0x038AF)
struct ListHead { void* prev; void* next; };
struct Group { int cnt; ListHead heads[64]; };
struct Region { int use; char cnt[64]; uint32_t hi[32], lo[32]; Group grp[32]; };
struct Header { uint32_t bitHi, bitLo, commit; char* data; Region* reg; };
struct Entry { int size; Entry* next; Entry* prev; };
inline Header* sbh_hdrs; inline int sbh_count, sbh_cap; inline Header* sbh_cached; inline int sbh_cached_grp;

static inline void bit_clear(Header* h, Region* r, int grp, int idx) {
  uint32_t m = ~(0x80000000u >> (idx & 31));
  (idx < 32 ? r->hi : r->lo)[grp] &= m;
  if (--r->cnt[idx] == 0) (idx < 32 ? h->bitHi : h->bitLo) &= m;
}
static inline void bit_link(Header* h, Region* r, int grp, int idx, Entry* e) {
  ListHead* lh = &r->grp[grp].heads[idx];
  e->next = (Entry*)lh; e->prev = (Entry*)lh->prev;  // insert at list head
  lh->prev = e; ((Entry*)e->prev)->next = e;
  if (e->prev == e->next) {                          // list was empty
    uint8_t was = (uint8_t)r->cnt[idx]++;
    uint32_t m = 0x80000000u >> (idx & 31);
    if (!was) (idx < 32 ? h->bitHi : h->bitLo) |= m;
    (idx < 32 ? r->hi : r->lo)[grp] |= m;
  }
}
static inline void unlink_entry(Header* h, Region* r, int grp, Entry* e, int idx) {
  if (e->prev == e->next) bit_clear(h, r, grp, idx);
  ((Entry*)e->next)->prev = e->prev; ((Entry*)e->prev)->next = e->next;
}

// 1.13c D2NET+0x038AF
void sbh_free_block(Header* h, void* p) {
  Region* r = h->reg;
  int grp = (int)(((char*)p - h->data) >> 15);
  Group* gp = &r->grp[grp];
  Entry* e = (Entry*)((char*)p - 4);
  int sz = e->size - 1;
  if (sz & 1) return;                                    // already free
  Entry* nx = (Entry*)((char*)e + sz);
  int nsz = nx->size, psz = ((int*)e)[-1];
  if (!(nsz & 1)) {                                      // coalesce with next
    int i = (nsz >> 4) - 1; if (i > 63) i = 63;
    unlink_entry(h, r, grp, nx, i);
    sz += nsz;
  }
  int i = (sz >> 4) - 1; if (i > 63) i = 63;
  if (!(psz & 1)) {                                      // coalesce with previous
    int pi = (psz >> 4) - 1; if (pi > 63) pi = 63;
    e = (Entry*)((char*)e - psz);
    sz += psz; i = (sz >> 4) - 1; if (i > 63) i = 63;
    if (pi != i) unlink_entry(h, r, grp, e, pi);
    else goto fixup;                                     // same bucket: leave it linked
  }
  bit_link(h, r, grp, i, e);
fixup:
  e->size = sz; *(int*)((char*)e + sz - 4) = sz;
  if (--gp->cnt != 0) return;
  // group empty: decommit it; release region when all groups are
  if (sbh_cached) {
    Region* cr = sbh_cached->reg;
    g.VirtualOp(sbh_cached->data + ((size_t)sbh_cached_grp << 15), 0x8000, 1);
    sbh_cached->commit |= 0x80000000u >> sbh_cached_grp;
    cr->hi[sbh_cached_grp] = 0;
    if (--cr->cnt[63] == 0) sbh_cached->bitHi &= ~1u;
    if (sbh_cached->commit == 0xFFFFFFFFu) {
      g.VirtualOp(sbh_cached->data, 0x100000, 0); g.HeapFreeHdr(cr);
      memmove(sbh_cached, sbh_cached + 1, (char*)(sbh_hdrs + sbh_count) - (char*)(sbh_cached + 1));
      --sbh_count;
      if (h > sbh_cached) --h;
      sbh_cap = sbh_count; sbh_cached = h;
    }
  }
  sbh_cached = h; sbh_cached_grp = grp;
}

// ---- ___crtGetStringTypeA (D2Net+0x04F78) / __convertcp (0x05950) / ___ansicp (0x05909)
// 1.13c D2NET+0x05909
int ansicp(unsigned lcid) {
  char b[6]; b[5] = 0;
  if (!g.GetLocaleInfoA(lcid, 0x1004, b, 6)) return -1;
  return atoi(b);                                         // _atol
}

// 1.13c D2NET+0x05950
char* convertcp(unsigned from, unsigned to, const char* src, int* plen, char* dst, int dstlen) {
  int len = *plen;
  if (from == to) return nullptr;                         // retail returns 0 without converting
  // Single-byte pair shortcut: retail compares GetCPInfo(MaxCharSize)==1 on both, then MBToWC/WCToMB via a
  // stack-or-calloc'd UTF-16 buffer of 2*n bytes, result length written back to *plen when len != -1.
  bool sb = g.GetCPInfoMax(from) == 1 && g.GetCPInfoMax(to) == 1;
  int n = sb ? (len == -1 ? (int)strlen(src) + 1 : len) : 0;
  if (!sb) n = g.MBToWC(from, 1, src, len, nullptr, 0);
  if (!n) return nullptr;
  uint16_t* w = (uint16_t*)g.calloc_(2, n);
  if (!w) return nullptr;
  if (!g.MBToWC(from, 1, src, len, w, n)) { g.free_(w); return nullptr; }
  char* out = dst;
  int outn = dstlen;
  if (!out) {
    outn = g.WCToMB(to, 0, w, n, nullptr, 0, nullptr, nullptr);
    if (!outn) { g.free_(w); return nullptr; }
    out = (char*)g.calloc_(1, outn);
    if (!out) { g.free_(w); return nullptr; }
  }
  int r = g.WCToMB(to, 0, w, n, out, outn, nullptr, nullptr);
  g.free_(w);
  if (!r) { if (out != dst) g.free_(out); return nullptr; }
  if (len != -1) *plen = r;
  return out;
}

// 1.13c D2NET+0x04F78
int crtGetStringTypeA(unsigned type, const char* src, int cch, uint16_t* out, unsigned cp, unsigned lcid, int err) {
  if (!g.stype_mode) {                                    // probe the wide API once
    uint16_t t;
    if (g.GetStringTypeW(1, (const uint16_t*)L"", 1, &t)) g.stype_mode = 1;
    else if (g.GetLastError() == 0x78) g.stype_mode = 2;  // ERROR_CALL_NOT_IMPLEMENTED
  }
  if (g.stype_mode == 1 || g.stype_mode == 0) {
    if (!cp) cp = g.lc_codepage;
    int n = g.MBToWC(cp, err ? 9 : 1, src, cch, nullptr, 0);
    if (!n) return 0;
    uint16_t* w = (uint16_t*)g.calloc_(2, n);
    if (!w) return 0;
    int r = 0;
    if (g.MBToWC(cp, 1, src, cch, w, n)) r = g.GetStringTypeW(type, w, n, out);
    g.free_(w);
    return r;
  }
  if (!lcid) lcid = g.lc_handle;
  if (!cp) cp = g.lc_codepage;
  unsigned acp = ansicp(lcid);
  if (acp == 0xFFFFFFFFu) return 0;
  if (acp != cp) {                                        // convert the source into the LCID's ANSI page
    int len = cch;
    char* s = convertcp(cp, acp, src, &len, nullptr, 0);
    if (!s) return 0;
    int r = g.GetStringTypeA(lcid, type, s, len, out);
    g.free_(s);
    return r;
  }
  return g.GetStringTypeA(lcid, type, src, cch, out);
}

// ---- __crtLCMapStringA (D2Net+0x051C0)
// 1.13c D2NET+0x051C0
int crtLCMapStringA(unsigned lcid, unsigned fl, const char* src, int cch, char* dst, int ndst, unsigned cp, int err) {
  if (!cp) cp = g.lc_codepage;
  int n = g.MBToWC(cp, err ? 9 : 1, src, cch, nullptr, 0);
  if (!n) return 0;
  uint16_t* w = (uint16_t*)g.calloc_(2, n);
  if (!w) return 0;
  int r = 0;
  if (g.MBToWC(cp, 1, src, cch, w, n)) {
    int m = g.LCMapW(lcid, fl, w, n, nullptr, 0);
    if (m) {
      if (fl & 0x400) {                                   // LCMAP_SORTKEY: bytes straight into dst
        if (ndst && m <= ndst) r = g.LCMapW(lcid, fl, w, n, (uint16_t*)dst, ndst);
        else if (!ndst) r = m;
      } else {
        uint16_t* w2 = (uint16_t*)g.calloc_(2, m);
        if (w2) {
          if (g.LCMapW(lcid, fl, w, n, w2, m))
            r = ndst ? g.WCToMB(cp, 0, w2, m, dst, ndst, nullptr, nullptr)
                     : g.WCToMB(cp, 0, w2, m, nullptr, 0, nullptr, nullptr);
          g.free_(w2);
        }
      }
    }
  }
  g.free_(w);
  return r;
}

// ---- __security_error_handler (D2Net+0x055FC)
// 1.13c D2NET+0x055FC
void security_error(int code, int arg) {
  if (g.Handler) { g.Handler(code, arg); return; }
  const char* title = code == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
  char buf[0x120];
  const char* mod = g.ModuleName ? g.ModuleName() : nullptr;
  char path[0x104]; if (mod) strncpy(path, mod, 0x103), path[0x103] = 0; else strcpy(path, "<program name unknown>");
  const char* p = path;
  if (strlen(p) + 11 > 60) p = path + strlen(path) - 49;  // keep the tail "...<last 49 chars>"
  snprintf(buf, sizeof buf, "%s\n\nProgram: %s\n\n", title, p);
  g.MsgBox(buf, "Microsoft Visual C++ Runtime Library", 0x12010);
  g.Exit(-1);
}

// ---- DllMain thread-detach reset (D2Net+0x06AA0): stdcall(hinst, reason, reserved) -> 1
// 1.13c D2NET+0x06AA0
int dllmain_stub(void*, int reason, void*, int* flag) {
  if (reason == 1) *flag = 0;                             // DAT_6fbfb418
  return 1;
}

}  // namespace d2net_n116
