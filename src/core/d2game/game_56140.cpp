// 1.13c D2Game+0x56140: packet-envelope dispatcher. Verified against the
// 1.13c decompile: size<8 or a bad tag -> 9, else tail-call 0x55E10 (selector < 0x5C) or 0xEDB50 and
// return its result. The two callees are UNMAPPED, so they arrive as function pointers; the FN probe
// passes recorders. The packet is opaque u32 words and the body dereferences no struct field, so there
// are no offsets to pin with static_assert.
#include <cstdint>
namespace {
constexpr uint32_t kEnvelopeMagic = 0xAA55AA55u;   // u32 at pkt+0; D2SMAGIC_HEADER, reused as the tag
constexpr uint32_t kBigSelector   = 0x5Cu;         // pkt[1] is unsigned: 0x5C and up -> 0xEDB50
constexpr uint32_t kReject        = 9u;            // returned when size < 8 or the tag is wrong
}
typedef uint32_t (*Gm56140Callee)(uint32_t a0, uint32_t a1, uint32_t size, const uint32_t* pkt, uint32_t ebx);
uint32_t GAME_56140(uint32_t a0, uint32_t a1, const uint32_t* pkt, uint32_t size, uint32_t ebx, Gm56140Callee small, Gm56140Callee big) {
    if (size < 8 || pkt[0] != kEnvelopeMagic) return kReject;
    return (pkt[1] < kBigSelector ? small : big)(a0, a1, size, pkt, ebx);
}
