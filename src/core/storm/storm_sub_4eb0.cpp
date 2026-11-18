// 1.13c Storm+0x4eb0: reads word [1] of the object (esi, or base+4 when esi is null),
// runs two callees on esi (Storm+0x4D60, Storm+0x1B960 with "__AUMESSAGE__", -2), returns that word clamped to >= 0.
#include <cstdint>

struct StormSub4eb0Env {
    void (*sub_4d60)(int32_t* esi);                              // Storm+0x4D60
    void (*sub_1b960)(int32_t* esi, const char* name, int32_t n);  // Storm+0x1B960
};

extern "C" uint32_t Storm_Sub4EB0(int32_t* base, int32_t* esi, const StormSub4eb0Env* e) {
    int32_t* p = esi ? esi : base + 1;  // retail: param_1 + 4 bytes
    uint32_t v = static_cast<uint32_t>(p[1]);
    e->sub_4d60(esi);
    e->sub_1b960(esi, "__AUMESSAGE__", -2);
    return static_cast<int32_t>(v) < 1 ? 0 : v;
}
