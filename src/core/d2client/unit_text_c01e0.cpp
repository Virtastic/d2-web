// 1.13c D2Client+0xC01E0: fill the global item-text buffer for an item unit.
#include <cstdint>
struct ClientC01E0Env {
    uint32_t viewMode;      // D2Client+0x11C414
    uint8_t  flag9721;      // D2Client+0x119721
    uint32_t* clearWord;    // D2Client+0x11C780 (first word of the text buffer, written 0)
    void (*build)(const uint32_t* unit, uint16_t* buf, int32_t size);  // D2Client+0x1FDD0 with eax=0
};
extern "C" uint32_t d2_client_c01e0(const uint32_t* unit, uint16_t* buf, const uint8_t* itemData, ClientC01E0Env& e) {
    if (e.viewMode == 3 || e.flag9721) return 0;
    *e.clearWord = 0;
    if (!unit || unit[0] != 1 || !itemData || !(itemData[0x16] & 0x18)) return 0;
    e.build(unit, buf, 0x100);
    return 1;
}
