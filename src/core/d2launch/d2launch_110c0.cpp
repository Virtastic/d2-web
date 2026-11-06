// 1.13c D2Launch+0x110C0  sub_110C0: when 9B2C (the language id) returns 9, rewrites eighteen layout words in the data range 0x21408..0x2416C
// to their alternate values; EAX is 0x22B on that path and 9B2C's value otherwise. g is that range from 0x21408.
#include <cstdint>
#include "d2launch_env.h"
extern "C" uint32_t d2l_110c0(const D2lSeams* s, uint32_t* g) {
    auto W = [&](uint32_t rva) -> uint32_t& { return g[(rva - 0x21408u) / 4]; };
    const uint32_t v = s->t9b2c();
    if (v != 9) return v;
    W(0x21E58) = 0x9B;  W(0x21408) = 0x14E; W(0x21438) = 0x184; W(0x21468) = 0x1BA; W(0x214F8) = 0xF0;
    W(0x21528) = 0x158; W(0x220F8) = 0x9C;  W(0x234D8) = 0x1FA; W(0x23508) = 0x22B; W(0x23298) = 0x23F;
    W(0x236B8) = 0x22B; W(0x232F8) = 0x22B; W(0x23358) = 0x23F; W(0x233B8) = 0x253; W(0x23FB8) = 0x46;
    W(0x24168) = 0xEE;
    return 0x22B;
}
