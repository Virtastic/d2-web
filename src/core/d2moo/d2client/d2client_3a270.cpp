// 1.13c D2Client+0x3A270 -- rebind command `cmd` (mod `mod`) to `key`. EAX = out (word*, may be null), ECX = cmd, stack (mod, key), ret 8.
// Clears *out; cmd >= 0x38 returns 0. 0x38CD0(key) must accept the key (else *out = 0xF8B, 0); 0x39BA0(key, out, cmd) must accept it (else 0, *out as it set).
// Key 0x2C is refused (*out = 0xF8B) when the command's cmd-table word (0xF4F10 + 12*cmd) is set. The first bind carrying the key is unbound (0xFFFF),
// then the first (cmd, mod) bind takes the key and the result is 1; with no such bind the unbound entry gets its key back and the result is 0.
#include <cstdint>
#include <cstring>
struct D2c3A270Env {
    uint32_t (*f38cd0)(uint32_t key);
    uint32_t (*f39ba0)(uint32_t key, uint16_t* out, int32_t cmd);
};
extern "C" uint32_t d2c_3a270(const uint8_t* cmdtab, uint8_t* bind, uint16_t* out, int32_t cmd, uint32_t mod, uint32_t keyw, const D2c3A270Env* env) {
    const uint16_t key = (uint16_t)keyw;
    if (out) *out = 0;
    if (cmd >= 0x38) return 0;
    if (!env->f38cd0(keyw)) { if (out) *out = 0xf8b; return 0; }
    if (!env->f39ba0(keyw, out, cmd)) return 0;
    if (key == 0x2c) {
        uint32_t h; std::memcpy(&h, cmdtab + (int64_t)cmd * 12, 4);
        if (h) { if (out) *out = 0xf8b; return 0; }
    }
    uint32_t old = 0x72;
    for (uint32_t i = 0; i < 0x72; ++i) {
        uint16_t k; std::memcpy(&k, bind + i * 10 + 4, 2);
        if (k == key) { const uint16_t ff = 0xffff; std::memcpy(bind + i * 10 + 4, &ff, 2); old = i; break; }
    }
    for (uint32_t i = 0; i < 0x72; ++i) {
        uint32_t c, m; std::memcpy(&c, bind + i * 10, 4); std::memcpy(&m, bind + i * 10 + 6, 4);
        if (c == (uint32_t)cmd && m == mod) { std::memcpy(bind + i * 10 + 4, &key, 2); return 1; }
    }
    if (old != 0x72) std::memcpy(bind + old * 10 + 4, &key, 2);
    return 0;
}
