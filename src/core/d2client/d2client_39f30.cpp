// 1.13c D2Client+0x39F30 -- show the key bound to (cmd, mod): look the pair up in the 114-entry bind table (0x11A2E0, stride 10:
// cmd dword, key word, mod dword); key 0xFFFF when absent. A key in 8..0x104 with a jump-table entry shows the string id for that key via
// the D2Lang-style Ordinal_10003 thunk (0xD1EC, ECX = id); everything else goes to 0x391E0(ESI=cmd, EDX=mod).
#include <cstdint>
#include <cstring>
struct D2c39F30Env {
    void (*ord10003)(uint32_t ecx);
    void (*f391e0)(uint32_t cmd, uint32_t mod);
};
static uint32_t d2c_39f30_id(uint32_t key) {
    switch (key) {
    case 0x8: return 0xf2c; case 0x10: return 0xf2f; case 0x11: return 0xf30; case 0x1b: return 0xf1e; case 0x21: return 0xf34;
    case 0x22: return 0xf35; case 0x2c: return 0xf37; case 0x2d: return 0xf38; case 0x2e: return 0xf39; case 0x60: return 0xf3b;
    case 0x61: return 0xf3c; case 0x62: return 0xf3d; case 0x63: return 0xf3e; case 0x64: return 0xf3f; case 0x65: return 0xf40;
    case 0x66: return 0xf41; case 0x67: return 0xf42; case 0x68: return 0xf43; case 0x69: return 0xf44; case 0x6a: return 0xf45;
    case 0x6b: return 0xf46; case 0x6c: return 0xf48; case 0x6d: return 0xf47; case 0x6f: return 0xf49; case 0x90: return 0xf2b;
    case 0x91: return 0xf4a; case 0x100: return 0xf15; case 0x101: return 0xf16; case 0x102: return 0xf17; case 0x103: return 0xf18;
    case 0x104: return 0xf19; default: return 0;
    }
}
extern "C" void d2c_39f30(const uint8_t* table, uint32_t cmd, uint32_t mod, const D2c39F30Env* env) {
    uint32_t key = 0xffff;
    for (uint32_t i = 0; i < 0x72; ++i) {
        uint32_t c, m; uint16_t k;
        std::memcpy(&c, table + i * 10, 4); std::memcpy(&k, table + i * 10 + 4, 2); std::memcpy(&m, table + i * 10 + 6, 4);
        if (c == cmd && m == mod) { key = k; break; }
    }
    uint32_t id = d2c_39f30_id(key);
    if (id) env->ord10003(id); else env->f391e0(cmd, mod);
}
