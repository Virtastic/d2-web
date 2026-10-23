// 1.13c D2Win+0x57A8 (sub_57A8): CRT setSBUpLow, fill the single-byte ctype flag (+1 indexed) and
// case-map tables for the current code page. OS calls injected via env.
#include <cstdint>
#include <cstring>

struct D2Win57A8Env {
    // GetCPInfo: false on failure, else fills 12 LeadByte bytes (pairs, 0-terminated).
    bool (*getCPInfo)(uint8_t leadByte[12]);
    void (*getStringType)(const char* in256, uint16_t out256[256]);       // CT_CTYPE1
    void (*mapString)(uint32_t flags, const char* in256, char* out256);   // 0x100 lower, 0x200 upper
};

// flags: 257 bytes (table at DAT_6f9aa8e0, entry i+1 for char i); map: 256 bytes (DAT_6f9aaa00).
void d2win_sub_57a8(uint8_t* flags, uint8_t* map, const D2Win57A8Env& env) {
    uint8_t lead[12];
    if (env.getCPInfo(lead)) {
        char s[256], lo[256], up[256];
        uint16_t t[256];
        for (int i = 0; i < 256; i++) s[i] = (char)i;
        s[0] = ' ';
        for (int i = 0; i < 12 && lead[i]; i += 2)
            for (int c = lead[i]; c <= lead[i + 1]; c++) s[c] = ' ';
        env.getStringType(s, t);
        env.mapString(0x100, s, lo);
        env.mapString(0x200, s, up);
        for (int i = 0; i < 256; i++) {
            if (t[i] & 1) { flags[i + 1] |= 0x10; map[i] = (uint8_t)lo[i]; }
            else if (t[i] & 2) { flags[i + 1] |= 0x20; map[i] = (uint8_t)up[i]; }
            else map[i] = 0;
        }
    } else {
        for (int i = 0; i < 256; i++) {
            if (i >= 'A' && i <= 'Z') { flags[i + 1] |= 0x10; map[i] = i + 0x20; }
            else if (i >= 'a' && i <= 'z') { flags[i + 1] |= 0x20; map[i] = i - 0x20; }
            else map[i] = 0;
        }
    }
}
