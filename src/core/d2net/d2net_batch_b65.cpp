// D2Net CRT lock / exit / free helpers, batch b65. Platform calls are hooks.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <initializer_list>

namespace d2net_b65 {

struct Crt {
  void* lock[48] = {};            // DAT_6fbfa038, stride 8: one critical-section ptr per lock id
  int mode_a = 0, mode_b = 0;     // DAT_6fbfadf4 / DAT_6fbfadf8 (console/gui run-type flags)
  void (*banner_hook)() = nullptr;        // DAT_6fbfb06c
  void (*exit_fn)(unsigned) = nullptr;    // PTR___exit_6fbfa030
  void (*enter)(void*) = nullptr;         // EnterCriticalSection
  void (*leave)(void*) = nullptr;         // LeaveCriticalSection
  bool (*init_cs)(void*, int) = nullptr;  // ___crtInitCritSecAndSpinCount
  void (*write_msg)(int) = nullptr;       // FUN_6fbf292c body (rterr table lookup + output)
  void (*doexit)(unsigned, int, int) = nullptr;
  int errno_v = 0;                        // FUN_6fbf2b78 -> &errno
  int heap_mode = 0;                      // DAT_6fbfb684 (3 = small-block heap)
  void (*sbh_free)(void*) = nullptr;
};

static bool console_or_gui(const Crt& c) { return c.mode_a == 1 || (c.mode_a == 0 && c.mode_b == 1); }

// 1.13c D2Net+0x182B  (leave lock id)
void leave_lock(Crt& c, int id) { c.leave(c.lock[id]); }

// 1.13c D2Net+0x18D7  (unlock the lock-table lock, id 10)
void unlock_10(Crt& c) { leave_lock(c, 10); }

// 1.13c D2Net+0x1840  __mtinitlocknum
int mtinitlocknum(Crt& c, int id) {
  if (c.lock[id]) return 1;
  void* m = std::malloc(0x18);
  if (!m) { c.errno_v = 12; return 0; }
  c.enter(c.lock[10]);
  if (!c.lock[id]) {
    if (!c.init_cs(m, 4000)) { std::free(m); c.errno_v = 12; c.leave(c.lock[10]); return 0; }
    c.lock[id] = m;
  } else std::free(m);
  unlock_10(c);
  return 1;
}

// 1.13c D2Net+0x14D5  __exit
void crt_exit(Crt& c, unsigned code) { c.doexit(code, 1, 0); }

// 1.13c D2Net+0x2AA4  __FF_MSGBANNER
void ff_msgbanner(Crt& c) {
  if (!console_or_gui(c)) return;
  c.write_msg(0xfc);
  if (c.banner_hook) c.banner_hook();
  c.write_msg(0xff);
}

// 1.13c D2Net+0x175A  __amsg_exit
void amsg_exit(Crt& c, int msg) {
  if (console_or_gui(c)) ff_msgbanner(c);
  c.write_msg(msg);
  c.exit_fn(0xff);
}

// 1.13c D2Net+0x18E0  __lock
void lock(Crt& c, int id) {
  if (!c.lock[id] && !mtinitlocknum(c, id)) amsg_exit(c, 0x11);
  c.enter(c.lock[id]);
}

// 1.13c D2Net+0x1EF0  _free
void crt_free(Crt& c, void* p) {
  if (!p) return;
  if (c.heap_mode == 3) { lock(c, 4); c.sbh_free(p); leave_lock(c, 4); return; }
  std::free(p);
}

// 1.13c D2Net+0x2AED  ___crtInitCritSecAndSpinCount
bool crt_init_cs(Crt& c, void* cs, int spin) { return c.init_cs(cs, spin); }

// 1.13c D2Net+0x292C  FUN_6fbf292c (runtime-error message writer; Win32 output is the hook)
void rterr_write(Crt& c, int msg) { c.write_msg(msg); }

}  // namespace d2net_b65
