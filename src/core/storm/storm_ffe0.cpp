// 1.13c Storm+0xFFE0 -- provider-record comparator (register args: a=ECX, b=EAX).
#include <cstdint>
extern "C" int Storm_stricmp_5C57(const char* a, const char* b);

static int32_t Rank_FFE0(uint32_t id) {
    switch (id) {
        case 0x424e4554: return 0;  // 'BNET'
        case 0x4950584e: return 1;  // 'IPXN'
        case 0x49505857: return 2;  // 'IPXW'
        case 0x4d4f444d: return 3;  // 'MODM'
        case 0x5343424c: return 4;  // 'SCBL'
        case 0x4d534450: return 5;  // 'MSDP'
    }
    return 0x7fffffff;
}

extern "C" int Storm_Sub_FFE0(const uint8_t* a, const uint8_t* b) {
    int32_t ra = Rank_FFE0(*(const uint32_t*)(a + 0x110));
    int32_t rb = Rank_FFE0(*(const uint32_t*)(b + 0x110));
    if (ra == 0x7fffffff && rb == 0x7fffffff)
        return Storm_stricmp_5C57((const char*)(a + 0x114), (const char*)(b + 0x114));
    return rb - ra;
}
