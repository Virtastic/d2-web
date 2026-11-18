// 1.13c Storm+0x11160: deliver the queued SNet messages that are next in sequence (EDI = the session object).
// The queue's TSExplicitList head terminator is {prevlink @+0x164, first @+0x168}; +0x1D4 is the u16 next sequence number.
// While first > 0 and the packet (node[3]) has u16 +4 equal to it: mark the packet type (byte +9, below 0x10) in the dword array at Storm+0x55178,
// post 0x35DD0 (0x534E4554, 2, type, {node[2], packet+0xC, u16(packet+2)-0xC, bytes {packet[10], type}}), bump the sequence, hand
// (node[2], node[3], node[4]) to the provider callback (table DAT 0x55834, slot +0xC; never null-checked), unlink (0x4D60) and free the node (AUMESSAGE).
#include <cstdint>
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
void Storm_Sub4D60(uint8_t* node);
extern "C" void Storm13c_11160(uint32_t* obj, uint32_t* typeFlags /* dwords at Storm+0x55178 */,
                               void (*f35dd0)(uint32_t, uint32_t, uint32_t, uint32_t*), void (*providerCb)(uint32_t, uint32_t, uint32_t)) {
    uint16_t* seq = (uint16_t*)((uint8_t*)obj + 0x1D4);
    for (;;) {
        const int32_t first = (int32_t)obj[0x168 / 4];
        if (first <= 0) return;
        uint32_t* n = (uint32_t*)(uintptr_t)(uint32_t)first;
        uint8_t* pk = (uint8_t*)(uintptr_t)n[3];
        if (*(uint16_t*)(pk + 4) != *seq) return;
        const uint8_t type = pk[9];
        if (type < 0x10) typeFlags[type] = 1;
        uint32_t rec[4];
        rec[0] = n[2];
        rec[1] = (uint32_t)(uintptr_t)(pk + 0xC);
        rec[2] = (uint32_t)(*(uint16_t*)(pk + 2)) - 0xC;
        rec[3] = (uint32_t)pk[10] | ((uint32_t)type << 8);
        f35dd0(0x534E4554u, 2, type, rec);
        *seq = (uint16_t)(*seq + 1);
        providerCb(n[2], n[3], n[4]);
        Storm_Sub4D60((uint8_t*)n);
        Storm_Sub1B960(n, "__AUMESSAGE__", -2);
    }
}
