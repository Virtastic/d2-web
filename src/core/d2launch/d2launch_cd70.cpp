// 1.13c D2Launch+0xCD70  sub_CD70: character-select entry. EDI = the character name (implicit register input, kept for the callees).
// AE60, clears 0x25EC4, D2Win 10147 (ECX = the control at 0x25AAC, EDX = 1), Storm 502 hash of the name (stdcall name, 0, 0),
// GetTickCount, then the per-key cache list at 0x25EF4 ({hash, data, expiry, next}, retail 32-bit pointers): the first node whose
// hash matches and whose expiry is not past (signed tick <= expiry) with non-null data hands (name, data, 0) to CB00 and returns.
// Otherwise slot +0x68 of the BNET object at 0x256C4 (EAX = object): 0 -> 15090(Lang 10003(ECX = 0x14E3), 0x6FA57DF0, 1);
// non-zero -> 0x25EB8 := 1, 0x25EBC := the name. Module addresses travel as their retail virtual addresses (image base 0x6FA40000).
#include <cstdint>

struct D2lCd70Seams {
    void (*ae60)();
    void (*t9862)(uint32_t ecx, uint32_t edx);                 // D2Win 10147, fastcall
    uint32_t (*t97ae)(uint32_t name, uint32_t z0, uint32_t z1); // Storm 502, stdcall
    uint32_t (*tick)();                                        // kernel32 GetTickCount (IAT 0x1C364)
    uint32_t (*vcall68)(uint32_t obj);                         // [obj]+0x68, EAX = obj
    uint32_t (*t9b32)(uint32_t ecx);                           // D2Lang 10003, fastcall
    void (*f15090)(uint32_t text, uint32_t cb, uint32_t one);  // stdcall 3
    void (*fcb00)(uint32_t name, uint32_t data, uint32_t z);   // stdcall 3
};
struct D2lCd70G { uint32_t* g256c4; uint32_t* g25aac; uint32_t* g25eb8; uint32_t* g25ebc; uint32_t* g25ec4; uint32_t* g25ef4; };

extern "C" void d2l_cd70(const D2lCd70Seams* s, const D2lCd70G* g, uint32_t edi) {
    s->ae60();
    *g->g25ec4 = 0;
    s->t9862(*g->g25aac, 1);
    const uint32_t h = s->t97ae(edi, 0, 0);
    const int32_t now = (int32_t)s->tick();
    for (const uint32_t* n = (const uint32_t*)(uintptr_t)*g->g25ef4; n; n = (const uint32_t*)(uintptr_t)n[3]) {
        if (n[0] == h && now <= (int32_t)n[2]) {
            if (n[1]) { s->fcb00(edi, n[1], 0); return; }
            break;
        }
    }
    if (s->vcall68(*g->g256c4) == 0) { s->f15090(s->t9b32(0x14E3), 0x6FA57DF0u, 1); return; }
    *g->g25eb8 = 1;
    *g->g25ebc = edi;
}
