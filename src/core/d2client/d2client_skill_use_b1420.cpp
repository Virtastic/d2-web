// src/core/d2client/d2client_skill_use_b1420.cpp -- 1.13c D2Client+0xB1420 (768 bytes), the skill
// cast/use gate: validates unit `this` (ECX) against target EAX using the skills.txt record
// (0x1a4 stride), runs the per-skill precheck hook, starts the cast. Mapping lane: every callee,
// import ordinal and global is a host seam; ordinals are UNVERIFIED names. Spec
// Decompile registers: EAX = target, EBX = flags (bit0).
#include <cstdint>
#include <emscripten.h>

struct D2ClientSkillUseEnv {
    uint32_t (*rd32)(uintptr_t a);
    uint8_t  (*rd8)(uintptr_t a);
    uint16_t (*rd16)(uintptr_t a);
    uintptr_t dt;                    // *sgptDataTables
    uint32_t bitmask8;               // gdwBitMasks[8]
    uint32_t hook_count;             // DAT_6fb8db54
    // 0..: per-skill precheck table DAT_6fb8da10[skill]; returns 0 to abort
    uint32_t (*hook)(uint32_t skill);
    uint32_t (*hook_present)(uint32_t skill);
    // generic seam keyed by callee: RVA (0x...) or 0x10000+ordinal; ret value only
    uint32_t (*call)(uint32_t id, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);
};

// 1.13c D2Client+0xB1420
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_b1420(uintptr_t self, int32_t param_1, uintptr_t target, uint32_t flags,
                         const D2ClientSkillUseEnv* e) {
    uintptr_t rec = 0;
    int32_t idx = self ? (int32_t)e->rd32(self + 4) : -1;
    if (self && idx >= 0 && idx < (int32_t)e->rd32(e->dt + 0xb6c))
        rec = e->rd32(e->dt + 0xb64) + (uintptr_t)idx * 0x1a4;
    int16_t s16 = (int16_t)e->rd16(rec + 0x16);
    uint32_t local = 3;
    uintptr_t p4 = e->call(0xA60D0, 0, 0, 0, 0, 0);          // current player/owner
    if (!self) return 0;
    auto C = [&](uint32_t id, uintptr_t a = 0, uintptr_t b = 0, uintptr_t c = 0, uintptr_t d = 0,
                 uintptr_t f = 0) { return e->call(id, a, b, c, d, f); };
    bool to_skip = false;
    if (target) {
        if (e->rd8(rec + 0x188) && C(0x10000 + 10494, self)) return 0;
        if (C(0x10000 + 10227, self)) return 0;
        C(0x10000 + 10024, self);
        if (!((e->rd32(target + 0xC4) >> 2) & 1)) return 0;
        if (p4) {
            if (!C(0xA62A0 + 0, p4) && !e->rd8(rec + 0x187)) return 0;  // FUN_6fb562a0 (unrecovered id)
        }
    }
    if (p4 && target) {
        if ((e->bitmask8 & e->rd8(rec + 4)) && (int32_t)e->rd32(p4) >= 0 && (int32_t)e->rd32(p4) < 2 &&
            (C(0x10000 + 10494, p4, 0x45) || C(0x10000 + 10910, p4, 0x9c, 0)))
            local = C(0xB0100);
        to_skip = true;
    } else if (!p4) to_skip = true;
    if (!target) {
        if (!param_1 && !e->rd8(rec + 0x18c)) goto tail;
    }
    (void)to_skip;
    if (local & 2) {
        if (e->rd8(rec + 0x188) && target) {
            uint32_t o = C(0x10000 + 11013, 0, 2, e->rd8(rec + 0x189), e->rd32(target), e->rd32(target + 12));
            if (o) { C(0x10000 + 10785, o, 0x56); C(0x10000 + 10911, o, 0xB5F70); C(0x10000 + 10807); C(0x10000 + 10945); }
        }
        C(0xAD1400, self, 0);
        int16_t sk = (int16_t)e->rd16(rec + 10);
        if (sk > 0 && sk < (int32_t)e->hook_count && e->hook_present(sk) && e->hook(sk) == 0) return 0;
        if ((int16_t)e->rd16(rec + 0x14) >= 0) C(0xB38A70, self, 0, 0, 0);
        if ((int16_t)e->rd16(rec + 0x16) >= 0 && s16 != -1) {
            uint32_t u = C(0x10000 + 11029, self);
            C(0x10000 + 10240, self, u);
            uint32_t r = C(0xB0450);
            if (r) {
                C(0xB77EC0); C(0xB77EE0); uint32_t k = C(0xB77F00); C(0xB77E30, 0, 0, k);
                C(0xB77C30); C(0xB77BF0);
                uint32_t a, b = e->rd32(r + 0x2c);
                a = e->rd32(r + 4) == 0x92 ? C(0xAB1330, r + 0x20) : C(0x10000 + 10042, e->rd32(self + 0x2c));
                C(0x10000 + 10425, b, a);
            }
        }
    }
tail:
    if (target && !e->rd8(rec + 0x186)) return 0;
    if ((int16_t)e->rd16(rec + 0x12) >= 0 && C(0xAD25D0, self)) C(0xB38C20, self, 0);
    if (C(0x10000 + 10398, self)) C(0xB585F0);
    if (flags & 1) {
        if ((e->rd32(self + 0xc4) >> 0x15) & 1) { C(0xB57340, e->rd32(self), e->rd32(self + 0xc)); return local; }
        C(0xB55B00, e->rd32(self), e->rd32(self + 0xc));
    }
    return local;
}
