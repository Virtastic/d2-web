// BNClient.dll alpha-lane batch b61-bnc-a: two rows transcribed from the cached decompile.
// 0x0FE00/0x0FEC0 have no cached decompile; 0x11780/0x11F80/0x12000 are register-argument;
// 0x11960 is direct Win32; 0x11B00/0x11CB0/0x11DC0/0x11E60 have garbled stack decompiles. Skipped.
#include <cstdint>
#include <cstring>
#include <initializer_list>

// 1.13c BNClient+0x11BB0
// Fills 5 dwords via FUN_6FF2A610(DAT_6FF3ECF8, DAT_6FF3ED40, DAT_6FF3ED18), compares to param.
bool bn_sub_11BB0(const int32_t* param, void (*fill)(int32_t out[5])) {
    int32_t local[5];
    fill(local);
    for (int i = 0; i < 5; ++i)
        if (local[i] != param[i]) return false;
    return true;
}

struct BnB61State {
    int32_t once_count;     // DAT_6FF3EB50
    int32_t connected;      // DAT_6FF3F654
    int32_t blocked;        // DAT_6FF3F660
    int32_t pending_flag;   // DAT_6FF3F644
    uint8_t* send_buf;      // Ordinal_491 destination
};

// 1.13c BNClient+0x11BF0
// Sends BNCS 0xFF 0x57 len=4 (empty body) when connected, else sets the pending flag.
uint32_t bn_sub_11BF0(BnB61State& s, int32_t first, void (*pre)(), void (*send)(uint32_t len)) {
    if (first && ++s.once_count > 1) return 0;
    pre();  // FUN_6FF2AED0(&DAT_6FF3F8E4)
    const uint8_t pkt[4] = {0xff, 0x57, 4, 0};
    if (s.connected && !s.blocked) {
        std::memcpy(s.send_buf, pkt, 4);
        send(4);  // Ordinal_10007
        return 1;
    }
    s.pending_flag = 1;
    return 1;
}
