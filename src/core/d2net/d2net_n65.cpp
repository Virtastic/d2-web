// D2Net n65: DLL globals -> N65St, Win32/Fog/CRT calls -> N65Ops function-pointer seams.
#include <initializer_list>
#include <cstdint>
#include <cstring>
struct N65St {
  uint32_t sendOn /*b244*/, closing /*b41c*/, mode /*b418*/, sock /*b248*/;
  uint8_t *headA /*b3fc*/, *headB /*b3f4*/;
  void *thread /*b3f0*/, *startEvt /*b3f8*/, *sendQ /*b424*/, *worker; uint32_t tid /*b240*/;
  uint8_t *sendBuf /*b244 block, 0x7bc bytes*/;
};
struct N65Ops {
  void (*lock)(); void (*unlock)(); void* (*alloc)(uint32_t); void (*freeNode)(void*);
  uint32_t (*tick)(); uint32_t (*waitObj)(void*, uint32_t);
  void* (*createThread)(void* fn, void* arg, uint32_t* tid); void (*setPrio)(void*, int);
  uint32_t (*sizeOf)(const uint8_t*, uint32_t, uint32_t*); /* D2Net+0x6750 */
  int (*qpush)(void* q, const void* p, uint32_t n); uint32_t (*directSend)(const void*);
  int (*closeSock)(); void (*sockErr)(); void (*fatal)(uint32_t line); void (*initCs)(); void (*delCs)();
  void (*setSendOnly)(uint32_t); void (*sendFwd)(void* cs, uint8_t** head, void* out);
  void (*initFwd)(); void* workerFn; void* workerFn2;
};
// 1.13c D2NET+0x07650
extern "C" uint32_t d2net_n65_7650(N65St* s, const N65Ops* o, uint16_t len, const void* p) {
  if (!s->sendOn) return 0;
  if (s->mode != 1 && s->mode != 2) return o->directSend(p);
  if (len > 0x204) o->fatal(0x1c5);
  return o->qpush(s->sendQ, p, len) != 0;
}
// 1.13c D2NET+0x075E0
extern "C" uint32_t d2net_n65_75e0(N65St* s, const N65Ops* o) {
  if (s->mode != 1 && s->mode != 2) {
    if (o->waitObj(s->startEvt, 100) != 0) return 1;
    if (!s->sock) return 0;
    o->setSendOnly(1);
    s->thread = o->createThread(o->workerFn, nullptr, &s->tid);
    o->setPrio(s->thread, 1);
  }
  return 2;
}
// 1.13c D2NET+0x07540
extern "C" void* d2net_n65_7540(N65St* s, const N65Ops* o, int mode, void* arg) {
  s->headA = s->headB = nullptr;
  o->initCs();
  s->sendBuf = (uint8_t*)o->alloc(0xc2); memset(s->sendBuf, 0, 0x1ef * 4);
  s->mode = mode;
  if (mode != 1 && mode != 2) { s->startEvt = o->createThread(o->workerFn2, arg, &s->tid); return s->startEvt; }
  o->initFwd();
  return (void*)1;
}
// 1.13c D2NET+0x071E0
extern "C" void d2net_n65_71e0(N65St* s, const N65Ops* o, void* out) { o->sendFwd(nullptr, &s->headB, out); }
// 1.13c D2NET+0x06EF0
extern "C" void d2net_n65_6ef0(N65St* s, const N65Ops* o) {
  o->lock();
  if (s->sendBuf) o->freeNode(s->sendBuf);
  s->sendBuf = nullptr; s->sendOn = 0; s->closing = 1;
  o->unlock();
  o->waitObj(s->thread, 6000);
  for (uint8_t** h : {&s->headB, &s->headA}) {
    o->lock();
    while (*h) { uint8_t* n = *h; *h = *(uint8_t**)(n + 0x20c); o->freeNode(n); }
    o->unlock();
  }
  o->lock();
  o->delCs();
  if (s->mode != 1 && s->mode != 2) { int r = o->closeSock(); s->sock = 0; if (r == -1) o->sockErr(); }
}
// 1.13c D2NET+0x06BD0
extern "C" void d2net_n65_6bd0(N65St* s, const N65Ops* o, const uint8_t* p, int n) {
  while (n >= 1) {
    uint8_t* node = (uint8_t*)o->alloc(0x1fc);
    uint32_t sz;
    if (!o->sizeOf(p, n, &sz)) return;
    if (sz > 0x204) o->fatal(0x202);
    if ((int)sz < 1) o->fatal(0x203);
    memcpy(node, p, sz);
    *(uint32_t*)(node + 0x204) = sz;
    *(uint32_t*)(node + 0x208) = o->tick();
    *(uint32_t*)(node + 0x20c) = 0;
    uint8_t id = *p;
    uint8_t** list = id <= 0xae ? &s->headA : &s->headB;
    if (id > 0xb4) o->fatal(0x216);
    if (!*list) *list = node;
    else { uint8_t* t = *list; while (*(uint8_t**)(t + 0x20c)) t = *(uint8_t**)(t + 0x20c); *(uint8_t**)(t + 0x20c) = node; }
    n -= sz; p += sz;
  }
}
// 1.13c D2NET+0x043E0
extern "C" char* d2net_n65_43e0(char* d, const char* s) { return strcpy(d, s); }
// 1.13c D2NET+0x043F0
extern "C" char* d2net_n65_43f0(char* d, const char* s) { return strcat(d, s); }
