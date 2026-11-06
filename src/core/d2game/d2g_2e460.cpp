// 1.13c D2Game+0x2E460 GAME_ProcessNetworkMessages (export 10040).
// Drains the three server message queues into one 0x200-byte stack buffer. Retail call shapes (disassembly):
// queue 0 (thunk 0xA370, D2Net) then 0xCAF30(buf) stdcall, the size is not passed; queue 1 (thunk 0xA376) then
// 0xCAE80(EAX = buf, [esp] = size, ret 4); queue 2 (thunk 0xA36A) then 0x29CB0(EAX = buf), plain ret.
#ifndef D2G_2E460_CPP
#define D2G_2E460_CPP
#include <cstdint>
namespace d2g_2e460 {
struct Env {
    int32_t (*read0)(void* buf, int32_t cap);   // 0xA370
    int32_t (*read1)(void* buf, int32_t cap);   // 0xA376
    int32_t (*read2)(void* buf, int32_t cap);   // 0xA36A
    void (*sysMsg)(void* buf);                  // 0xCAF30
    void (*clientMsg)(void* buf, int32_t n);    // 0xCAE80
    void (*msg2)(void* buf);                    // 0x29CB0
};
// 1.13c D2Game+0x2E460
inline void GAME_ProcessNetworkMessages_113c(const Env& e) {
    uint32_t buf[128];
    while (e.read0(buf, 0x200) != -1) e.sysMsg(buf);
    for (;;) { int32_t n = e.read1(buf, 0x200); if (n == -1) break; e.clientMsg(buf, n); }
    while (e.read2(buf, 0x200) != -1) e.msg2(buf);
}
}
#endif
