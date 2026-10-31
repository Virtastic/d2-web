// 1.13c D2Client+0x96280  target_dispatch: the left-hand body-slot press (slot 5 of the per-slot table at 0xE0EC4,
// reached from the inventory press dispatcher 0x98DC0 with the body location on the stack; the caller drops eax).
// Convention: EDX = inventory, ECX = player unit, one stack word = body location, ret 4. The frame is balanced:
// sub esp,8; four register pushes (ebx ebp esi edi) and four pops; the "fifth push" is the argument of the C202 thunk.
// Callees go through host(&call) in retail's register/stack order (s[] in parameter order, s[0] = top of stack).
// Thunks (import targets read from the image): 0xC202 = D2Common #11017 (cursor item of an inventory, 1 arg),
// 0xC262 = #11139 (item at a body location, 2 args), 0xC976 = #10946 (placement answer 0..7; player, bodyLoc, item,
// flag), 0xC9A0 = #10822 (1 arg). All four are stdcall (each caller pushes and none cleans).
// Local callees: 0x81CB0 (EAX = item, 0 = usable), 0x81CE0 (EAX = item, release), 0x923F0 (0 = refuse),
// 0x14910 / 0x14940 / 0x14A10 (packet builders: AL = opcode, ECX, EDX), 0x23200 (ret 8: player, code),
// 0x81E40 (ESI = item, EDI = out or 0, returns the sound id), 0x88A70 (EBX = sound id, four zero words, ret 0x10),
// 0x8BE10 (shop-mode predicate), 0x95F80 (EAX, ECX = item, EDX, ESI, three stack words, ret 0xC).
// Returns EBX (0, or 1 once a path ran), or 1 on the cases that write EAX = 1.
#include <cstdint>
#include <emscripten.h>

struct D2cTdCall { uint32_t rva, eax, ecx, edx, ebx, esi, edi; uint32_t s[4]; };
typedef uint32_t (*D2cTdHost)(D2cTdCall*);
typedef uint32_t (*D2cTdRead)(uint32_t ref, uint32_t off);   // dword at ref + off (unit records)

struct D2cTdEnv {
    D2cTdHost host;
    D2cTdRead rd;
    const int32_t* mode;       // [0x6FBCB864] RVA 0x11B864  cursor mode: 6 and 8 send a packet first
    const uint32_t* srcRef;    // [0x6FBCB834] RVA 0x11B834  source unit of the mode-6 packet
    const int32_t* shopMode;   // [0x6FBCBC34] RVA 0x11BC34
    int32_t* consume;          // [0x6FBCBC30] RVA 0x11BC30  written 1 on the consume paths
    const uint32_t* slideA;    // [0x6FBBB2A4] RVA 0x10B2A4  third stack word of 0x95F80
    const uint32_t* slideB;    // [0x6FBBB430] RVA 0x10B430  EDX of 0x95F80
};

namespace {
constexpr uint32_t kNone = 0xFFFFFFFFu;
uint32_t call(const D2cTdEnv* e, uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t ebx = 0,
              uint32_t esi = 0, uint32_t edi = 0, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) {
    D2cTdCall c{rva, eax, ecx, edx, ebx, esi, edi, {s0, s1, s2, s3}};
    return e->host(&c);
}
bool usable(const D2cTdEnv* e, uint32_t ref) { return call(e, 0x81CB0, ref) == 0; }
void release(const D2cTdEnv* e, uint32_t ref) { call(e, 0x81CE0, ref); }
uint32_t unitId(const D2cTdEnv* e, uint32_t ref) { return ref ? e->rd(ref, 0xC) : kNone; }
// 0x66xx tail shared by the packet cases: 0x81E40 (ESI = item, EDI = 0) names the sound, 0x88A70 plays it.
void sound(const D2cTdEnv* e, uint32_t esi) {
    const uint32_t id = call(e, 0x81E40, 0, 0, 0, 0, esi, 0);
    call(e, 0x88A70, 0, 0, 0, id, 0, 0, 0, 0, 0, 0);
}
}  // namespace

extern "C" {
EMSCRIPTEN_KEEPALIVE uint32_t d2c_target_dispatch_96280(const D2cTdEnv* e, uint32_t inv, uint32_t player,
                                                        uint32_t bodyLoc) {
    uint32_t ebx = 0;
    const uint32_t esi = call(e, 0xC202, 0, 0, 0, 0, 0, 0, inv);           // cursor item
    const uint32_t edi = call(e, 0xC262, 0, 0, 0, 0, 0, 0, inv, 5);        // item in the left hand
    const uint32_t s4 = call(e, 0xC262, 0, 0, 0, 0, 0, 0, inv, 4);         // item in the right hand

    // Pre-dispatch packet: 0x27 in mode 6 (needs the source unit), 0x4C in mode 8; either releases edi.
    const int32_t mode = *e->mode;
    if (mode == 6) {
        const uint32_t src = *e->srcRef;
        if (src && edi && usable(e, edi) && call(e, 0x923F0)) {
            call(e, 0x14910, 0x27, e->rd(edi, 0xC), e->rd(src, 0xC));
            release(e, edi);
            ebx = 1;
        }
    } else if (mode == 8) {
        if (edi && usable(e, edi) && call(e, 0x923F0)) {
            call(e, 0x14940, 0x4C, e->rd(edi, 0xC));
            release(e, edi);
            ebx = 1;
        }
    }

    const uint32_t c = call(e, 0xC976, 0, 0, 0, 0, 0, 0, player, bodyLoc, esi, 0);
    if (c > 7) return ebx;
    switch (c) {
    case 0: {
        if (!esi) return 1;
        if (call(e, 0xC976, 0, 0, 0, 0, 0, 0, player, bodyLoc, esi, 1) == 0) {
            call(e, 0x23200, 0, 0, 0, 0, 0, 0, player, 0x13);
            return 1;
        }
        const uint32_t k = call(e, 0xC9A0, 0, 0, 0, 0, 0, 0, esi);
        const uint32_t want = player ? e->rd(player, 4) : kNone;
        call(e, 0x23200, 0, 0, 0, 0, 0, 0, player, (k == 7 || k == want) ? 0x14 : 0x13);
        return 1;
    }
    case 1:
        if (!usable(e, esi) || !call(e, 0x923F0)) return ebx;
        call(e, 0x14910, 0x1A, unitId(e, esi), 5);
        release(e, esi);
        sound(e, esi);
        return 1;
    case 2:
        if (!usable(e, esi) || !usable(e, s4) || !call(e, 0x923F0)) return ebx;
        call(e, 0x14910, 0x1B, unitId(e, esi), 5);
        release(e, s4);
        release(e, esi);
        sound(e, esi);
        *e->consume = 1;
        return 1;
    case 3:
        if (call(e, 0x8BE10)) {
            if (!usable(e, edi) || !call(e, 0x923F0)) return ebx;
            call(e, 0x14A10, 0x1C, 5);
            release(e, edi);
            *e->consume = 1;
            return 1;
        }
        if (*e->shopMode >= 0x13) return ebx;
        call(e, 0x95F80, 0, edi, *e->slideB, 0, 0, 0, player, 1, *e->slideA);
        return ebx;
    case 4:
        if (*e->shopMode == 4) {
            call(e, 0x95F80, 0, s4, *e->slideB, 0, 0, 0, player, 1, *e->slideA);
            return ebx;
        }
        if (!usable(e, s4) || !call(e, 0x923F0)) return ebx;
        call(e, 0x14A10, 0x1C, 4);
        release(e, s4);
        *e->consume = 1;
        return 1;
    case 5:
        if (!usable(e, edi) || !call(e, 0x923F0)) return ebx;
        call(e, 0x14910, 0x1D, unitId(e, esi), 5);
        release(e, esi);
        sound(e, esi);
        return 1;
    case 6:
        if (!usable(e, esi) || !usable(e, s4) || !usable(e, edi) || !call(e, 0x923F0)) return ebx;
        call(e, 0x14910, 0x1E, unitId(e, esi), 5);
        release(e, esi);
        release(e, s4);
        release(e, edi);
        return 1;
    default: {   // 7
        if (!usable(e, esi) || !call(e, 0x923F0) || *e->shopMode == 0xB) return ebx;
        call(e, 0x14910, 0x21, unitId(e, esi), unitId(e, edi));
        release(e, esi);
        sound(e, esi);
        return 1;
    }
    }
}
}
