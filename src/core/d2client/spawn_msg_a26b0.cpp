// src/core/d2client/spawn_msg_a26b0.cpp -- 1.13c D2Client+0xA26B0 (alpha/mapping lane, env-seam style).
// Builds a 23-word spawn descriptor positioned at a unit (+ offsets) and dispatches it via 0xB30F0.
#include <cstdint>
#include <cstddef>

struct SpawnMsgA26b0Env {
    uint32_t (*unitType)(uint32_t unit);              // *unit
    uint32_t (*unitPathPtr)(uint32_t unit);           // unit[0xb]
    int32_t  (*pathXField)(uint32_t path);            // *(path+0xc)
    int32_t  (*pathYField)(uint32_t path);            // *(path+0x10)
    int32_t  (*ord10867)(uint32_t path);              // path x
    int32_t  (*ord10750)(uint32_t path);              // path y
    int32_t  (*getClientCtx)(int32_t* out);           // 0xA1340 (out[0], out[1])
    uint32_t (*f630f0)(uint32_t* msg);                // 0xB30F0
};

struct SpawnMsgA26b0 { uint32_t w[23]; };
static_assert(sizeof(SpawnMsgA26b0) == 0x5c, "23 words");

static int32_t pos(const SpawnMsgA26b0Env& e, uint32_t unit, bool y) {
    uint32_t t = e.unitType(unit), p = e.unitPathPtr(unit);
    if (t == 2 || (t > 3 && t < 6)) return y ? e.pathYField(p) : e.pathXField(p);
    if (p == 0) return 0;
    return y ? e.ord10750(p) : e.ord10867(p);
}

// unit = EAX, esi = ESI (register args), p1..p3 / dx,dy / ctx1 = stack args in decompile order.
// Returns 0 or the 0xB30F0 result.
// 1.13c D2Client+0xA26B0
uint32_t d2_client_a26b0(const SpawnMsgA26b0Env& e, uint32_t unit, uint32_t esi,
                         uint32_t p1, uint32_t p2, uint32_t p3, int32_t dx, int32_t dy, uint32_t ctx1) {
    SpawnMsgA26b0 m = {};
    m.w[0] = 0x21;
    m.w[4] = p1;
    m.w[5] = (uint32_t)(pos(e, unit, false) + dx);
    m.w[6] = (uint32_t)(pos(e, unit, true) + dy);
    m.w[10] = p2;
    m.w[11] = p3;
    int32_t pair[2];
    if (esi == 0 || ctx1 == 0) {
        if (!e.getClientCtx(pair)) return 0;
    } else { pair[0] = (int32_t)esi; pair[1] = (int32_t)ctx1; }
    m.w[7] = (uint32_t)pair[0]; m.w[8] = (uint32_t)pair[1];
    if (pair[0] != 0 && pair[1] != 0) return e.f630f0(m.w);
    return 0;
}
