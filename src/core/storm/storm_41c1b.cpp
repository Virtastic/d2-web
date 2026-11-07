// 1.13c Storm+0x41C1B: EH unwind funclet (push ebp - 0x3a80, call 0x4B40); ebp is the unwound frame (a 32-bit address in retail's layout).
#include <cstdint>
struct StormSub4910Env { void (*memberDtor)(uint8_t*); void (*elemDtor)(uint8_t*); };
extern "C" void Storm_Sub4910(const StormSub4910Env& e, uint8_t* self);
extern "C" void Storm_Sub4B40(intptr_t arg, void (*sub4910)(intptr_t));
void Storm_Sub4520_Own(uint8_t* p);
void Storm_Sub47D0(intptr_t* e);
static void member4520(uint8_t* m) { Storm_Sub4520_Own(m); }
static void elem47D0(uint8_t* e) { Storm_Sub47D0((intptr_t*)e); }
static void sub4910(intptr_t p) { const StormSub4910Env e = { member4520, elem47D0 }; Storm_Sub4910(e, (uint8_t*)p); }
void Storm_Unwind_41C1B(uint32_t ebp) {
    Storm_Sub4B40((intptr_t)(ebp - 0x3a80), sub4910);
}
