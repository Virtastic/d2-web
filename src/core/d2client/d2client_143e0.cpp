// 1.13c D2Client+0x143E0 (235 B): outgoing-packet send with a duplicate window. EBX = length (signed; >= 0x200 reports string 0x26D and
// exits), one stack word = the packet. The retail convention is not a plain C one; this is the rule on explicit state.
//   now = GetTickCount(). The first byte t picks the window (table at 0x144D8 over t-5, unsigned): ':' (0x3A) none; t in 0x0B or 0x12..0x39
//   or outside 5..0x3A -> 200 ms; t in 5..0x0A or 0x0C..0x11 -> 50 ms. Inside the window (now - lastTick < win, unsigned) and the first
//   `len` bytes equal the stored last packet -> drop. Else copy the packet to the last-packet store, lastTick = now, send(len, 1, copy);
//   a nonzero result adds len to the byte counter and bumps the packet counter.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Client143e0State { uint32_t lastTick; uint8_t last[0x200]; uint32_t bytes; uint32_t count; };
struct D2Client143e0Env {
    uint32_t now;                                                       // GetTickCount()
    D2Client143e0State* st;                                            // 0x1234C8 / 0x1234D0 / 0x11C4E0 / 0x11C4E8
    void* ctx;
    uint32_t (*send)(void* ctx, uint32_t len, const uint8_t* copy);   // D2Net Ordinal 10024 through the thunk 0xD252 (stack len, 1, copy)
    void (*fatal)(void* ctx, uint32_t strId);                          // string id 0x26D, then _exit(-1)
};

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_143e0(const uint8_t* buf, int32_t len, const D2Client143e0Env* env) {
    D2Client143e0State& s = *env->st;
    if (len >= 0x200) { env->fatal(env->ctx, 0x26D); return; }
    const uint32_t t = (uint32_t)buf[0] - 5u;
    uint32_t win;
    if (t > 0x35u) win = 200;
    else if (t == 0x35u) win = 0;                                   // ':'
    else if (t == 6u || t >= 13u) win = 200;
    else win = 50;
    if (win && env->now - s.lastTick < win && std::memcmp(s.last, buf, (uint32_t)len) == 0) return;
    std::memcpy(s.last, buf, (uint32_t)len);
    s.lastTick = env->now;
    if (env->send(env->ctx, (uint32_t)len, buf)) { s.bytes += (uint32_t)len; s.count++; }
}
