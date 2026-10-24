// D2Net.dll MSVC 2003 CRT heap/io/env/args block (0x1EF0-0x2693). Wasm form: the Win32 calls are the
// Host seam (function pointers a caller fills; wasm has no HeapAlloc/GetFileType), globals are CrtState.
// No packet bytes; shapes follow the decompile, the small-block heap (mode 3) is a seam, not reimplemented.
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace d2net_n38 {

struct Host {
  void* (*sbh_alloc)(size_t);          // ___sbh_alloc_block, nullptr result = fall through to HeapAlloc
  bool (*sbh_free)(void*);             // find_block + free_block; true if the block was SBH-owned
  void (*lock)(int);                   // __lock(n)
  void (*unlock)(int);                 // _mtunlock_(n)
  int (*callnewh)(size_t);             // __callnewh
  int (*init_cs)(void*, int);          // ___crtInitCritSecAndSpinCount
  void (*del_cs)(void*);               // DeleteCriticalSection
  int (*file_type)(intptr_t);          // GetFileType, 0 = unknown
  intptr_t (*std_handle)(uint32_t);    // GetStdHandle, -1 = invalid
};

struct IoInfo { intptr_t osfhnd; uint8_t osfile, pipech; int lockinit; uint8_t cs[24]; };  // 0x24 bytes
static_assert(sizeof(IoInfo) >= 0x24, "ioinfo is 0x24 on x86; wider here, stride is sizeof");

struct CrtState {
  int heap_mode = 1;                   // DAT_6fbfb684
  size_t sbh_threshold = 0;            // DAT_6fbfb444
  int newh_set = 0;                    // DAT_6fbfb0d8
  int nhandle = 0;                     // DAT_6fbfb688
  IoInfo* pio[0x40] = {};              // DAT_6fbfb6a0[] (32 entries per block)
  int winplatform = 0, winmajor = 0;   // DAT_6fbfada4 / DAT_6fbfadb0
  char* envblock = nullptr;            // DAT_6fbfadec
  char** environ_ = nullptr;           // DAT_6fbfadc4
  int env_init = 0;                    // DAT_6fbfb7a8
  int envmode = 0;                     // DAT_6fbfb068 (___crtGetEnvironmentStringsA mode)
  int argc = 0; char** argv = nullptr; // 0x6fbfadb8 / 0x6fbfadbc
  const char* pgmptr = nullptr;
  const uint8_t* ctype = nullptr;      // DAT_6fbfb461 table, bit 4 = DBCS lead byte
};

// 1.13c D2Net+0x1EF0
void crt_free(CrtState& s, const Host& h, void* p) {
  if (!p) return;
  if (s.heap_mode == 3) {
    h.lock(4);
    bool sbh = h.sbh_free(p);
    h.unlock(4);
    if (sbh) return;
  }
  std::free(p);
}

// 1.13c D2NET+0x01F43
void unlock_4_1f43(const Host& h) { h.unlock(4); }

// 1.13c D2Net+0x1F61  _calloc-shaped (count, size) zeroed allocator with the new-handler retry loop
void* crt_calloc(CrtState& s, const Host& h, uint32_t n, uint32_t sz) {
  if (n != 0 && sz > 0xffffffe0u / n) return nullptr;
  uint32_t size = n * sz, bytes = size ? size : 1;
  for (;;) {
    void* p = nullptr;
    if (bytes < 0xffffffe1u) {
      if (s.heap_mode == 3 && (bytes = (bytes + 15) & ~15u, size <= s.sbh_threshold)) {
        h.lock(4);
        p = h.sbh_alloc(size);
        h.unlock(4);
        if (p) { std::memset(p, 0, size); return p; }
      }
      p = std::calloc(1, bytes);
      if (p) return p;
    }
    if (!s.newh_set) return nullptr;
    if (!h.callnewh(bytes)) return nullptr;
  }
}

// 1.13c D2Net+0x2027
void unlock_4_2027(const Host& h) { h.unlock(4); }

static void ioinit_block(IoInfo* b) {
  for (int i = 0; i < 32; ++i) { b[i].osfhnd = -1; b[i].lockinit = 0; b[i].osfile = 0; b[i].pipech = 10; }
}

// 1.13c D2Net+0x2038  __ioinit; reserved2 = STARTUPINFO.lpReserved2 (cbReserved2 bytes) or null
int ioinit(CrtState& s, const Host& h, const uint8_t* reserved2, unsigned cb) {
  s.pio[0] = static_cast<IoInfo*>(std::malloc(32 * sizeof(IoInfo)));
  if (!s.pio[0]) return -1;
  s.nhandle = 32;
  ioinit_block(s.pio[0]);
  if (cb && reserved2) {
    int n; std::memcpy(&n, reserved2, 4);
    const uint8_t* flags = reserved2 + 4;
    const uint8_t* hnd = flags + n;
    if (n > 0x7ff) n = 0x800;
    int cnt = n;
    for (int blk = 1; s.nhandle < n; ++blk) {
      IoInfo* b = static_cast<IoInfo*>(std::malloc(32 * sizeof(IoInfo)));
      if (!b) { cnt = s.nhandle; break; }
      s.nhandle += 32; s.pio[blk] = b; ioinit_block(b);
    }
    for (int i = 0; i < cnt; ++i, hnd += 4, ++flags) {
      intptr_t hv; int32_t t; std::memcpy(&t, hnd, 4); hv = t;
      if (hv != -1 && (*flags & 1) && ((*flags & 8) || h.file_type(hv) != 0)) {
        IoInfo* e = &s.pio[i >> 5][i & 31];
        e->osfhnd = hv; e->osfile = *flags;
        if (!h.init_cs(e->cs, 4000)) return -1;
        ++e->lockinit;
      }
    }
  }
  for (int i = 0; i < 3; ++i) {
    IoInfo* e = &s.pio[0][i];
    if (e->osfhnd == -1) {
      e->osfile = 0x81;
      intptr_t hf = h.std_handle(i == 0 ? 0xfffffff6u : 0xfffffff5u - (i != 1));
      int ft;
      if (hf == -1 || (ft = h.file_type(hf)) == 0) { e->osfile |= 0x40; }
      else {
        e->osfhnd = hf;
        if ((ft & 0xff) == 2) e->osfile |= 0x40; else if ((ft & 0xff) == 3) e->osfile |= 8;
        if (!h.init_cs(e->cs, 4000)) return -1;
        ++e->lockinit;
      }
    } else e->osfile |= 0x80;
  }
  return 0;  // SetHandleCount(nhandle): no wasm analogue needed, the table is the count
}

// 1.13c D2Net+0x2236  __ioterm
void ioterm(CrtState& s, const Host& h) {
  for (auto& blk : s.pio) {
    if (!blk) continue;
    for (int i = 0; i < 32; ++i) if (blk[i].lockinit) h.del_cs(blk[i].cs);
    std::free(blk);
    blk = nullptr;
  }
}

// 1.13c D2Net+0x2282  __setenvp: split the NUL-separated block into environ, skipping "=X" entries
int setenvp(CrtState& s) {
  if (!s.envblock) return -1;
  int n = 0;
  for (char* p = s.envblock; *p; p += std::strlen(p) + 1) if (*p != '=') ++n;
  char** e = static_cast<char**>(std::malloc(n * sizeof(char*) + sizeof(char*)));
  s.environ_ = e;
  if (!e) return -1;
  for (char* p = s.envblock; *p; p += std::strlen(p) + 1) {
    if (*p == '=') continue;
    size_t l = std::strlen(p);
    *e = static_cast<char*>(std::malloc(l + 1));
    if (!*e) { std::free(s.environ_); s.environ_ = nullptr; return -1; }
    std::memcpy(*e++, p, l + 1);
  }
  std::free(s.envblock); s.envblock = nullptr;
  *e = nullptr;
  s.env_init = 1;
  return 0;
}

// 1.13c D2Net+0x2349  parse_cmdline (regs: EAX=cmdline, ECX=args, ESI=numchars -> explicit)
void parse_cmdline(const uint8_t* ctype, const uint8_t* cmd, char** argv, char* args, int* numchars, int* numargs) {
  const uint8_t* p = cmd;
  bool inq = false;
  uint8_t c;
  *numchars = 0; *numargs = 1;
  if (argv) *argv++ = args;
  do {  // program name: quotes toggle, no escapes
    if (*p == '"') { inq = !inq; c = '"'; ++p; }
    else {
      ++*numchars;
      if (args) *args++ = *p;
      c = *p++;
      if (ctype[c] & 4) { ++*numchars; if (args) *args++ = *p; ++p; }
      if (c == 0) { --p; goto rest; }
    }
  } while (inq || (c != ' ' && c != '\t'));
  if (args) args[-1] = 0;
rest:
  inq = false;
  while (*p) {
    while (*p == ' ' || *p == '\t') ++p;
    if (!*p) break;
    if (argv) *argv++ = args;
    ++*numargs;
    for (;;) {
      bool copy = true;
      unsigned bs = 0;
      while (*p == '\\') { ++p; ++bs; }
      if (*p == '"') {
        const uint8_t* q = p;
        if (!(bs & 1)) {
          if (!inq || *(q = p + 1) != '"') { copy = false; q = p; }
          inq = !inq;
        }
        bs >>= 1; p = q;
      }
      for (; bs; --bs) { if (args) *args++ = '\\'; ++*numchars; }
      c = *p;
      if (!c || (!inq && (c == ' ' || c == '\t'))) break;
      if (copy) {
        if (args) {
          if (ctype[c] & 4) { *args++ = c; ++p; ++*numchars; }
          *args++ = *p;
        } else if (ctype[c] & 4) { ++p; ++*numchars; }
        ++*numchars;
      }
      ++p;
    }
    if (args) *args++ = 0;
    ++*numchars;
  }
  if (argv) *argv = nullptr;
  ++*numargs;
}

// 1.13c D2Net+0x24B5  __setargv; module = GetModuleFileNameA text, cmdline = the "_acmdln" string
int setargv(CrtState& s, const char* module, const char* cmdline) {
  static char pgm[0x104];
  std::strncpy(pgm, module, sizeof pgm - 1);
  s.pgmptr = pgm;
  int nc, na;
  parse_cmdline(s.ctype, reinterpret_cast<const uint8_t*>(pgm), nullptr, nullptr, &nc, &na);
  (void)cmdline;  // ponytail: decompile parses the module name here too (_pgmptr), cmdline is _wincmdln's job
  char** av = static_cast<char**>(std::malloc(nc + na * sizeof(char*)));
  if (!av) return -1;
  parse_cmdline(s.ctype, reinterpret_cast<const uint8_t*>(pgm), av, reinterpret_cast<char*>(av + na), &nc, &na);
  s.argc = na - 1;
  s.argv = av;
  return 0;
}

// 1.13c D2Net+0x2557  ___crtGetEnvironmentStringsA: wasm has one environment form (narrow); the
// W->A / A copy of retail both end as a malloc'd NUL-NUL block, so copy the host block.
char* crt_get_environment_strings(const char* host_block) {
  if (!host_block) return nullptr;
  const char* p = host_block;
  while (*p) p += std::strlen(p) + 1;
  size_t n = static_cast<size_t>(p - host_block) + 1;
  char* d = static_cast<char*>(std::malloc(n));
  if (d) std::memcpy(d, host_block, n);
  return d;
}

// 1.13c D2Net+0x2679  ___heap_select
int heap_select(const CrtState& s) { return (s.winplatform == 2 && s.winmajor > 4) ? 1 : 3; }

// 1.13c D2Net+0x2693  __heap_init: HeapCreate is the wasm allocator (always present); sbh seam picks mode
int heap_init(CrtState& s, int (*sbh_heap_init)(int)) {
  s.heap_mode = heap_select(s);
  if (s.heap_mode == 3 && !sbh_heap_init(0x3f8)) return 0;
  return 1;
}

}  // namespace d2net_n38
