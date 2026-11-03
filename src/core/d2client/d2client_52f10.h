// D2Client 0x52F10 state and callee interface (shared with the FN probe src/core/fn/d2client_52f10.cpp).
#pragma once
#include <cstdint>
#include <cstddef>
namespace d2client_52f10 {
struct State {
    int32_t c960c, c9610; uint8_t c9614, c9615, c9616, pad17; int32_t c9618, c961c, c9620, c9624, c9628, c962c, c9630, c9634;
};
static_assert(sizeof(State) == 44, "0x6FBC960C block");
static_assert(offsetof(State, c9630) == 0x24, "c9630");
struct Env {
    virtual ~Env() = default;
    virtual uint32_t f59a20(uint8_t* r, uint8_t* g, uint8_t* b) = 0;  // returns the dword stored through the local
    virtual int32_t cosine(int32_t n) = 0;                           // Fog 10083 as the float bit pattern
    virtual int32_t lookup(int32_t id) = 0;                          // Ordinal 10331, then 10826 on the result (0 = none)
    virtual bool unit(int32_t id) = 0;
};
void progress(State& s, int32_t cbbfc, Env& env);
}  // namespace d2client_52f10
