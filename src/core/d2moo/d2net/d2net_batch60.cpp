// D2Net batch: queue lifecycle / send dispatch. Globals and imports are injected (pure functions of inputs).
#include <initializer_list>
#include <cstdint>
struct D2NetSt { uint32_t sendOn /*b244*/, closing /*b41c*/, mode /*b418*/, sock /*b248*/; };
struct D2NetOps {
  void (*lock)(); void (*unlock)(); void (*freeNode)(); uint32_t (*waitWorker)(); uint32_t (*waitStart)();
  void (*stopSock)(); int (*closeSock)(); void (*sockErr)(); void (*setSendOnly)(uint32_t);
  void (*startThread)(); uint32_t (*send)(uint32_t); int (*qpush)(const void*, uint32_t); void (*fatal)();
};
// 1.13c D2Net+0x6EF0: shutdown. Frees both packet lists (next ptr at +0x20c) under the lock, closes the socket unless mode 1/2.
extern "C" void d2net_sub_6ef0(D2NetSt* s, uint32_t** headA, uint32_t** headB, const D2NetOps* o) {
  o->lock();
  if (s->sendOn) o->stopSock();
  s->sendOn = 0; s->closing = 1;
  o->unlock();
  o->waitWorker();
  for (uint32_t** h : {headA, headB}) {
    o->lock();
    while (*h) { *h = *(uint32_t**)((char*)*h + 0x20c); o->freeNode(); }
    o->unlock();
  }
  o->lock();
  if (s->mode != 1 && s->mode != 2) { int r = o->closeSock(); s->sock = 0; if (r == -1) o->sockErr(); }
}
// 1.13c D2Net+0x75E0: start the worker. 1 = wait timed out, 0 = no socket, 2 = started (or mode 1/2).
extern "C" uint32_t d2net_sub_75e0(D2NetSt* s, const D2NetOps* o) {
  if (s->mode != 1 && s->mode != 2) {
    if (o->waitStart() != 0) return 1;
    if (s->sock == 0) return 0;
    o->setSendOnly(1); o->startThread();
  }
  return 2;
}
// 1.13c D2Net+0x7650: send dispatch. Oversize (>0x204) in mode 1/2 is fatal (_exit).
extern "C" uint32_t d2net_sub_7650(D2NetSt* s, uint16_t len, const void* data, uint32_t arg, const D2NetOps* o) {
  if (!s->sendOn) return 0;
  if (s->mode != 1 && s->mode != 2) return o->send(arg);
  if (len > 0x204) { o->fatal(); return 0; }
  return o->qpush(data, len) != 0;
}
