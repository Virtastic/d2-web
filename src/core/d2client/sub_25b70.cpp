// 1.13c D2Client+0x25B70: message filter, fastcall(msg).
#include <cstdint>
// m = msg words (+4 id, +0x10 param). flag = *(u8*)(*(u32*)m[5] + 0x1B5). send() is 0x6B300(m,3,0,0,0).
template <class Send>
void D2Client_Sub25B70(const uint32_t* m, uint8_t flag, Send send) {
    if (!flag) return;
    uint32_t id = m[1], p = m[4];
    if (id == 0x95) { if (p == 1) send(); return; }
    if (id == 0x98) { send(); return; }
    bool ok = id == 0x18d || (id >= 0x162 && id <= 0x164) || (id >= 0x195 && id <= 0x197) || id == 0x1c7;
    if (ok && p == 0) send();
}
