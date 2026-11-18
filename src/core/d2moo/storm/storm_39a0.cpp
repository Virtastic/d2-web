// 1.13c Storm+0x39A0: constructor of the 0x168-byte object; every word 0..0x59 is zeroed (16 element ctors 0x3630 zero 5 words each, then 0x37C0 zeroes +0x140 of the element block, then four words at +0x158).
#include <cstdint>
extern "C" uint32_t* Storm13c_39A0(uint32_t* obj) {
    for (int i = 0; i < 0x5a; i++) obj[i] = 0;
    return obj;
}
