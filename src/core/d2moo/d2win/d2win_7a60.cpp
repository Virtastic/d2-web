// 1.13c D2Win+0x7A60 -- fatal-string guard.
// Register arg: ESI = flag. ESI == 0 is fatal (0x59). Otherwise Ordinal_10105 (0x78C2) decides: nonzero
// returns that value in eax, zero is fatal (0x5F). The fatal path never returns in retail.
#include <cstdint>

// probe(esi) is retail's Ordinal_10105 thunk (0x78C2).
extern "C" uint32_t d2win_7A60(uint32_t esi, uint32_t (*probe)(uint32_t esi), void (*fatal)(uint32_t id)) {
    if (esi == 0) { fatal(0x59); return 0; }
    uint32_t r = probe(esi);
    if (r != 0) return r;
    fatal(0x5F);
    return 0;
}
