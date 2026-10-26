// D2Net CRT heap/exception/runtime-error helpers (n43). Shape follows the BNClient/D2MCPClient CRT copies;
// platform calls (Win32 heap, stdout, message box, kernel32 lookup) are hooks on one state struct.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>

namespace d2net_n43 {

struct XcptEntry { uint32_t num, signo; void (*act)(int, ...); };  // 12-byte _XCPT_ACTION row

struct Ptd {                       // the _tiddata fields __XcptFilter touches
  XcptEntry* tab = nullptr;        // _initaddr
  void* initarg = nullptr;         // _initarg (current _EXCEPTION_POINTERS)
  int fpecode = 0;                 // _pxcptacttab slot
  int terrno = 0;                  // _terrno
};

struct Heap {
  int mode_a = 0, mode_b = 0;      // DAT_6fbfadf4 / DAT_6fbfadf8
  int heap_mode = 0;               // DAT_6fbfb684 (3 = small-block heap)
  size_t sbh_max = 0;              // DAT_6fbfb444
  int fpe_first = 0, fpe_count = 0;// DAT_6fbfa208 / DAT_6fbfa20c
  int xcpt_count = 0;              // DAT_6fbfa214
  void (*banner_hook)() = nullptr; // DAT_6fbfb06c
  int (*unhandled)(void*) = nullptr;
  void* (*sbh_alloc)(size_t) = nullptr;
  void* (*heap_alloc)(size_t) = nullptr;   // HeapAlloc(DAT_6fbfb680,0,n)
  int (*callnewh)(size_t) = nullptr;
  void (*lock)(int) = nullptr;
  void (*unlock)(int) = nullptr;
  void (*write_stdout)(const char*, size_t) = nullptr;
  void (*msgbox)(const char*, const char*, unsigned) = nullptr;
  const char* (*module_path)() = nullptr;  // GetModuleFileNameA(0,..) or null on failure
  const char* (*cs_init_spin)() = nullptr; // GetProcAddress lookup: nonnull = found
  void (*init_cs)(void*) = nullptr;
  bool (*init_cs_spin)(void*, int) = nullptr;
  bool spin_resolved = false, spin_found = false;  // DAT_6fbfb070 state
  int platform_flag = 0;                   // DAT_6fbfada4
  // heap_term
  int sbh_regions = 0;
  void (*virtual_free)(int region, uint32_t size, uint32_t type) = nullptr;
  void (*heap_free_region)(int region) = nullptr;
  void (*heap_free_table)() = nullptr;
  void (*heap_destroy)() = nullptr;
};

struct Msg { int id; const char* text; };

// 1.13c D2Net+0x26E4  __heap_term
void heap_term(Heap& h) {
  if (h.heap_mode == 3) {
    for (int i = 0; i < h.sbh_regions; i++) {
      h.virtual_free(i, 0x100000, 0x4000);  // MEM_DECOMMIT
      h.virtual_free(i, 0, 0x8000);         // MEM_RELEASE
      h.heap_free_region(i);
    }
    h.heap_free_table();
  }
  h.heap_destroy();
}

// 1.13c D2Net+0x2770  __chkstk: probes page by page, returns the stack pointer after eax bytes are reserved
uintptr_t chkstk(uintptr_t sp, uint32_t eax) { return sp - eax; }

// 1.13c D2Net+0x27AD  __XcptFilter
int xcpt_filter(Heap& h, Ptd& p, uint32_t num, void* ptrs, void (*raise_fpe)(int, int) = nullptr) {
  XcptEntry* e = p.tab;
  XcptEntry* end = p.tab + h.xcpt_count;
  for (; e < end; e++) if (e->num == num) break;
  if (e >= end || e->num != num) e = nullptr;
  if (!e || !e->act) return h.unhandled(ptrs);
  auto act = e->act;
  if ((uintptr_t)act == 5) { e->act = nullptr; return 1; }
  if ((uintptr_t)act != 1) {
    void* saved = p.initarg;
    p.initarg = ptrs;
    if (e->signo == 8) {
      for (int i = h.fpe_first; i < h.fpe_count + h.fpe_first; i++) p.tab[i].act = nullptr;
      int old = p.fpecode;
      switch (e->num) {
        case 0xc000008e: p.fpecode = 0x83; break;
        case 0xc0000090: p.fpecode = 0x81; break;
        case 0xc0000091: p.fpecode = 0x84; break;
        case 0xc0000093: p.fpecode = 0x85; break;
        case 0xc000008d: p.fpecode = 0x82; break;
        case 0xc000008f: p.fpecode = 0x86; break;
        case 0xc0000092: p.fpecode = 0x8a; break;
      }
      act(8, p.fpecode);
      p.fpecode = old;
    } else {
      e->act = nullptr;
      act((int)e->signo);
    }
    p.initarg = saved;
  }
  return -1;
}

// 1.13c D2Net+0x2911  SEH filter: only the C++ exception code 0xE06D7363 goes to __XcptFilter
int seh_filter(Heap& h, Ptd& p, int code, void* ptrs) {
  return code == -0x1f928c9d ? xcpt_filter(h, p, 0xe06d7363u, ptrs) : 0;
}
int seh_filter_probe(int code) { Heap h; Ptd p; return seh_filter(h, p, code, nullptr); }  // FN probe seam

static bool console_or_gui(const Heap& h) { return h.mode_a == 1 || (h.mode_a == 0 && h.mode_b == 1); }

// 1.13c D2Net+0x292C  runtime-error writer: table lookup (19 rows), then stdout or the message box text
std::string rterr_write(Heap& h, const Msg* tab, int id) {
  int i = 0;
  while (i < 0x13 && tab[i].id != id) i++;
  if (i >= 0x13 || tab[i].id != id) return {};
  if (console_or_gui(h)) { h.write_stdout(tab[i].text, strlen(tab[i].text)); return {}; }
  if (id == 0xfc) return {};
  std::string prog = h.module_path ? h.module_path() : "";
  if (prog.empty()) prog = "<program name unknown>";
  if (prog.size() + 1 > 0x3c) prog = "..." + prog.substr(prog.size() - (0x3c - 3 - 1));  // keeps the tail
  std::string out = "Runtime Error!\n\nProgram: " + prog + "\n\n" + tab[i].text;
  h.msgbox(out.c_str(), "Microsoft Visual C++ Runtime Library", 0x12010);
  return out;
}

// 1.13c D2Net+0x2AA4  __FF_MSGBANNER
void ff_msgbanner(Heap& h, const Msg* tab) {
  if (!console_or_gui(h)) return;
  rterr_write(h, tab, 0xfc);
  if (h.banner_hook) h.banner_hook();
  rterr_write(h, tab, 0xff);
}

// 1.13c D2Net+0x2ADD  ___crtInitCritSecNoSpinCount@8
int init_cs_nospin(Heap& h, void* cs) { h.init_cs(cs); return 1; }

// 1.13c D2Net+0x2AED  ___crtInitCritSecAndSpinCount: resolve once, then call through
bool init_cs_spin(Heap& h, void* cs, int spin) {
  if (!h.spin_resolved) {
    h.spin_resolved = true;
    h.spin_found = h.platform_flag != 1 && h.cs_init_spin && h.cs_init_spin();
  }
  if (h.spin_found) return h.init_cs_spin(cs, spin);
  return init_cs_nospin(h, cs) != 0;
}

// 1.13c D2Net+0x2B78  &errno
int* errno_ptr(Ptd& p) { return &p.terrno; }

// 1.13c D2Net+0x2BF3  leave lock 4
void unlock_4(Heap& h) { h.unlock(4); }

// 1.13c D2Net+0x2B81  __heap_alloc
void* heap_alloc(Heap& h, size_t n) {
  if (h.heap_mode == 3 && n <= h.sbh_max) {
    h.lock(4);
    void* p = h.sbh_alloc(n);
    unlock_4(h);
    if (p) return p;
  }
  if (n == 0) n = 1;
  if (h.heap_mode != 1) n = (n + 0xf) & ~(size_t)0xf;
  return h.heap_alloc(n);
}

// 1.13c D2Net+0x2BFC  __nh_malloc
void* nh_malloc(Heap& h, size_t n, int nhflag) {
  if (n < 0xffffffe1u) {
    do {
      if (void* p = heap_alloc(h, n)) return p;
      if (!nhflag) return nullptr;
    } while (h.callnewh(n));
  }
  return nullptr;
}

// 1.13c D2Net+0x435F  __callnewh
int callnewh(int (*handler)(size_t), size_t n) { return handler && handler(n) ? 1 : 0; }

}  // namespace d2net_n43
