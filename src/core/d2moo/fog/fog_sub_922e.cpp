// 1.13c Fog+0x922e: codepage -> LANGID. EAX in: 0x3a4->0x411, 0x3a8->0x804,
// 0x3b5->0x412, 0x3b6->0x404, else 0. No seams needed (pure).
#include <cstdint>

struct FogCpLangEnv {};  // no external state

extern "C" uint32_t fog_sub_922e(int32_t codepage, const FogCpLangEnv*) {
    switch (codepage) {
        case 0x3a4: return 0x411;
        case 0x3a8: return 0x804;
        case 0x3b5: return 0x412;
        case 0x3b6: return 0x404;
        default: return 0;
    }
}
