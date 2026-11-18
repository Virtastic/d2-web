// 1.13c BNClient+0x11B00 (stdcall, ret 4): compose and send frame 0x58, 0x58 payload bytes.
// The stack frame retail builds, in order:
//   seed[0x1C] = { GetTickCount(), arg, H1[20] }   H1 = broken_hash(lowercase(copy of the name at DAT_1ECB8), strlen)
//   body[0x58] = { the same tick, H2[20], slot[0x40] }   H2 = broken_hash(seed, 0x1C), slot = a copy of DAT_1EC58[0x40]
// then DAT_1EC58[0x40] is zeroed and EB00 sends body (ECX = 0x58, EDI = body, id 0x58 on the stack). Returns 0.
// The lowercase runs over the SStrCopy count (CharLowerBuffA), the hash over SStrLen of the copy; both are seams so the probe follows retail.
#include <cstdint>
#include <cstring>

struct Bn11B00Env {
    uint32_t (*tick)();                                         // kernel32 GetTickCount
    uint32_t (*scopy)(char* dst, const char* src, uint32_t max); // 0x804E Storm #501 SStrCopy, answers the count copied
    void (*lower)(char* s, uint32_t n);                         // user32 CharLowerBuffA
    uint32_t (*slen)(const char* s);                            // 0x8042 Storm #506 SStrLen
    void (*hash)(uint32_t* out, const void* in, uint32_t n);    // 0x172C0 the broken SHA-1 (stdcall ret 0xC)
    void (*mcopy)(void* dst, const void* src, uint32_t n);      // 0x807E Storm #491 SMemCopy
    void (*zero)(void* dst, uint32_t n);                        // 0x806C Storm #494 SMemZero
    void (*send)(uint32_t len, uint32_t id, const uint8_t* p);  // 0xEB00 (ECX = len, EDI = payload, id on the stack)
};

// 1.13c BNClient+0x11B00
extern "C" uint32_t d2_bnclient_11b00(uint32_t arg, const char* name, uint8_t* slot, const Bn11B00Env* e) {
    uint32_t seed[7];
    alignas(4) uint8_t body[0x58];
    char buf[0x100];
    const uint32_t tick = e->tick();
    seed[0] = tick; seed[1] = arg;
    const uint32_t n = e->scopy(buf, name, 0x100);
    e->lower(buf, n);
    e->hash(seed + 2, buf, e->slen(buf));
    memcpy(body, &tick, 4);
    e->hash((uint32_t*)(body + 4), seed, 0x1C);
    e->mcopy(body + 0x18, slot, 0x40);
    e->zero(slot, 0x40);
    e->send(0x58, 0x58, body);
    return 0;
}
