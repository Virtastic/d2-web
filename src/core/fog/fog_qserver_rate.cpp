// QServer rate/hacklist helpers, decompile-derived from Fog 1.13c. srv = QServer object (32-bit field offsets).
#include <cstdint>
extern "C" {
void FUN_6ff61690(void*);
void* FUN_6ff6b560(int*);
void FUN_6ff6b0c0(void*);
void FUN_6ff60f30(int, const char*, int, const char*);
void FUN_6ff690a0(const void*, const char*);
extern const char DAT_6ff768ec[];
extern const char DAT_6ff77040[];
extern uint32_t DAT_6ffa6ed4;
void EnterCriticalSection(void*);
void LeaveCriticalSection(void*);
uint32_t GetTickCount(void);
}
#define U32(p, o) (*(uint32_t*)((uint8_t*)(p) + (o)))
#define NODE(h) ((uint8_t*)(intptr_t)(h))

// 1.13c Fog+0x0D640 -- drop rate-list nodes (head +0x84c, next +0xc, stamp +4) older than 15 s
extern "C" uint32_t Fog_D640(uint8_t* srv) {
  uint32_t now = GetTickCount();
  void* cs = srv + 0x854;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  while (U32(srv, 0x84c) && now - U32(NODE(U32(srv, 0x84c)), 4) > 15000) {
    uint8_t* n = NODE(U32(srv, 0x84c));
    U32(srv, 0x84c) = U32(n, 0xc);
    FUN_6ff6b0c0(&n);
  }
  uint32_t prev = U32(srv, 0x84c);
  if (prev) {
    uint32_t cur = U32(NODE(prev), 0xc);
    while (cur) {
      uint8_t* n = NODE(cur);
      if (now - U32(n, 4) < 0x3a99) {
        prev = cur;
        cur = U32(n, 0xc);
      } else {
        U32(NODE(prev), 0xc) = U32(n, 0xc);
        FUN_6ff6b0c0(&n);
        cur = U32(NODE(prev), 0xc);
      }
    }
  }
  LeaveCriticalSection(cs);
  return 1;
}

// 1.13c Fog+0x0DBE0 -- per-key rate limit; 0 = over limit, 1 = ok
extern "C" uint32_t Fog_DBE0(uint8_t* srv, uint32_t* key) {
  int lim = (int)U32(srv, 0x884);
  if (lim <= 0) return 1;
  uint32_t k = *key, now = GetTickCount();
  Fog_D640(srv);
  void* cs = srv + 0x854;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  for (uint32_t h = U32(srv, 0x84c); h; h = U32(NODE(h), 0xc)) {
    uint8_t* n = NODE(h);
    if (U32(n, 0) != k) continue;
    if (now - U32(n, 4) < 15000) {
      uint32_t c = ++U32(n, 8);
      if ((uint32_t)lim * 2 + 3 < c) {
        LeaveCriticalSection(cs);
        return 0;
      }
    } else {
      U32(n, 4) = now;
      U32(n, 8) = 1;
    }
  }
  uint8_t* n = (uint8_t*)FUN_6ff6b560(nullptr);
  if (!n) FUN_6ff60f30(2, "..\\Source\\Fog\\Src\\QServer\\QServer.cpp", 0x54f, DAT_6ff768ec);
  U32(n, 0) = k;
  U32(n, 4) = now;
  U32(n, 8) = 1;
  U32(n, 0xc) = 0;
  if (!U32(srv, 0x84c)) U32(srv, 0x84c) = (uint32_t)(intptr_t)n;
  else U32(NODE(U32(srv, 0x850)), 0xc) = (uint32_t)(intptr_t)n;
  U32(srv, 0x850) = (uint32_t)(intptr_t)n;
  LeaveCriticalSection(cs);
  return 1;
}

// 1.13c Fog+0x0DD10 -- true if key's entry (hash +0x40, buckets +0x10, next +8, count +4) reached the limit
extern "C" bool Fog_DD10(uint8_t* srv, uint32_t* key) {
  int lim = (int)U32(srv, 0x884);
  if (lim < 0) return false;
  uint32_t k = *key, nb = U32(srv, 0x10);
  void* cs = srv + 0x854;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  uint32_t* tab = (uint32_t*)(intptr_t)U32(srv, 0x40);
  uint32_t h = tab[k % nb];
  while (h && U32(NODE(h), 0) != k) h = U32(NODE(h), 8);
  if (!h) {
    LeaveCriticalSection(cs);
    return false;
  }
  int cnt = (int)U32(NODE(h), 4);
  LeaveCriticalSection(cs);
  return lim <= cnt;
}

// 1.13c Fog+0x0DD90 -- hacklist (hash +0x8ac, buckets +0x8b0, next +0xc, ttl +8): refresh ttl to 120
extern "C" uint32_t Fog_DD90(uint8_t* srv, uint32_t key) {
  void* cs = srv + 0x894;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  uint32_t* tab = (uint32_t*)(intptr_t)U32(srv, 0x8ac);
  uint32_t h = tab[key % U32(srv, 0x8b0)];
  uint32_t r = 0;
  while (h) {
    uint8_t* n = NODE(h);
    if (U32(n, 0) == key) {
      if (U32(n, 8)) {
        U32(n, 8) = 0x78;
        r = 1;
      }
      break;
    }
    h = U32(n, 0xc);
  }
  LeaveCriticalSection(cs);
  return r;
}

// 1.13c Fog+0x0E010 -- hacklist tick (once per second): age ttl(+8), ban timer(+4); expired nodes go to free list +0x88c
extern "C" void Fog_E010(uint8_t* srv) {
  if (U32(srv, 0x888) == 0xffffffffu) U32(srv, 0x888) = GetTickCount();
  uint32_t now = GetTickCount();
  uint32_t dt = now - U32(srv, 0x888);
  if (dt < 1000) return;
  dt /= 1000;
  bool freed = false;
  void* cs = srv + 0x894;
  FUN_6ff61690(cs);
  EnterCriticalSection(cs);
  U32(srv, 0x888) = now;
  uint32_t* tab = (uint32_t*)(intptr_t)U32(srv, 0x8ac);
  for (uint32_t i = 0; i < U32(srv, 0x8b0); i++) {
    uint32_t cur = tab[i], prev = 0;
    while (cur) {
      uint8_t* n = NODE(cur);
      uint32_t next = U32(n, 0xc), t = U32(n, 8);
      if (t) {
        if (dt < t) U32(n, 8) = t - dt;
        else {
          U32(n, 8) = 0;
          U32(n, 4) = dt < 0x78 ? 0x78 - dt : 0;
          U32(n, 4) += dt;
        }
      }
      t = U32(n, 4);
      if (t && dt >= t) {  // expired: unlink, push on free list
        if (!prev) tab[i] = next;
        else U32(NODE(prev), 0xc) = next;
        U32(n, 0xc) = U32(srv, 0x88c);
        U32(srv, 0x88c) = cur;
        U32(srv, 0x890)--;
        freed = true;
      } else {
        if (t) U32(n, 4) = t - dt;
        prev = cur;
      }
      cur = next;
    }
  }
  LeaveCriticalSection(cs);
  if (freed) {
    uint32_t t = GetTickCount();
    if (t - DAT_6ffa6ed4 > 2000) {
      FUN_6ff690a0(DAT_6ff77040, "[HACKLIST]  Hacklist size is %d");
      DAT_6ffa6ed4 = t;
    }
  }
}
