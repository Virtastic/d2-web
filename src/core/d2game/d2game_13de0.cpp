// 1.13c D2Game+0x13DE0 -- act spawn follow-up. EAX = unit, stack word = record (dword +0xC owner, byte
// +0x80 flag 0x20). ordinal 10957(unit) is the act; outside 0..6 or with no #11120 count it ends. #10044
// (act, 0) fetches the act record. Format 0 (#10718) -> 0x135E0(EAX = owner, stack = record); else
// 0x13320(owner, record, flag ? owner : 0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game13DE0Env {
    int32_t  (*ordinal10957)(uint32_t unit);
    int32_t  (*ordinal11120)(int32_t act);
    uint32_t (*ordinal10044)(int32_t act, uint32_t zero);
    uint16_t (*ordinal10718)(uint32_t unit);
    void (*sub135E0)(uint32_t owner, uint32_t rec);
    void (*sub13320)(uint32_t owner, uint32_t rec, uint32_t third);
};
extern "C" void d2_game_13DE0(uint32_t unit, const uint8_t* rec, const D2Game13DE0Env* env)
{
    const int32_t act = env->ordinal10957(unit);
    if (act < 0 || act >= 7) return;
    if (env->ordinal11120(act) <= 0) return;
    const uint32_t actRec = env->ordinal10044(act, 0);
    uint32_t owner; std::memcpy(&owner, rec + 0xC, 4);
    if (env->ordinal10718(unit) == 0) { env->sub135E0(owner, actRec); return; }
    env->sub13320(owner, actRec, (rec[0x80] & 0x20) ? owner : 0);
}
