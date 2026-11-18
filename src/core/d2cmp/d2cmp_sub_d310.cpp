// D2CMP sprite-cache frame lookup wrapper (EAX = sprite, EDI = frame request in retail).
#include <cstdint>

// 1.13c D2CMP+0x0D310
int d2cmp_d310(uint8_t* spr, uint32_t* req, uint8_t* (*dataBase)(), int (*d140)(uint32_t*))
{
    if (!spr || !*(uint32_t*)(spr + 0xb0)) return 0;
    if (!dataBase()) return 0;  // Ordinal_10094
    req[0xc] = spr[0x24];
    return d140(req);
}
