// D2Net batch b59-net-a.
#include <cstdint>
// 1.13c D2Net+0x71E0: forwards the packet to sub_6AC0 with the lock (DAT_6fbfb400) and queue head (DAT_6fbfb3f4) (`fn` = sub_6AC0).
extern "C" void d2net_sub_71e0(void* lock, void* head, uint32_t* pkt, void (*fn)(void*, void*, uint32_t*)) { fn(lock, head, pkt); }
