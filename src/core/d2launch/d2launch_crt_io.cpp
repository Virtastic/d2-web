// D2Launch statically linked MSVC CRT: low-level io table (ioinfo), __sopen family, small-block heap header.
// Win32 services come through Host (function pointers) so wasm supplies them; the 32-bit table layout is kept.
#include <cstdint>
#include <cstdlib>

namespace d2launch_crt {

struct Host {
  uint32_t (*CreateFileA)(const char*, uint32_t access, uint32_t share, uint32_t disp, uint32_t flags);  // -1 on failure
  uint32_t (*GetFileType)(uint32_t h);
  void (*CloseHandle)(uint32_t h);
  uint32_t (*SetFilePointer)(uint32_t h, int32_t lo, int32_t* hi, uint32_t method);  // -1 + GetLastError!=0 on failure
  uint32_t (*GetLastError)();
  void (*SetStdHandle)(uint32_t id, uint32_t h);
  void (*dosmaperr)(uint32_t winerr);
  void (*EnterCS)(void* cs);
  void (*LeaveCS)(void* cs);
  bool (*InitCS)(void* cs, uint32_t spin);
  void (*lock)(int n);     // __lock
  void (*unlock)(int n);   // __unlock
  void* (*heapAlloc)(uint32_t n);
  int (*lseek_lk)(int fh, int off, int whence);   // __lseek_lk
  int (*read_lk)(int fh, char* buf, unsigned n);  // FUN_6fa43d82
  int (*chsize_lk)(int fh, int size);             // FUN_6fa48382
  void (*close_lk)(int fh);
};

struct ioinfo {  // 0x24 bytes
  uint32_t osfhnd;
  uint8_t osfile, pipech;
  uint16_t pad;
  int32_t lockinit;
  uint8_t cs[24];
};
static_assert(sizeof(ioinfo) == 0x24, "ioinfo");

struct State {
  Host h;
  int32_t errnoV = 0;
  uint32_t doserrnoV = 0;
  ioinfo* pioinfo[0x40] = {};  // DAT_6fa66fa0
  uint32_t nhandle = 0;        // DAT_6fa66f84
  int fmode = 0;               // DAT_6fa65140 (0x8000 = binary)
  uint32_t umaskV = 0;         // DAT_6fa64de0
  int osplatform = 0;          // DAT_6fa64de4
  int consoleMode = 0;         // DAT_6fa64e38 (1 = SetStdHandle on std fds)
  // small-block heap globals
  uint8_t* sbhRegions = nullptr;  // DAT_6fa66f68
  uint32_t sbhCount = 0;          // DAT_6fa66f64
  uint32_t sbhA = 0;              // DAT_6fa66f60
  uint8_t* sbhCur = nullptr;      // DAT_6fa66f70
  uint32_t sbhThreshold = 0;      // DAT_6fa66f6c
  uint32_t sbhCap = 0;            // DAT_6fa66f74
};

static void seterr(State& s, int e) { s.errnoV = e; s.doserrnoV = 0; }
static ioinfo* io(State& s, int fh) { return &s.pioinfo[fh >> 5][fh & 0x1f]; }

// 1.13c D2Launch+0x06929
int set_osfhnd(State& s, int fh, uint32_t h) {
  if ((uint32_t)fh < s.nhandle && io(s, fh)->osfhnd == 0xffffffffu) {
    if (s.consoleMode == 1 && fh >= 0 && fh <= 2) s.h.SetStdHandle(fh == 0 ? 0xfffffff6 : fh == 1 ? 0xfffffff5 : 0xfffffff4, h);
    io(s, fh)->osfhnd = h;
    return 0;
  }
  seterr(s, 9);
  return -1;
}

// 1.13c D2Launch+0x069A5
int free_osfhnd(State& s, int fh) {
  if ((uint32_t)fh < s.nhandle) {
    ioinfo* p = io(s, fh);
    if ((p->osfile & 1) && p->osfhnd != 0xffffffffu) {
      if (s.consoleMode == 1 && fh >= 0 && fh <= 2) s.h.SetStdHandle(fh == 0 ? 0xfffffff6 : fh == 1 ? 0xfffffff5 : 0xfffffff4, 0);
      p->osfhnd = 0xffffffffu;
      return 0;
    }
  }
  seterr(s, 9);
  return -1;
}

// 1.13c D2Launch+0x06AFC
void unlock10(State& s) { s.h.unlock(10); }
// 1.13c D2Launch+0x06C00
void unlock10b(State& s) { s.h.unlock(10); }
// 1.13c D2Launch+0x06C9A
void unlock11(State& s) { s.h.unlock(11); }

// 1.13c D2Launch+0x06B27
int alloc_osfhnd(State& s) {
  int fh = -1;
  s.h.lock(11);
  for (int i = 0; i < 0x40; i++) {
    ioinfo* p = s.pioinfo[i];
    if (!p) {
      p = (ioinfo*)s.h.heapAlloc(0x480);
      if (p) {
        s.pioinfo[i] = p;
        s.nhandle += 0x20;
        for (ioinfo* q = p; q < p + 0x20; q++) { q->osfile = 0; q->osfhnd = 0xffffffffu; q->pipech = 10; q->lockinit = 0; }
        fh = i << 5;
        // __lock_fhandle: lock entry fh (lazy cs init, as below)
        if (!p->lockinit) {
          s.h.lock(10);
          if (!p->lockinit) { if (!s.h.InitCS(p->cs, 4000)) { s.h.unlock(10); fh = -1; goto done; } p->lockinit++; }
          s.h.unlock(10);
        }
        s.h.EnterCS(p->cs);
      }
      break;
    }
    for (ioinfo* q = p; q < p + 0x20; q++) {
      if (!(q->osfile & 1)) {
        if (!q->lockinit) {
          s.h.lock(10);
          if (!q->lockinit) {
            if (!s.h.InitCS(q->cs, 4000)) { s.h.unlock(10); fh = -1; goto done; }
            q->lockinit++;
          }
          s.h.unlock(10);
        }
        s.h.EnterCS(q->cs);
        if (!(q->osfile & 1)) { q->osfhnd = 0xffffffffu; fh = (int)(q - p) + i * 0x20; break; }
        s.h.LeaveCS(q->cs);
      }
    }
    if (fh != -1) break;
  }
done:
  s.h.unlock(11);
  return fh;
}

// 1.13c D2Launch+0x06896
void sopen_unlock(State& s, int locked, int fh) { if (locked) s.h.LeaveCS(io(s, fh)->cs); }  // __unlock_fhandle

// 1.13c D2Launch+0x068A6  (returns 64-bit position; -1 on error)
int64_t lseeki64_lk(State& s, int fh, int32_t lo, int32_t hi, uint32_t method) {
  if (fh < 0 || (uint32_t)fh >= s.nhandle || io(s, fh)->osfhnd == 0xffffffffu) { s.errnoV = 9; return -1; }
  int32_t h = hi;
  uint32_t r = s.h.SetFilePointer(io(s, fh)->osfhnd, lo, &h, method);
  if (r == 0xffffffffu) {
    uint32_t e = s.h.GetLastError();
    if (e) { s.h.dosmaperr(e); return -1; }
  }
  io(s, fh)->osfile &= 0xfd;
  return (int64_t)(((uint64_t)(uint32_t)h << 32) | r);
}

// 1.13c D2Launch+0x0656A  (share = 0x10/0x20/0x30/0x40; returns fh or -1; *unlockFlag/*fhOut as retail)
int tsopen_lk(State& s, int* unlockFlag, int* fhOut, const char* path, unsigned oflag, unsigned pmode, unsigned share) {
  uint8_t fl = (int8_t)oflag >= 0 ? 0 : 0x10;
  uint32_t inherit = (int8_t)oflag >= 0;  // SECURITY_ATTRIBUTES.bInheritHandle (not forwarded to the host)
  (void)inherit;
  if (!(oflag & 0x8000) && ((oflag & 0x4000) || s.fmode != 0x8000)) fl |= 0x80;
  uint32_t access, sh, disp;
  switch (oflag & 3) { case 0: access = 0x80000000; break; case 1: access = 0x40000000; break; case 2: access = 0xc0000000; break; default: goto bad; }
  switch (share) { case 0x10: sh = 0; break; case 0x20: sh = 1; break; case 0x30: sh = 2; break; case 0x40: sh = 3; break; default: goto bad; }
  switch (oflag & 0x700) {
    case 0: case 0x400: disp = 3; break;
    case 0x100: disp = 4; break;
    case 0x300: disp = 2; break;
    case 0x200: case 0x600: disp = 5; break;
    case 0x500: case 0x700: disp = 1; break;
    default: goto bad;
  }
  {
    uint32_t flags = 0x80;
    if ((oflag & 0x100) && !((uint8_t)(~s.umaskV & pmode) & 0x80)) flags = 1;
    if (oflag & 0x40) { access = 0x10000; flags |= 0x4000000; if (s.osplatform == 2) sh |= 4; }
    if (oflag & 0x1000) flags |= 0x100;
    if (oflag & 0x20) flags |= 0x8000000; else if (oflag & 0x10) flags |= 0x10000000;
    int fh = alloc_osfhnd(s);
    if (fh == -1) { seterr(s, 0x18); return -1; }
    *unlockFlag = 1;
    *fhOut = fh;
    uint32_t h = s.h.CreateFileA(path, access, sh, disp, flags);
    if (h != 0xffffffffu) {
      uint32_t t = s.h.GetFileType(h);
      if (t != 0) {
        if (t == 2) fl |= 0x40; else if (t == 3) fl |= 8;
        set_osfhnd(s, fh, h);
        fl |= 1;
        io(s, fh)->osfile = fl;
        uint8_t mask = fl & 0x48;
        if (mask == 0 && (int8_t)fl < 0 && (oflag & 2)) {
          int pos = s.h.lseek_lk(fh, -1, 2);
          bool ok;
          if (pos == -1) ok = (s.doserrnoV == 0x83);
          else {
            char c = 0;
            ok = !(s.h.read_lk(fh, &c, 1) == 0 && c == 0x1a && s.h.chsize_lk(fh, pos) == -1) && s.h.lseek_lk(fh, 0, 0) != -1;
          }
          if (!ok) { s.h.close_lk(fh); return -1; }
        }
        if (mask == 0 && (oflag & 8)) io(s, fh)->osfile |= 0x20;
        return fh;
      }
      s.h.CloseHandle(h);
    }
    s.h.dosmaperr(s.h.GetLastError());
    return -1;
  }
bad:
  seterr(s, 0x16);
  return -1;
}

// 1.13c D2Launch+0x06851
int sopen(State& s, const char* path, unsigned oflag, unsigned share, unsigned pmode) {
  int locked = 0, fh = 0;
  int r = tsopen_lk(s, &locked, &fh, path, oflag, pmode, share);
  sopen_unlock(s, locked, fh);
  return r;
}

// 1.13c D2Launch+0x06CA3
int sbh_heap_init(State& s, uint32_t threshold) {
  s.sbhRegions = (uint8_t*)s.h.heapAlloc(0x140);
  if (!s.sbhRegions) return 0;
  s.sbhA = 0; s.sbhCount = 0; s.sbhCur = s.sbhRegions; s.sbhThreshold = threshold; s.sbhCap = 0x10;
  return 1;
}

// 1.13c D2Launch+0x06CEB  (region records are 0x14 bytes; +0xc = base address; 32-bit addr as in retail)
uint8_t* sbh_find_block(State& s, uint32_t addr) {
  for (uint8_t* p = s.sbhRegions; p < s.sbhRegions + s.sbhCount * 0x14; p += 0x14)
    if (addr - *(uint32_t*)(p + 0xc) < 0x100000) return p;
  return nullptr;
}

}  // namespace d2launch_crt
