// Implementation batch 4ebe0: SkillNec/skill-glue helpers, written from the cached 1.13c decompiles.
// Callees are declared by retail RVA (rNNNN) / ordinal (oNNNNN); registers the decompile shows as
// unaff_* / in_EAX are explicit parameters. Not yet linked or vectored (RE-DONE only).
#include <cstdint>
#include <cstdlib>
struct U; // opaque unit/room/game; fields touched by raw offset as in the decompile
using i32 = int32_t; using u32 = uint32_t;
#define F(o, ...) extern "C" i32 o(__VA_ARGS__)
extern "C" {
void r_E0460(); void r_EFC60(); intptr_t o10945(...); intptr_t o10265(...); intptr_t o10024(...);
intptr_t r_DFA90(...); intptr_t r_9F120(...); intptr_t r_13B0(...); intptr_t o10444(...);
intptr_t r_E07A0(...); intptr_t o10392(...); i32 o11064(i32); i32 o10202(void*, i32, i32, const char*); i32 o11160(void*, i32);
intptr_t o10830(...); intptr_t r_E0A60(...); intptr_t o10331(...); i32 o10057(i32);
i32 o10910(i32, i32, i32); bool r_22A80(i32); intptr_t r_A00B0(...); void o10261(i32, i32, i32, i32); intptr_t o10819(...);
void r_DE0C0(i32*, i32*, i32, u32*); void r_DCF20(i32, i32*, i32); intptr_t o10750(...); intptr_t o10867(...); void o10769(i32, i32, i32, i32);
intptr_t o10913(...); intptr_t r_DE5A0(...); intptr_t o10653(...); intptr_t o10786(...); intptr_t r_9F210(...); intptr_t r_DDDE0(...);
void r_9D230(i32, i32*); void r_57110(i32, i32); void r_9F7A0(i32*, i32);
intptr_t o10045(...); intptr_t o10188(...); intptr_t r_4E230(...); bool r_215D0(i32, u32); intptr_t r_8EA80(...);
intptr_t r_A3610(...); intptr_t r_4F970(...); void r_232B0(i32*, i32); void r_EFDB0(i32*); intptr_t r_9EE50(...); i32 r_21280();
}
extern i32** sgptDataTables_exref;
static inline i32 sdt(i32 off) { return *(i32*)(*(i32*)sgptDataTables_exref + off); }
static const char* NEC = "..\\Source\\D2Game\\SKILLS\\SkillNec.cpp";

// 1.13c D2Game+0x4EBE0
void D2GAME_4EBE0(void* a, i32 b)
{
    r_E0460();
    o10945(a, b, 0);
    if (a == 0) { o10024((void*)0, o10265(0xa2a)); std::_Exit(-1); } // DAT_6fd19ff9 message arg
    r_EFC60();
}

// 1.13c D2Game+0x4EC30 (esi = unit)
bool D2GAME_4EC30(i32 p1, i32* unit)
{
    if (unit && *unit == 1 && r_13B0(p1, 8) != 0 && r_DFA90() != 0 && o10444() != 0) return r_9F120() != 0;
    return false;
}

// 1.13c D2Game+0x4EDD0
bool D2GAME_4EDD0(i32 p1, i32* p2)
{
    r_E07A0(p1);
    i32* u = (i32*)(intptr_t)o10392(p2[0xb]);
    if (u != p2 && u && *u == 4 && u[4] == 3 && o11064(u[1]) != 0 && o10202(u, 0x10, 0x100, NEC) != 0)
        return o11160(u, 0) == 0;
    return false;
}

// 1.13c D2Game+0x4EE80 (esi = unit)
bool D2GAME_4EE80(void* unit)
{
    if (!unit) return false;
    if (o10830(nullptr) == 0 && r_9F120() != 0 && r_DFA90() == 0 && o10057(o10331(0)) == 0)
        return r_E0A60(unit, unit) != 0;
    return false;
}

// 1.13c D2Game+0x4EF70
u32 D2GAME_4EF70(i32 p1, i32* p2)
{
    if (!p1 || !p2) return 0;
    i32 v = o10910(p1, 0x96, 0);
    if (!v) return 0;
    if (*p2 == 0) { if (v > 0x31) v = 0x32; }
    else if (*p2 == 1) { if (!r_22A80(0xc)) { if (v > 0x59) v = 0x5a; } else if (v > 0x31) v = 0x32; }
    else return 0;
    i32 s = r_A00B0();
    if (!s) return 0;
    o10261(s, 0x44, -v, 0); o10261(s, 0x45, -v, 0);
    o10819(p2, NEC, 0xb62);
    return 1;
}

static i32 unitArg(i32* u, int off, intptr_t (*f)(...))
{
    i32 t = *u;
    if (t == 2 || (t > 3 && t < 6)) return *(i32*)(u[0xb] + off);
    return u[0xb] ? (i32)f((intptr_t)(u32)u[0xb]) : 0;
}

// 1.13c D2Game+0x4F370 (edi = pair)
u32 D2GAME_4F370(i32 p1, i32* p2, i32* pair)
{
    i32* e = *(i32**)(p1 + 0xc);
    if (!e) return 0;
    u32 local[28];
    const u32* src = *(const u32**)e;
    for (int i = 0; i < 0x1c; i++) local[i] = src[i];
    i32 a = unitArg(p2, 0x10, o10750);
    i32 b = unitArg(p2, 0xc, o10867);
    o10769(e[1], e[2], b, a);
    r_DE0C0((i32*)(intptr_t)pair[0], (i32*)(intptr_t)pair[1], 1, local);
    r_DCF20(pair[0], (i32*)(intptr_t)pair[1], (i32)(intptr_t)local);
    return 1;
}

// 1.13c D2Game+0x4F450 (ebx = game, esi = unit)
void D2GAME_4F450(i32* game, i32* unit)
{
    if (unit && *unit == 0 && unit[1] == 3) {
        u32 buf[28];
        o10913();
        for (int i = 0; i < 0x1c; i++) buf[i] = 0;
        r_DE0C0(game, unit, 0, buf);
        r_DCF20((i32)(intptr_t)game, unit, (i32)(intptr_t)buf);
    }
}

// 1.13c D2Game+0x4F7D0 (ebx, ebp = caller registers; iStack_* come from the callee-filled frame)
u32 D2GAME_4F7D0(i32 p1, i32 p2, i32* ebx, i32* ebp, i32 skillId, i32 lvl, i32 extra)
{
    r_E07A0(p1);
    i32 o = o10392(0);
    if (o == p2 || o == 0) return 0;
    o10331(0);
    if (o10057(0) != 0) return 0;
    if (!(skillId > -1 && skillId < sdt(0xba0))) return 0;
    i32 rec = skillId * 0x23c + sdt(0xb98);
    if (!rec) return 0;
    i32 h = o10653(p2, skillId, lvl, 0);
    u32 r = r_DE5A0((i32)(intptr_t)ebx, h, p2);
    if ((r & 1) && (o10786(p2, *(i32*)(rec + 0x138), skillId, extra), *(char*)(rec + 0x1dc) != 0)) {
        o10786(p2, *(i32*)(rec + 0x144), skillId, extra);
        r_9F210();
    }
    u32 b = *(uint8_t*)(rec + 0x1a5);
    if (!b) b = 0x80;
    r_DDDE0(b, ebx, ebp);
    return 1;
}

// 1.13c D2Game+0x500A0 (eax = skill id)
void D2GAME_500A0(i32 skillId, i32* unit, i32 p3)
{
    if (skillId > -1 && skillId < sdt(0xba0) && skillId * 0x23c + sdt(0xb98) != 0)
        r_9D230(p3, unit); // both retail branches make the identical call
}

// 1.13c D2Game+0x50130
void D2GAME_50130(i32* unit, i32 p2)
{
    if (!unit) return;
    r_57110(1, p2);
    if (*unit == 1) r_9D230(unit[0x20], unit);
    r_9F7A0(unit, p2);
}

// 1.13c D2Game+0x50B70
u32 D2GAME_50B70(i32* unit, i32* p2)
{
    i32 g = *p2; void* room = (void*)(intptr_t)p2[1];
    if (!(unit && *unit == 1 && o10830(unit) == 0 && r_E0A60(room, (void*)(intptr_t)g) != 0 && r_DFA90() == 0 && r_9F120() != 0)) return 0;
    r_4E230();
    i32 s = r_A00B0();
    if (!s) return 0;
    i32* q = p2 + 8;
    for (int n = 5; n; n--, q++)
        if (*q > -1 && *q < sdt(0xbd4)) { i32 x = r_4E230(); if (x) o10188(s, *q, x, 0); }
    if (p2[3]) o10819(unit, NEC, 0x3d7);
    r_E0460();
    i32 gm = *p2;
    if (gm && unit[0x34] == 0xb && (*unit == 0 || *unit == 1)) {
        i32 old = *(i32*)(gm + 0x111c);
        i32* t = (i32*)o10045("..\\Source\\D2Game\\GAME\\Targets.cpp", 0xa1, 0);
        if (t) {
            t[0] = (i32)(intptr_t)unit; t[1] = 0; t[2] = 0; t[3] = 0;
            *(i32**)(gm + 0x111c) = t;
            if (old) { t[2] = old; *(i32**)(old + 0xc) = t; }
            unit[0x34] = 9;
        }
    }
    if (*unit == 1 && r_215D0(0, 0x10) && unit[5]) { *(i32*)(unit[5] + 0x38) = 3; *(i32*)(unit[5] + 0x34) = 0; }
    r_8EA80(*p2, unit, 10, *(i32*)(*p2 + 0xa8) + p2[6], 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0x50E60 (eax = skill id; ebp frame holds the caller unit, retaddr byte = level)
i32* D2GAME_50E60(i32 skillId, i32* piVar1, i32 p3, i32 p4, i32 frameUnit, uint8_t lvl)
{
    i32 rec = (skillId < 0 || sdt(0xba0) <= skillId) ? 0 : skillId * 0x23c + sdt(0xb98);
    if (skillId == 0) return nullptr;
    i32* out[2];
    if (r_9EE50(0, (i32**)out, piVar1, 0, skillId, p4) < 0) return nullptr;
    i32* pu = piVar1;
    uint8_t b = *(uint8_t*)(rec + 0xbe);
    if (sdt(0xbf0) <= (i32)b) b = 0;
    o10331(0);
    if (o10057(r_21280()) != 0) return nullptr;
    (void)o10786((i32)(intptr_t)piVar1, *(i32*)(frameUnit + 0xc0), 0, 0);
    i32* r = (i32*)(intptr_t)r_A3610((i32)(intptr_t)pu);
    if (!r) return nullptr;
    r_4F970((i32)(intptr_t)pu, (i32)(intptr_t)piVar1, r, skillId, (i32*)(intptr_t)b);
    r_232B0(r, 0);
    r_EFDB0(r);
    return r;
}
