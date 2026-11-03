// 1.13c D2Client+0x39BA0: keybind-key validity gate. EAX = key (16-bit), ESI = out word* (may be null), EDI = command index. Returns 1 when the key
// may be bound; else 0 and *ESI = 0xF89 (keys 0x100..0x102 whose 0x42060 / 0x42030 test fails) or 0xF8A (keys 0x103/0x104 when neither 0x420E0 nor
// 0x42090 passes, or the command's table word at D2Client+0xF4F14 + 12*EDI is set). Any other key passes. cmdTbl = that table.
#include "d2c_seam.h"
extern "C" uint32_t d2c_39ba0(const D2cSeam* s, uint32_t eax, uint16_t* esi, int32_t edi, const uint8_t* cmdTbl) {
    const uint32_t k = (uint32_t)(uint16_t)eax - 0x100u;
    if (k > 4) return 1;
    if (k <= 2) {
        if ((uint32_t)(*s)(k == 0 ? 0x42060 : 0x42030)) return 1;
        if (esi) *esi = 0xf89;
        return 0;
    }
    if ((uint32_t)(*s)(0x420e0) || (uint32_t)(*s)(0x42090)) {
        uint32_t h; std::memcpy(&h, cmdTbl + (int64_t)edi * 12, 4);
        if (!h) return 1;
    }
    if (esi) *esi = 0xf8a;
    return 0;
}
