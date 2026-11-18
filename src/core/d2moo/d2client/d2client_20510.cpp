// 1.13c D2Client+0x20510: refresh the palette/cell node of a client object. thiscall(ECX = obj, [esp+4] = flag), ret 4.
// obj+0x00 kind, +0x04 key (passed to the lookup, EAX), +0x14 link to a record (flags byte at +0x16), +0x20/+0x24 a 64-bit
// multiplicative generator state, +0x64 node, +0x68 node size >> 3. Links are 32-bit retail addresses.
#include <cstring>
#include "d2client_callee.h"
namespace {
constexpr uint32_t kAlloc = 0xa8c20, kRelease = 0xa8630, kLookup = 0x1260, kStrId = 0xbf92, kMsg = 0xbf80, kExit = 0x3664;
constexpr uint32_t kMul = 0x6ac690c5u;
inline uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t x; std::memcpy(&x, p + o, 4); return x; }
inline void wr(uint8_t* p, uint32_t o, uint32_t x) { std::memcpy(p + o, &x, 4); }
}
// 1.13c D2Client+0x20510
extern "C" void d2c_20510(const D2cSvc& v, uint8_t* obj, uint32_t flag) {
    if (!flag) return;
    if (const uint32_t node = rd(obj, 0x64)) {
        d2c_call(v, kRelease, 0, 0, 0, 0, node);
        wr(obj, 0x64, 0);
    }
    uint32_t node;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)rd(obj, 0x14);
    if (rd(obj, 0) == 1 && rec && (rec[0x16] & 4)) {
        // three steps of x = x * kMul + carry over the pair (lo = +0x20, hi = +0x24); the low bytes of the three lo values feed the lookup
        uint32_t lo = rd(obj, 0x20), hi = rd(obj, 0x24), b[3];
        for (int i = 0; i < 3; ++i) {
            const uint64_t p = (uint64_t)lo * kMul + hi;
            lo = (uint32_t)p; hi = (uint32_t)(p >> 32); b[i] = lo & 0xff;
        }
        wr(obj, 0x20, lo); wr(obj, 0x24, hi);
        node = d2c_call(v, kAlloc, 0, 0, 0, 0, 0, 7, 0xff, b[2], b[1], b[0]);
    } else {
        const uint8_t* q = (const uint8_t*)(uintptr_t)d2c_call(v, kLookup, rd(obj, 4), 0, 0, 0, 0);
        if (!q) return;
        node = d2c_call(v, kAlloc, 0, 0, 0, 0, 0, 7, 0xff, q[0x11f], q[0x120], q[0x121]);
    }
    wr(obj, 0x64, node);
    if (!node) {                                           // retail shows an error box and exits the process
        d2c_call(v, kMsg, 0, 0, 0, 0, 0, 0xd359f, d2c_call(v, kStrId, 0, 0, 0, 0, 0, 0x155));
        d2c_call(v, kExit, 0, 0, 0, 0, 0, 0xffffffffu);
        return;
    }
    wr(obj, 0x68, (uint32_t)((int32_t)rd((const uint8_t*)(uintptr_t)node, 0x18) >> 3));
}
