// src/core/fog/fog_n80.cpp -- Fog.dll error reporter (+0x10F30 +0x11100 +0x19100) and AsyncData (+0x1CD50 +0x1D7F0
// +0x1DB40 +0x1DCC0 +0x1DF00 +0x1E010). State lives in FogN80State; every callee outside this file (pool, Storm
// async ordinals, event objects, log writer, stack walk, process exit) is a function-pointer seam the host binds.
// Win32 effects map to the closest host behaviour (events = host objects, GetThreadContext = host capture).
#include <cstdint>
#include <cstdarg>
#include <cstring>

namespace fog_n80 {
constexpr uint32_t kMagic = 0x41737960u, kMask = 0xfffffff0u;
constexpr const char* kAsync = "..\\Source\\Fog\\Src\\AsyncData.cpp";
constexpr const char* kFmt = "Unrecoverable internal error %08x";

struct Slot { void* event; uint32_t busy; void* req; };   // 12 bytes, table DAT_9af70
struct Req {                                              // 0x54 bytes
  void* file; void* pool; void* io; void* r3; uint32_t seek; void* r5; void* ev;  // +0..+0x18
  uint32_t cb[7];                                         // +0x1C
  Slot* slot; void* buf; int32_t size; int32_t prio; uint32_t flags; int32_t savedPrio; Req* next;
};

struct Host {
  // error reporter seams
  int (*suppressed)(const char* file, const char* fmt);                         // Fog+0x5FCF0
  void (*logHeader)(int line, const char* file, int type, va_list ap);          // Fog+0x609A0
  void (*captureContext)(void* thread, uint32_t* ctx /*0xB3 dwords*/);          // GetThreadContext / self
  void (*dumpModules)();                                                        // Fog+0x60E30
  void (*stackWalk)(const uint32_t* ctx);                                       // Fog+0x60B80
  void (*logText)(const char* fmt, const char* s);                              // Fog+0x68AF0
  void (*flush)();                                                              // Fog+0x684A0
  void (*prelude)();                                                            // Fog+0x5FC20 / Ordinal_116
  void (*logWrite)(int a, int b, const char* fmt, va_list ap);                  // Fog+0x68770
  void (*exit)(int);
  // pool + async seams
  void* (*poolAlloc)(void* pool);        void (*poolFree)(void* pool, void* p);     // 1B560 / 1B0C0
  void* (*eventCreate)();                void (*eventClose)(void*);                 // CreateEventA / CloseHandle
  void (*eventWait)(void*);              void* (*slotAcquire)();                    // WaitForSingleObject / 1D870
  int (*sOpen)(const char*, void**);     int (*sSize)(void*, int);                  // Ordinal 267 / 265
  int (*sSeek)(void*, uint32_t);         void (*sClose)(void*);                     // 271 / 253
  void (*sPrio)(void*, int32_t);         void (*sCancel)(void*);                    // 282 / 283
  void (*sBudget)(uint32_t which, uint32_t n);                                      // 284 / 285
  void (*sRead)(void* f, void* buf, int32_t n, Req* r, void* ud, uint32_t* cb);     // 289
  uint32_t (*poll)(Req*);                void (*complete)();                        // 1DC40 / 1D740
};
struct State {
  Host h{};
  int reporting = 0; void (*hook)() = nullptr;            // DAT_6ffa6e70 / 6ffa6e6c
  Slot* table = nullptr; uint32_t tableCount = 0, enabled = 0;  // 9af70 / 9af6c / 9af68
  Req* deferred = nullptr; int32_t deferCount = 0;        // 9af64 / 9af60
  uint32_t poolEnabled = 1;                               // 7f350
};
State g;

// 1.13c FOG+0x10F30 ordinal 10028 FogError(type, file, line, fmt, ...); type 10 takes a thread handle in `file`.
void fogError(int type, const char* file, int line, const char* fmt, ...) {
  void* thread = nullptr;
  if (type == 10) { thread = (void*)file; file = "..\\Source\\Fog\\Src\\ErrorManager.cpp"; }
  if (g.h.suppressed && g.h.suppressed(file, fmt)) return;
  g.reporting = 1;
  va_list ap; va_start(ap, fmt);
  if (g.h.logHeader) g.h.logHeader(line, file, type, ap);
  va_end(ap);
  uint32_t ctx[0xb3] = {0}; ctx[0] = 0x10007;
  if (g.h.captureContext) g.h.captureContext(thread, ctx);
  if (type == 5 || type == 6 || type == 7 || type == 10 || type == 2) {
    if (g.h.dumpModules) g.h.dumpModules();
    if (g.h.stackWalk) g.h.stackWalk(ctx);
  }
  if (g.hook) g.hook();
  if (g.h.logText) g.h.logText("%s", "");
  if (g.h.flush) g.h.flush();
  if (type != 5 && type != 6 && type != 2) g.reporting = 0;
}

[[noreturn]] static void fatal(int line) {   // prelude, FogError(6), flush, _exit(-1)
  if (g.h.prelude) g.h.prelude();
  fogError(6, "", line, kFmt);
  if (g.h.flush) g.h.flush();
  g.h.exit(-1);
  for (;;) {}
}

// 1.13c FOG+0x11100 ordinal 10024 -- report unrecoverable error at `line`, flush, return.
void fogFatalReturn(int, int, int line) {
  if (g.h.prelude) g.h.prelude();
  fogError(6, "", line, kFmt);
  if (g.h.flush) g.h.flush();
}

// 1.13c FOG+0x19100 ordinal 10029 -- logger; null fmt is fatal (line 0x118).
void fogLog(const char* fmt, ...) {
  if (!fmt) fatal(0x118);
  va_list ap; va_start(ap, fmt);
  g.h.logWrite(1, 1, fmt, ap);
  va_end(ap);
}

// 1.13c FOG+0x1CD50 ordinal 10045 -- pool alloc (pool ignored when pooling is off); failure reports type 2.
void* poolAllocChecked(void* pool, const char* file, int line) {
  void* p = g.h.poolAlloc(g.poolEnabled ? pool : nullptr);
  if (!p) fogError(2, file, line, "");
  return p;
}

// 1.13c FOG+0x1D7F0 ordinal 10090 -- close every slot event, free the table, disable async.
void asyncShutdown() {
  if (g.table) {
    for (uint32_t i = 0; i < g.tableCount; i++)
      if (g.table[i].event) { g.h.eventClose(g.table[i].event); g.table[i].event = nullptr; }
    g.h.poolFree(nullptr, g.table);
    g.table = nullptr; g.tableCount = 0;
  }
  g.enabled = 0;
}

// 1.13c FOG+0x1DB40 ordinal 10099 -- set request priority (<=0); deferred while deferCount>0.
void asyncSetPriority(Req* r, int32_t prio) {
  if (!r) fatal(0x1df);
  if ((r->flags & kMask) != kMagic) fatal(0x1e0);
  if (prio > 0) fatal(0x1e1);
  if (g.deferCount < 1) {
    if (prio != r->prio) { r->prio = prio; g.h.sPrio(r->buf, prio); }
  } else if (!(r->flags & 8)) {
    r->savedPrio = r->prio; r->prio = prio; r->flags |= 8; r->next = g.deferred; g.deferred = r;
  } else if (r->prio < prio) r->prio = prio;
}

// 1.13c FOG+0x1DCC0 ordinal 10091 -- open+start an async read. size<1 = whole file; buf null = pool-allocated;
// cb = 7-dword callback block or null; async/offset are the register/stack tail args (EBX, caller slot).
Req* asyncOpen(void* pool, const char* name, int32_t size, void* buf, const uint32_t* cb, void* ud,
               int async, uint32_t offset) {
  Req* r = (Req*)poolAllocChecked(g.poolEnabled ? pool : nullptr, kAsync, 0x12d);
  std::memset(r, 0, sizeof *r);
  r->flags = kMagic;
  if (!g.h.sOpen(name, &r->file)) { g.h.poolFree(nullptr, r); return nullptr; }
  r->pool = pool; r->prio = (int32_t)(intptr_t)ud;
  if (offset && g.h.sSeek(r->file, offset) == -1) fatal(0x147);
  if (size < 1) size = g.h.sSize(r->file, 0);
  r->size = size;
  if (!buf) { r->buf = g.h.poolAlloc(pool); r->flags |= 2; } else { r->buf = buf; r->flags &= ~2u; }
  uint32_t* cbp = nullptr;
  if (cb) { cbp = r->cb; std::memcpy(cbp, cb, sizeof r->cb); }
  if (async && g.enabled) {
    Slot* s = (Slot*)g.h.slotAcquire();
    r->slot = s;
    if (s) {
      s->req = r; r->ev = s->event; r->seek = offset;
      g.h.sRead(r->file, r->buf, r->size, r, ud, cbp);
      return r;
    }
    if (r->file) { g.h.sClose(r->file); r->file = nullptr; }
    if (r->flags & 2) g.h.poolFree(pool, r->buf);
    g.h.poolFree(pool, r);
    return nullptr;
  }
  g.h.sRead(r->file, r->buf, r->size, nullptr, ud, nullptr);
  if (cbp && cbp[0]) ((void (*)(void*, int32_t, uint32_t*))(uintptr_t)cbp[0])(r->buf, r->size, cbp);
  g.h.complete();
  return r;
}

// 1.13c FOG+0x1DF00 ordinal 10089 -- enable async: set Storm budgets, create 256 manual-reset slot events.
void asyncInit(int enable) {
  if (g.enabled) fatal(0x7b);
  g.h.sBudget(284, 0x40000); g.h.sBudget(285, 0x4000);
  if (!enable) return;
  g.table = (Slot*)g.h.poolAlloc(nullptr);
  if (!g.table) { fogError(2, kAsync, 0x88, ""); fatal(0x89); }
  for (int i = 0; i < 0x100; i++) { g.table[i].event = g.h.eventCreate(); g.table[i].busy = 1; }
  g.tableCount = 0x100; g.enabled = 1;
}

// 1.13c FOG+0x1E010 ordinal 10097 -- cancel/wait/free a request.
void asyncFree(Req* r) {
  if (!r) fatal(0x295);
  if ((r->flags & kMask) != kMagic) fatal(0x296);
  if (r->flags & 8)
    for (Req** pp = &g.deferred; *pp; pp = &(*pp)->next)
      if (*pp == r) { *pp = r->next; break; }
  if (g.h.poll(r) == 0) { g.h.sCancel(r->buf); g.h.eventWait(r->slot->event); g.h.complete(); }
  if (r->flags & 2) {
    if (!r->buf) fatal(0x2c8);
    g.h.poolFree(nullptr, r->buf); r->buf = nullptr; r->flags &= 0xf;
  }
  g.h.poolFree(nullptr, r);
}
}  // namespace fog_n80
