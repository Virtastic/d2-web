// 1.13c D2Client+0x6A220: unit-type dispatcher. EAX=unit, stack=arg (ret 4).
// Callees are returned as events {rva,eax,edi,s0,s1} so the caller supplies the world.
#include <cstdint>
// type/id: the out-pair after D2Common#10350 ran (defaults {unit?unit[0]:6, unit?unit[1]:-1}); flag_ok: 69B40 result;
// slot: unit[0x54] contents (pointer to a byte table) or 0. Returns event count, or -1 for the _exit(-1) path.
extern "C" int32_t d2_client_6a220(uint32_t unit, uint32_t arg, uint32_t type, uint32_t ok69b40, uint32_t tbl,
                                   uint32_t* tblbyte_off, uint32_t* ev) {
    uint32_t n = 0;
    auto E = [&](uint32_t rva, uint32_t a, uint32_t d, uint32_t s0, uint32_t s1) {
        uint32_t* w = ev + n++ * 5; w[0] = rva; w[1] = a; w[2] = d; w[3] = s0; w[4] = s1;
    };
    *tblbyte_off = 0xFFFFFFFFu;
    E(0xC658, unit, 0, 0, 0);               // D2Common#10350 fills {type,id}
    switch (type) {
    case 0:
        E(0x69B40, unit, arg, 0, 0);
        if (ok69b40 && unit && tbl) *tblbyte_off = arg * 2;  // byte at tbl+arg*2 = 1
        return (int32_t)n;
    case 1: E(0x6A180, arg, 0, 0, 0); return (int32_t)n;
    case 2: E(0x69F20, unit, arg, 0, 0); return (int32_t)n;
    default: return -1;
    }
}
