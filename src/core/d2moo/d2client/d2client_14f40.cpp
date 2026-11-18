// 1.13c D2Client+0x14F40 (130 B): fastcall(ECX = unit, EDX = skill-record index), one unused stack word (ret 4).
// cltdofunc 16 (Jab's `dosound`): only a unit whose action-frame byte (+0x4E) is 3 does anything.
//   idx outside 0..*(dt+0xBA0)-1, or a null record -> return 0.
//   record = *(dt+0xB98) + idx * 0x23C. unit dword +0 == 0 and the i16 at rec+0x102 > 0 -> play(unit, id);
//   unit dword +0 == 1 and the i16 at rec+0x104 > 0 -> play(unit, id). Return 1 (also when the frame byte is not 3).
// The callee 0x88A70 takes (unit, 0, 0, 0) on the stack (ret 0x10) with the sound id in EBX (movsx of the i16).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Client14f40Env {
    const uint8_t* dt;                                             // *sgptDataTables
    const uint8_t* (*record_base)(const uint8_t* dt);             // the pointer stored at dt+0xB98 (0 = none)
    void (*play)(const void* unit, int32_t soundId);              // 0x88A70, EBX = soundId
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_14f40(const uint8_t* unit, int32_t idx, const D2Client14f40Env* env) {
    if (unit[0x4E] != 3) return 1;
    int32_t count; std::memcpy(&count, env->dt + 0xBA0, 4);
    if (idx < 0 || count <= idx) return 0;
    const uint8_t* base = env->record_base(env->dt);
    const uint8_t* rec = base + (uint32_t)idx * 0x23Cu;
    if (rec == nullptr) return 0;   // retail tests the sum, which only vanishes for a null base and idx 0
    uint32_t kind; std::memcpy(&kind, unit, 4);
    int16_t s;
    std::memcpy(&s, rec + 0x102, 2);
    if (kind == 0 && s > 0) env->play(unit, s);
    std::memcpy(&s, rec + 0x104, 2);
    if (kind == 1 && s > 0) env->play(unit, s);
    return 1;
}
