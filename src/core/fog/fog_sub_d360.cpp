// Fog+0x0D360 QServer: pull one packet from the receive callback and queue it. Decompile-derived.
#include <cstdint>
#include <cstring>
extern "C" {
void FUN_6ff61690(void*);
void* FUN_6ff6b560(int*);
void FUN_6ff60f30(int, const char*, int, const char*);
extern const char DAT_6ff768ec[];
void EnterCriticalSection(void*);
void LeaveCriticalSection(void*);
uint32_t GetTickCount(void);
}
static const char kQS[] = "..\\Source\\Fog\\Src\\QServer\\QServer.cpp";

// 1.13c Fog+0x0D360  (stdcall 4 args: server, p2, p3, p4; p2 = buffer base, p3 = buffer end, p4 = packet tag)
extern "C" uint32_t Fog_D360(uint8_t* srv, uint32_t p2, uint32_t p3, uint32_t p4) {
  // cb(fastcall ecx=p2, edx=p3; &d, &c, &b, &a, 0, 0) -> 1 or 2 on success
  uint32_t a = 0, b = 0, c = 0, d = 0;
  typedef int (*Cb)(uint32_t, uint32_t, uint32_t*, uint32_t*, uint32_t*, uint32_t*, int, int);
  int r = (*(Cb*)(srv + 0xbc0))(p2, p3, &d, &c, &b, &a, 0, 0);
  if (r != 2 && r != 1) return 0;
  uint8_t* pkt;
  void* cs = srv + 0x854;
  if (*(int*)(srv + 0x878) == 0) {
    pkt = (uint8_t*)FUN_6ff6b560(nullptr);
    if (!pkt) FUN_6ff60f30(2, kQS, 0xa0c, DAT_6ff768ec);
  } else {
    FUN_6ff61690(cs);
    EnterCriticalSection(cs);
    pkt = (uint8_t*)(intptr_t) * (uint32_t*)(srv + 0x878);
    if (!pkt) {
      pkt = (uint8_t*)FUN_6ff6b560(nullptr);
      if (!pkt) FUN_6ff60f30(2, kQS, 0xa03, DAT_6ff768ec);
    } else {
      *(uint32_t*)(srv + 0x878) = *(uint32_t*)(pkt + 0x5bc);
    }
    LeaveCriticalSection(cs);
  }
  uint32_t len = p3 - c;
  memcpy(pkt + 4, (const uint8_t*)(intptr_t)(c + p2), len);
  *(uint32_t*)(pkt + 0x5b4) = len;
  *(uint32_t*)(pkt + 0x5b8) = GetTickCount();
  *(uint32_t*)(pkt + 0x5bc) = 0;
  *(uint32_t*)pkt = p4;
  if ((int)b < *(int*)(srv + 0x87c)) {
    FUN_6ff61690(cs);
    EnterCriticalSection(cs);
    uint32_t tail = ((uint32_t*)(intptr_t) * (uint32_t*)(srv + 0x874))[b];
    if (!tail) ((uint32_t**)(intptr_t) * (uint32_t*)(srv + 0x870))[b] = (uint32_t*)pkt;
    else *(uint32_t*)(intptr_t)(tail + 0x5bc) = (uint32_t)(intptr_t)pkt;
    ((uint32_t*)(intptr_t) * (uint32_t*)(srv + 0x874))[b] = (uint32_t)(intptr_t)pkt;
    LeaveCriticalSection(cs);
  }
  return 1;
}
