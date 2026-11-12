// Callee seam for D2Client ports that call retail callees. Every call a port makes to a function it does not own goes
// through Seam::call, keyed by the callee's D2Client RVA (D2Common import thunks included), with the register and stack
// words retail has live at the call: w = eax ecx edx ebx esi s0..s8 (nine stack words). A port fills what retail loads; the
// binding (a probe, or the engine) reads what it needs.
#pragma once
#include <cstdint>
struct D2cSeam {
    uint64_t (*call)(void* ctx, uint32_t rva, const uint64_t (&w)[14]);
    void* ctx;
    uint64_t operator()(uint32_t rva, uint64_t eax = 0, uint64_t ecx = 0, uint64_t edx = 0, uint64_t ebx = 0, uint64_t esi = 0,
                        uint64_t s0 = 0, uint64_t s1 = 0, uint64_t s2 = 0, uint64_t s3 = 0, uint64_t s4 = 0, uint64_t s5 = 0, uint64_t s6 = 0, uint64_t s7 = 0, uint64_t s8 = 0) const {
        const uint64_t w[14] = { eax, ecx, edx, ebx, esi, s0, s1, s2, s3, s4, s5, s6, s7, s8 };
        return call(ctx, rva, w);
    }
    // The word retail has for a module address (a vtable or string at D2Client+rva): the binding picks the representation (the probe's
    // is D2Client's preferred base plus rva, as in retail's memory); not a callee, never logged.
    uint64_t mod(uint32_t rva) const { return (*this)(0xC0000000u | rva); }
};
// The skills.txt / data-table words these ports read: sgptDataTables+0xB98 rows (stride 0x23C), +0xBA0 count, +0xC4 states,
// +0xBD4 item stat costs, +0xBC0 (a count) and the byte at gdwBitMasks+8.
struct D2cSkillTbl { const uint8_t* rec; int32_t count, nStates, nStats, nBc0; uint8_t bit8; };
#include <cstring>
// Shared prologue of the 0x406xx-0x409xx unit gates: 0x2A0B0(EAX=unit); id = unit+0x2C; a zero id is retail's fatal assert
// (0xBF92(0x946), 0xBF80(fmt, rc), 0x3664(-1), never returns); then 0xC2DA(id) (D2Common #10392) gives the record pointer.
inline uint64_t d2cUnitRes(const D2cSeam& s, const uint8_t* unit) {
    s(0x2a0b0, (uintptr_t)unit);
    uint32_t id; std::memcpy(&id, unit + 0x2c, 4);
    if (!id) { uint64_t rc = s(0xbf92, 0, 0, 0, 0, 0, 0x946); s(0xbf80, 0, 0, 0, 0, 0, 0xB20D359Full, rc); s(0x3664, 0, 0, 0, 0, 0, 0xffffffffull); return 0; }
    return s(0xc2da, 0, 0, 0, 0, 0, id);
}
inline const uint8_t* d2cSkillRec(const D2cSkillTbl& t, int32_t id) {
    if (id < 0 || id >= t.count || !t.rec) return nullptr;
    return t.rec + (uint32_t)id * 0x23c;
}
inline int16_t d2cS16(const uint8_t* r, uint32_t o) { int16_t v; std::memcpy(&v, r + o, 2); return v; }
