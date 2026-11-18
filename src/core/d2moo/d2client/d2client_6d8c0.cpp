// 1.13c D2Client+0x6D8C0 (82 B, EAX = index, ECX = kind): kind 0 and 2 take a record from D2Common (thunks 0xCBBC / 0xCBCE,
// stack index and 0) and answer its dword +0x20 (0 when null); kind 1 reads sgptDataTables (+0xA78 rows of 0x1A8, count
// +0xA80) and answers the row's dword +0x10 (0 when out of range or the row address is 0); any other kind answers 0.
#include <emscripten.h>
#include <cstdint>
extern "C" uint32_t d2c_port_6d8c0(int32_t idx, uint32_t kind, const uint8_t* dt, uint32_t (*cbbc)(uint32_t idx, uint32_t z),
                                   uint32_t (*cbce)(uint32_t idx, uint32_t z)) {
    uint32_t rec;
    if (kind == 0) rec = cbbc((uint32_t)idx, 0);
    else if (kind == 1) {
        if (idx < 0) return 0;
        if (*(const int32_t*)(dt + 0xa80) <= idx) return 0;
        uint32_t row = (uint32_t)*(const uint32_t*)(dt + 0xa78) + (uint32_t)idx * 0x1a8u;
        if (row == 0) return 0;
        return *(const uint32_t*)(uintptr_t)(row + 0x10);
    } else if (kind == 2) rec = cbce((uint32_t)idx, 0);
    else return 0;
    if (rec == 0) return 0;
    return *(const uint32_t*)(uintptr_t)(rec + 0x20);
}
