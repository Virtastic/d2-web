// src/core/d2game/d2game_batch_43450.cpp -- twelve 1.13c D2Game.dll functions (RE-DONE, alpha lane)
// transcribed from the cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), image base 0x6FC20000.
// Missile/skill-effect helpers 0x43450..0x463A0 plus CLIENTS_GetExpansionClientCount (0x4A030). Ghidra's
// register-only arguments (in_EAX/unaff_*) are explicit parameters, in the order the comments reads them.
// Every callee (Fog/D2Common ordinals and unmapped D2Game RVAs) is a seam on D2GameBatch43450Env; where a
// callee's register convention could not be pinned from the decompile alone, the seam takes the visible
// operands and the gap is noted at the call ("seam:"). sgptDataTables is the dword table base (+0xB6C
// missile count, +0xB98/+0xBA0 skill table/count stride 0x23C, +0xA78/+0xA80 stride 0x1A8, +0xA90/+0xA98
// stride 0x134, +0xC4 limit).
#include <cstdint>
#include <cstring>

namespace {
inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
inline int16_t  rds16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
}

struct D2GameBatch43450Env {
    const uint8_t* dataTables;          // *sgptDataTables_exref
    // Fog/D2Common ordinals (by number)
    void*    (*o10331)(void* unit);                       // unit -> skill-ish record
    uint32_t (*o10826)(void* p);
    uint8_t* (*o10014)(uint32_t id);
    int      (*o10686)(void* unit, uint32_t ebp, uint32_t esi, uint32_t flags);
    void*    (*o10392)(uint32_t id);                      // unit lookup by id
    void     (*o10786)(void* unit, uint32_t a, uint32_t b);
    void     (*o10637)(void* missile, uint32_t v);
    void     (*o10019)(void* missile, uint32_t v);
    void*    (*o10203)(void* unit);
    void*    (*o10871)(void* unit, int a);
    int      (*o10680)(void* p);
    uint32_t (*o10057)(void* p);
    int      (*o10983)(uint32_t id);
    int      (*o10764)(uint32_t id);
    uint32_t (*o10867)(uint32_t id);
    uint32_t (*o10750)(uint32_t id);
    uint32_t (*o10945)(void* unit, int a, int b);
    void*    (*o11013)(uint32_t a, int b, uint32_t c, uint32_t d, uint32_t e);
    void     (*o10785)(void* p, int v);
    void     (*o10911)(void* p);
    void     (*o10807)(void* a, void* b, int c);
    void     (*o11043)(void* p, uint32_t v);
    uint32_t (*o10653)();
    // D2Game unmapped / mapped RVAs
    int      (*f9FA30)(void* game, int* out);
    uint32_t (*fE19C0_d019c0)(uint32_t ebp, uint32_t z, void* game, void* unit, uint32_t z2); // 0xD019C0
    void*    (*f_d003a0)(void* game, uint32_t id);        // 0xE03A0
    uint32_t (*f_d00a60)(void* a, void* game);            // 0xE0A60
    void*    (*f21420)(void* p);
    uint8_t* (*f21250)(void* p);
    uint32_t (*f21100)(void* unit);                       // 0x1100 unit x
    uint32_t (*f21130)(void* unit);                       // 0x1130 unit y
    uint32_t (*f21280)();
    void*    (*f8F930)(void* game, uint32_t* params);     // 0x6F930 CreateMissile
    void*    (*fCC28F0)(void* game, uint32_t a, void* unit, uint32_t b, uint32_t c, int d, int e, int f, int g, int h);
    void     (*fD007A0)(void* game);
    bool     (*fCC0630)(uint32_t game, void* unit);       // 0xA0630
    void     (*fAEA80)(void* game, void* unit, int a, uint32_t b, int c, int d, int e);
    void     (*fAE6F0)(void* item);                       // 0x8E6F0
    int      (*fB1340)(void* p);                          // 0x91340
    uint32_t (*fFE5A0)(void* game, uint32_t a, int b);    // 0xDE5A0
    void     (*fBF210)();                                 // 0x9F210, seam: reads ambient regs
    void     (*fBE9B0)(uint32_t ecx, int v);              // 0x9E9B0
    void     (*fFDDE0)(int v, void* a, void* b);          // 0xDDDE0
    void     (*fFED30)(void* a, void* b, void* c);        // 0xDED30
    void     (*f648E0)(void* a, void* b, uint32_t cb, void* c); // 0x448E0
    void     (*fCC11B0)(int skillId, void* game, void* unit, void* target, uint32_t* blk, int z); // 0xA11B0, seam: block is the 0x70-byte local
    void     (*fFD450)(void* game, void* unit, void* target, uint32_t* blk, int z, uint8_t srcDam); // 0xDD450
    void     (*f214D0)(int v, void* game);               // 0x14D0
    // 0x45CE0 / 0x46560 seams (ordinals whose arg order is taken from the decompile)
    uint32_t (*o10511)(void* unit);                       // used-skill frame / flags
    int      (*o10601)();                                 // skill id from ambient skill reg
    void     (*f21180)(void* p);                          // 0x1180 on unit+0x20
    uint32_t (*f21c20)();                                 // 0x1C20
    void     (*fAE900)(int v);                            // 0x8E900
    void     (*o10188)(void* it, int stat, int val, int layer);
    void     (*o10243)(int a, int b);
    uint32_t (*o10786r)(void* unit, uint32_t calc, uint32_t a, uint32_t b); // formula eval, returns value
    int      (*fBEE50)(int z, int* out, void* unit, int z2, int skillId, uint32_t lvl); // 0xFBEE50
    int      (*fBFA30)(void* game, int* out);             // 0xBFA30
    int*     (*fC3610)(void* game);                       // 0xC3610
    void     (*fD0FE40)(void* game);                      // 0x10FE40
    void     (*f6E2A0)(void* n, uint32_t calc, int* p, uint32_t q); // 0x6E2A0
    void     (*f6F970)(uint32_t calc, void* unit, int* p, int id, uint32_t q); // 0x6F970
    uint32_t bitMask4;                                    // gdwBitMasks[4]
    // 0x4A030 globals
    int      clientsInit;                                 // DAT_6fd31c18
    void     (*enterCS)();                                // Ordinal 10050
    void     (*leaveCS)();
    void**   clientBuckets;                               // DAT_6fd307b8, 0x100 entries
};

// 1.13c D2Game+0x43450
uint32_t sub_43450(D2GameBatch43450Env& e, void* game, void* unit, uint32_t ebp, uint32_t esi) {
    int local8[2];
    e.f9FA30(game, local8);
    void* r = e.o10331(unit);
    if (r) {
        uint8_t* rec = e.o10014(e.o10826(r));
        if (rec && rec[4]) {
            if (rec[4] == 2 && e.o10686(unit, ebp, esi, 0x804)) return 0;
            return e.fE19C0_d019c0(ebp, 0, game, unit, 0);
        }
    }
    return 0;
}

// 1.13c D2Game+0x434E0   (this=game, param_1=src unit, param_2=target unit)
uint32_t sub_434E0(D2GameBatch43450Env& e, void* game, void* src, void* tgt, uint32_t p3, uint32_t p4, int p5) {
    (void)p3;
    if (!src || !tgt) return 0;
    if (!((rd32(tgt, 200) >> 10) & 1)) return 0;
    void* u = e.f_d003a0(game, rd32(tgt, 0x98));
    if (!u || !e.f_d00a60(src, game)) return 0;
    void* sk = e.f21420(nullptr); // seam: 0x1420 takes ambient register arg
    if (!sk) return 0;
    int m = rds16(sk, 0x48);
    if (m < 0 || m >= (int)rd32(e.dataTables, 0xb6c)) return 0;
    if (p5 < 0) p5 = 1;
    uint8_t* r = e.f21250(nullptr); // seam: 0x1250 ambient register arg
    if (!(e.bitMask4 & r[5])) return 0;
    uint32_t p[0x17] = {};
    p[0] = 0x20; p[1] = (uint32_t)(uintptr_t)src; p[2] = (uint32_t)(uintptr_t)src;
    p[4] = (uint32_t)m;                  // local_4c (frame 0x5c bytes, dword = (0x5c-off)/4)
    p[7] = e.f21100(u); p[8] = e.f21130(u); // local_40, local_3c
    p[11] = p4; p[12] = (uint32_t)p5;       // local_30, local_2c
    e.f8F930(game, p);
    return 1;
}

// 1.13c D2Game+0x43610   (param_1=game, param_2=unit, param_3=skillId, param_4)
uint32_t sub_43610(D2GameBatch43450Env& e, uint32_t game, int* unit, int skillId, uint32_t p4) {
    const uint8_t* t = e.dataTables;
    if (skillId < 0 || (int)rd32(t, 0xba0) <= skillId) return 0;
    uint8_t* sk = (uint8_t*)(uintptr_t)rd32(t, 0xb98) + (uint32_t)skillId * 0x23c;
    if (!sk) return 0;
    int missile = rds16(sk, 0x48);
    if (missile < 0 || missile >= (int)rd32(t, 0xb6c)) return 0;
    if (unit) unit[0x31] |= 0x40;
    e.o10786(unit, rd32(sk, 0x138), skillId);
    int dx = 0, dy = 0;
    e.fD007A0((void*)(uintptr_t)game);
    int* tgt = (int*)e.o10392(unit[0xb]);
    if (tgt != unit && tgt) {
        dx = (int)e.f21100(tgt) - (int)e.f21100(unit);
        dy = (int)e.f21130(tgt) - (int)e.f21130(unit);
    }
    // seam: EBX (game) / EBP / EDI of 0xA28F0 and 0xAC9A are ambient registers in the decompile.
    int* m = (int*)e.fCC28F0((void*)(uintptr_t)game, p4, unit, 0, (uint32_t)missile, dx, dy, 0, 0, 0);
    if (!m) return 0;
    e.o10637(m, 0);
    return 1;
}

// 1.13c D2Game+0x437D0   (param_1=game, param_2=unit, param_3=skillId; EDI=game ptr, EBP=skill record)
uint32_t sub_437D0(D2GameBatch43450Env& e, uint32_t game, int* unit, int skillId, void* gameEdi) {
    const uint8_t* t = e.dataTables;
    if (skillId < 0 || skillId >= (int)rd32(t, 0xba0)) return 0;
    uint8_t* sk = (uint8_t*)(uintptr_t)rd32(t, 0xb98) + (uint32_t)skillId * 0x23c;
    if (!sk) return 0;
    int m = rds16(sk, 0x48);
    if (m < 0 || m >= (int)rd32(t, 0xb6c)) return 0;
    if (!e.fCC0630(game, unit)) return 0;
    e.fD007A0((void*)(uintptr_t)game);
    int* tgt = (int*)e.o10392(unit[0xb]);
    int x, y;
    if (tgt == unit || !tgt) { x = e.o10983(unit[0xb]); y = e.o10764(unit[0xb]); }
    else { x = (int)e.f21100(tgt); y = (int)e.f21130(tgt); }
    if (!x || !y) return 0;
    e.o10331(unit);
    if (e.o10057(nullptr)) return 0; // seam: 0x1280 result feeds Ordinal 10057
    uint32_t p[0x17] = {};
    p[0] = 0x21; p[1] = (uint32_t)(uintptr_t)unit;
    p[3] = (uint32_t)x; p[4] = (uint32_t)y;       // iStack_54/50
    int ux = (int)e.f21100(unit), uy = (int)e.f21130(unit);
    p[5] = (uint32_t)(x - (uy - y));              // iStack_4c
    p[6] = (uint32_t)((ux - x) + y);              // iStack_48
    e.f8F930(gameEdi, p);
    p[6] = (uint32_t)(y - (ux - x));
    p[5] = (uint32_t)((uy - y) + x);
    e.f8F930(gameEdi, p);
    int m2 = rds16(sk, 0x4a);
    if (m2 >= 0 && m2 < (int)rd32(t, 0xb6c)) { p[5] = 0; p[6] = 0; p[0] = 1; e.f8F930(gameEdi, p); }
    return 1;
}

// 1.13c D2Game+0x439A0   (ESI=unit; register-only otherwise; seam: Ordinal args are ambient)
void sub_439A0(D2GameBatch43450Env& e, int* unit, void* game) {
    if (!unit[0xb]) return;
    void* a = e.o10203(unit);
    if (!a) return;
    void* b = e.o10871(a, 0);
    if (!b) return;
    int skill = e.o10680(b); e.o10680(b);
    const uint8_t* t = e.dataTables;
    if (skill <= 0 || skill >= (int)rd32(t, 0xba0)) return;
    void* sk = e.f21420(nullptr);
    if (!sk) return;
    int m = rds16(sk, 0x48);
    if (m < 0 || m >= (int)rd32(t, 0xb6c)) return;
    if (e.o10057(e.o10331(nullptr))) return;
    e.f21100(unit);
    uint32_t p[0x17] = {};
    p[0] = 0x21;
    e.f21130(unit);
    p[4] = (uint32_t)m;                            // iStack_78
    e.f8F930(game, p);
}

// 1.13c D2Game+0x43CA0   (EAX=game, EBX=unit; callers 0x441C0)
void sub_43CA0(D2GameBatch43450Env& e, uint8_t* game, int* unit) {
    uint32_t n = 1;
    e.o10945(unit, 0, 0);
    if (unit) {
        for (uint8_t* p = (uint8_t*)(uintptr_t)unit[0x37]; p;) {
            uint8_t c = *p; p = (uint8_t*)(uintptr_t)rd32(p, 0x24);
            if (c == 0) e.fAE6F0(p);
        }
        int i = unit[1];
        const uint8_t* t = e.dataTables;
        if (i >= 0 && i < (int)rd32(t, 0xa80)) {
            int k = rds16((const uint8_t*)(uintptr_t)rd32(t, 0xa78), 0x18 + (uint32_t)i * 0x1a8);
            if (k >= 0 && k < (int)rd32(t, 0xa98)) {
                const uint8_t* r = (const uint8_t*)(uintptr_t)rd32(t, 0xa90) + (uint32_t)k * 0x134;
                if (r) n = rd8(r, 0x108);
            }
        }
    }
    e.fAEA80(game, unit, 1, rd32(game, 0xa8) + n, 0, 0, 0);
}

// 1.13c D2Game+0x451B0   (param_1=game, param_2=unit, param_3=duration, param_4=skill record; EBX)
void sub_451B0(D2GameBatch43450Env& e, void* game, void* unit, int dur, uint8_t* rec, void* ebx) {
    if (dur < 10) dur = 10;
    uint32_t b[0x1c] = {};
    uint16_t flags = (uint16_t)(rd32(rec, 0x12e) & 0xffff);
    uint32_t ecx = 0;
    if (!(flags & 1)) {
        uint32_t u = e.o10653();
        flags = (uint16_t)e.fFE5A0(game, u, 0);
        ecx = flags & 0xff;
        if (flags & 1) flags |= (uint16_t)rd32(rec, 0x12e);
    }
    if (flags & 1) {
        b[0] |= rd32(rec, 0x130);
        if (rd32(rec, 0x134)) b[0x1c - 1 - 0] = rd32(rec, 0x134); // seam: local_10 is the last dword of the block
        e.o10786(nullptr, 0, 0);   // seam: Ordinal 10786 args ambient
        e.fBF210();
    }
    std::memcpy(&b[1], &flags, 2);  // local_6c overlays b[1] low word
    e.fBE9B0(ecx, dur);
    e.fFDDE0(0x80, game, unit);
    e.fFED30(game, ebx, unit);
}

// 1.13c D2Game+0x45410   (EAX=skillId, param_1=game, param_2=unit/params)
void sub_45410(D2GameBatch43450Env& e, int skillId, void* game, uint32_t* p2) {
    const uint8_t* t = e.dataTables;
    if (skillId < 0 || skillId >= (int)rd32(t, 0xba0)) return;
    if (!(rd32(t, 0xb98) + (uint32_t)skillId * 0x23c)) return;
    if (e.fB1340(p2) <= 0) return;
    uint32_t b[0x17] = {};
    b[0] = 0x8000;
    void* m = e.f8F930(game, b);
    if (!m) return;
    e.o10637(m, p2 ? *p2 : 6);
    e.o10019(m, p2 ? p2[3] : 0xffffffffu);
}

// 1.13c D2Game+0x454E0   (EAX=flag, param_1=skillId, param_2=unit, param_3=game, param_4=count, param_5, param_6)
uint32_t sub_454E0(D2GameBatch43450Env& e, int flag, uint32_t p1, int* unit, void* game, int count, uint32_t p5, uint32_t p6) {
    if (flag) {
        e.fCC28F0(game, p5, unit, p6, p1, 0, 0, 0, 0, 0);
        if (--count < 1) return 1;
    }
    uint32_t b[0x17] = {};
    b[0] = 0x21; b[1] = (uint32_t)(uintptr_t)unit;
    int t = unit[0];
    bool direct = t == 2 || (t > 3 && t < 6);
    b[4] = direct ? rd32((void*)(uintptr_t)unit[0xb], 0xc) : unit[0xb] ? e.o10867(unit[0xb]) : 0;
    b[5] = direct ? rd32((void*)(uintptr_t)unit[0xb], 0x10) : unit[0xb] ? e.o10750(unit[0xb]) : 0;
    b[2] = p5; b[0x10] = p6; b[0x11] = p1;  // uStack_4c, uStack_30, uStack_2c (stack-frame order)
    int tmp[3];
    if (e.f9FA30(game, tmp)) for (int i = 0; i < count; ++i) e.f8F930(game, b);
    return 1;
}

// 1.13c D2Game+0x45EE0
uint32_t sub_45EE0(D2GameBatch43450Env& e, uint8_t* game, uint32_t, int* unit, int off, int skillId) {
    const uint8_t* t = e.dataTables;
    if (skillId < 0 || skillId >= (int)rd32(t, 0xba0)) return 0;
    const uint8_t* sk = (const uint8_t*)(uintptr_t)rd32(t, 0xb98) + (uint32_t)skillId * 0x23c;
    if (!sk) return 0;
    int16_t s = rds16(sk, 0x82);
    if (s < 0 || (int)rd32(t, 0xc4) < s) return 0;
    uint32_t when = rd32(game, 0xa8) + (uint32_t)off;
    if (e.o10871(unit, s)) return 0;
    int id = unit ? unit[3] : -1, ty = unit ? unit[0] : 6;
    void* it = e.o11013(rd32(game, 0x1c), 2, when, (uint32_t)ty, (uint32_t)id);
    if (!it) return 0;
    e.o10785(it, rds16(sk, 0x82));
    e.o10911(it);
    e.o10807(unit, it, 1);
    e.o10945(unit, rds16(sk, 0x82), 1);
    e.o11043(it, when);
    e.fAEA80(unit, unit, 0xc, when, 0, 0, 0);
    e.f648E0(unit, it, 0x6fcbf7a0, it);
    return 1;
}

// 1.13c D2Game+0x45CE0   (ECX=game, EDX=unit, stack=skillId)
uint32_t sub_45CE0(D2GameBatch43450Env& e, uint8_t* game, int* unit, int skillId) {
    const uint8_t* t = e.dataTables;
    if (skillId < 0 || skillId >= (int)rd32(t, 0xba0)) return 0;
    const uint8_t* sk = (const uint8_t*)(uintptr_t)rd32(t, 0xb98) + (uint32_t)skillId * 0x23c;
    if (!sk) return 0;
    int16_t s = rds16(sk, 0x80);
    if (s < 0 || s >= (int)rd32(t, 0xc4)) return 0;
    uint32_t when = e.o10511(unit);
    if (!when) return 0;
    if (e.o10601() != skillId) return 0;
    if (unit) unit[0x31] |= 0x40;
    e.f21180(unit + 8);
    uint32_t calc = rd32(sk, 0x60);
    e.o10786(unit, calc, (uint32_t)skillId);  // result unused (decompile discards it)
    void* it = e.o10871(unit, s);
    if (!it) {
        int id = unit ? unit[3] : -1, ty = unit ? unit[0] : 6;
        it = e.o11013(rd32(game, 0x1c), 2, 0x48b, (uint32_t)ty, (uint32_t)id);
        if (!it) return 0;
        e.o10785(it, rds16(sk, 0x80));
        e.o10911(it);
        e.o10807(unit, it, 1);
        e.o10945(unit, rds16(sk, 0x80), 1);
    }
    e.o11043(it, 0x48b);
    e.fAEA80(game, unit, 0xc, when, 0, 0, 0);
    e.f648E0(unit, it, 0x6fcbf7a0, (void*)"..\\Source\\D2Game\\SKILLS\\SkillDruid.cpp");
    e.o10188(it, 0x15e, skillId, 0);
    e.o10188(it, 0x15f, (int)calc, 0);
    uint32_t base = e.f21c20() + rd32((uint8_t*)it, 0xa8);
    e.fAE900(5);
    e.fAEA80(it, unit, 5, base, 0, skillId, 0x48b);
    e.o10243(skillId, 0x15e);
    return 1;
}

// 1.13c D2Game+0x46560   (ECX=game, EDX=unit, stack=skillId, lvl)
uint32_t sub_46560(D2GameBatch43450Env& e, uint8_t* game, int* unit, int skillId, uint32_t lvl) {
    const uint8_t* t = e.dataTables;
    if (skillId < 0 || skillId >= (int)rd32(t, 0xba0)) return 0;
    const uint8_t* sk = (const uint8_t*)(uintptr_t)rd32(t, 0xb98) + (uint32_t)skillId * 0x23c;
    if (!sk) return 0;
    int out30[1], out2c[2];
    int id = e.fBEE50(0, out30, unit, 0, skillId, lvl);
    if (id < 0) return 0;
    int8_t k = (int8_t)sk[0xbe];
    if (k < 0 || k >= (int)rd32(t, 0xbf0)) return 0;
    if (unit) unit[0x31] |= 0x40;
    if (!e.fBFA30(game, out2c)) return 0;
    uint32_t calc = rd32(sk, 0xc0);
    e.o10786r(unit, calc, (uint32_t)skillId, lvl);
    int* p = e.fC3610(game);
    if (!p) return 0;
    e.fD0FE40(game);
    int n = (int)e.o10786r(unit, rd32(sk, 0x13c), 0, (uint32_t)k);
    if (n < 1) n = 1;
    e.f6E2A0((void*)(intptr_t)n, calc, p, (uint32_t)k);
    e.f6F970(calc, unit, p, id, (uint32_t)k);
    return 1;
}

// 1.13c D2Game+0x463A0   (EAX=unit, ESI=count, EDI=skillId)
uint32_t sub_463A0(D2GameBatch43450Env& e, uint8_t* game, uint32_t* p2, int off, int* unit, int count, int skillId) {
    if (count < 1) return 0;
    if (sub_45EE0(e, game, 0, unit, off, skillId)) sub_45410(e, skillId, game, p2);
    return 1;
}

// 1.13c D2Game+0x4A030  CLIENTS_GetExpansionClientCount
int CLIENTS_GetExpansionClientCount(D2GameBatch43450Env& e) {
    int n = 0;
    if (!e.clientsInit) return 0;
    e.enterCS();
    for (int b = 0; b < 0x100; ++b)
        for (uint8_t* c = (uint8_t*)e.clientBuckets[b]; c; c = (uint8_t*)(uintptr_t)rd32(c, 0x4ac))
            if (rd8(c, 10) & 0x20) ++n;
    e.leaveCS();
    return n;
}

// 1.13c D2Game+0x44ED0   (ECX=game, EDX=unit, param_3=skillId; FireClaws-style melee strike; returns 1 handled)
int sub_44ED0(D2GameBatch43450Env& e, void* game, void* unit, int skillId) {
    const uint8_t* t = e.dataTables;
    if (skillId < 0 || skillId >= (int)rd32(t, 0xba0)) return 0;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)(rd32(t, 0xb98) + (uint32_t)skillId * 0x23c);
    if (!rec) return 0;
    e.fD007A0(game);
    void* tgt = e.o10392(rd32(unit, 0x2c));
    if (tgt == unit || !tgt) return 0;
    uint32_t b[0x1c] = {};
    e.fCC11B0(skillId, game, unit, tgt, b, 0);
    if (b[1] & 1) {                       // byte at +4 of the block (local_70)
        b[0] |= 2 | rd32(rec, 0x130);
        if (rd32(rec, 0x134)) b[0x1c - 1 - 0] = rd32(rec, 0x134);
        e.o10786(unit, rd32(rec, 0x138), 0);   // seam: Ordinal 10786 level arg ambient
        e.fFD450(game, unit, tgt, b, 0, rd8(rec, 0x1a5));
        e.fBF210();
        e.fFDDE0(0x80, game, tgt);
    }
    return 1;
}

// 1.13c D2Game+0x45000   (ECX=game, EDX=unit, param_3=skillId; Hunger: strike + life/mana leech)
int sub_45000(D2GameBatch43450Env& e, void* game, void* unit, int skillId) {
    const uint8_t* t = e.dataTables;
    if (skillId < 0 || skillId >= (int)rd32(t, 0xba0)) return 0;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)(rd32(t, 0xb98) + (uint32_t)skillId * 0x23c);
    if (!rec) return 0;
    e.fD007A0(game);
    void* tgt = e.o10392(rd32(unit, 0x2c));
    if (tgt == unit || !tgt) return 0;
    wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    uint32_t b[0x1c] = {};
    e.fCC11B0(skillId, game, unit, tgt, b, 0);
    if (b[1] & 1) {
        b[0] |= 2 | rd32(rec, 0x130);
        b[1] = (b[1] & 0xffff0000u) | ((b[1] | rd32(rec, 0x12e)) & 0xffff);
        if (rd32(rec, 0x134)) b[0x1c - 1 - 0] = rd32(rec, 0x134);
        e.fFD450(game, unit, tgt, b, 0, rd8(rec, 0x1a5));
        e.fBF210();
        e.o10786(unit, rd32(rec, 0x138), 0);
        e.f214D0(100, game);
        e.o10786(unit, rd32(rec, 0x13c), 0);
        e.o10786(unit, rd32(rec, 0x140), 0);
        e.fFDDE0(0x80, game, unit);
        e.fFED30(game, tgt, unit);
    }
    return 1;
}
