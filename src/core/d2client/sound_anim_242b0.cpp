// 1.13c D2Client+0x242B0: per-frame unit sound/anim-start trigger.
// EAX = flag, ECX = pUnit, [esp+4] = mode (4 selects the first record triple), ret 4. Decision logic
// only; every callee runs through cb(op, a, b, c), op order below. Globals arrive in the env.
#include <cstdint>

enum : uint32_t {
    D2C242B0_TYPE = 0,      // 0x22430 ESI=unit -> unit type
    D2C242B0_CVT = 1,       // 0x0C7A8 ECX=unit -> D2Common #10862 value
    D2C242B0_ERRSTR = 2,    // 0x0BF92 a=code -> string
    D2C242B0_ERRLOG = 3,    // 0x0BF80 a=file string RVA (0xD359F), b=message
    D2C242B0_EXIT = 4,      // 0x03664 a=-1 (retail never returns)
    D2C242B0_SPEED = 5,     // 0x22220 ECX=unit
    D2C242B0_SNDPLAY = 6,   // 0x88A70 EBX=a(sound id), unit=b, offset=c
    D2C242B0_PROFILE = 7,   // 0x22A70 EAX=unit -> MonSounds record (13 dwords) or 0
    D2C242B0_BUSY = 8,      // 0x87BD0 EBX=unit
    D2C242B0_LIST = 9,      // 0x23A30 ECX=a(100), unit=b, 1
    D2C242B0_STAMP = 10,    // 0x22070 EAX=unit, a=tick
    D2C242B0_SETVAL = 11,   // 0x87ED0 EDX=a(handle), b=value
};

struct D2C242B0Env {
    uint32_t tick;          // D2Client+0x1197F8
    uint32_t lastTick;      // D2Client+0x11C4C4 (in/out)
    uint32_t seedLo, seedHi;// [D2Client+0x11BBFC]+0x20/+0x24 (in/out)
    const uint32_t* cvtTable; // D2Client+0xF5538, 14 entries of 2 dwords; entry[0] read
    int16_t animSpeed;      // unit+0x4C
    uint32_t stamp;         // unit+0x7C (out)
};

typedef uintptr_t (*D2C242B0Cb)(uint32_t op, uintptr_t a, uintptr_t b, uintptr_t c);

// 1.13c D2Client+0x242B0
extern "C" void d2_client_242b0(D2C242B0Env* e, uint32_t unit, uint32_t flag, uint32_t mode, D2C242B0Cb cb)
{
    const uint32_t type = (uint32_t)cb(D2C242B0_TYPE, unit, 0, 0);
    if (type == 0) {
        uint32_t code = 0xb5c;
        const uint32_t k = (uint32_t)cb(D2C242B0_CVT, unit, 0, 0);
        if (unit) {
            if ((k & 0xf) < 0xe) {
                const uint32_t id = e->cvtTable[(k & 0xf) * 2];
                uint32_t off = (uint32_t)cb(D2C242B0_SPEED, unit, 0, 0);
                if (!flag) off = 0;
                cb(D2C242B0_SNDPLAY, id, unit, off);
                e->stamp = e->tick;
                return;
            }
            code = 0xb60;
        }
        const uintptr_t s = cb(D2C242B0_ERRSTR, code, 0, 0);
        cb(D2C242B0_ERRLOG, 0xD359F, s, 0);
        cb(D2C242B0_EXIT, 0xffffffffu, 0, 0);
        return;     // retail does not return from _exit; the capture hook does (spec)
    }
    if (type != 1) return;
    const uint32_t* r = (const uint32_t*)cb(D2C242B0_PROFILE, unit, 0, 0);
    if (!r) return;
    const bool m4 = mode == 4;
    const uint32_t a = m4 ? r[1] : r[7];
    uint32_t b = m4 ? r[2] : r[8];
    const uint32_t chance = m4 ? r[3] : r[9];
    if (!flag) b = 0;
    bool roll = a != 0;
    bool skip = false;
    if (chance < 100) {
        if (e->tick - e->lastTick < 15) roll = false;
        skip = cb(D2C242B0_BUSY, unit, 0, 0) != 0;
    }
    if (!skip && roll) {
        const uint64_t p = (uint64_t)e->seedLo * 0x6ac690c5u + e->seedHi;
        e->seedLo = (uint32_t)p; e->seedHi = (uint32_t)(p >> 32);
        if ((int32_t)((uint32_t)p % 100) < (int32_t)chance) {
            cb(D2C242B0_LIST, 100, unit, 0);
            cb(D2C242B0_SNDPLAY, a, unit, b);
            cb(D2C242B0_STAMP, unit, e->tick, 0);
            e->lastTick = e->tick;
        }
    }
    uint32_t div = (uint32_t)(int32_t)e->animSpeed;
    if (!div) div = 1;
    const uint32_t id = m4 ? r[4] : r[10];
    const uint32_t off = ((m4 ? r[5] : r[11]) * 256u + 0x80u) / div;
    const uint32_t val = m4 ? r[6] : r[12];
    const uint32_t h = (uint32_t)cb(D2C242B0_SNDPLAY, id, unit, off);
    if (h) cb(D2C242B0_SETVAL, h, val, 0);
}
