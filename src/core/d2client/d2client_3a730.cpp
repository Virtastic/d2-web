// 1.13c D2Client+0x3A730 -- load the saved key bindings. cdecl, no arguments.
// Opens the profile bindings (0x38E80 with ESI=3, EBX=0x80000000), reads 0x476 bytes into {u16 version; 114 x (u32 cmd, u16 key, u32 mod)}, closes.
// Accepted when the count is 0x476, version == 0x25, every command 0..0x38 is bound by some entry and no key other than 0xFFFF repeats; then every key of the
// live bind table (0x11A2E0) is set to 0xFFFF, the 1140 bytes are copied over it and 0x3A3A0 runs. Otherwise 0x39910(1), the keys are cleared, the defaults at 0xF4A98 are copied in, 0x3A3A0 runs and 0x39A60 writes the file back.
#include <cstdint>
#include <cstring>
struct D2c3A730Env {
    int32_t (*open)(uint32_t esi, uint32_t ebx);                                   // 0x38E80
    void (*read)(uint32_t handle, uint8_t* buf, uint32_t len, uint32_t* count);    // 0xC034
    void (*close)(uint32_t handle);                                                // 0xC03A
    void (*f3a3a0)();                                                              // 0x3A3A0
    void (*f39910)(uint32_t);                                                      // 0x39910
    void (*f39a60)();                                                              // 0x39A60
};
static void clearKeys(uint8_t* bind) { for (uint32_t i = 0; i < 114; ++i) { bind[i * 10 + 4] = 0xff; bind[i * 10 + 5] = 0xff; } }
extern "C" void d2c_3a730(uint8_t* bind, const uint8_t* defaults, const D2c3A730Env* env) {
    uint8_t buf[0x480]; uint32_t count = 0;
    std::memset(buf, 0, sizeof buf);
    int32_t h = env->open(3, 0x80000000u);
    bool ok = false;
    if (h != -1) {
        env->read((uint32_t)h, buf, 0x476, &count);
        env->close((uint32_t)h);
        uint16_t ver; std::memcpy(&ver, buf, 2);
        if (count == 0x476 && ver == 0x25) {
            const uint8_t* t = buf + 2;
            ok = true;
            for (uint32_t c = 0; c < 0x39 && ok; ++c) {
                bool f = false;
                for (uint32_t j = 0; j < 114 && !f; ++j) { uint32_t v; std::memcpy(&v, t + j * 10, 4); f = v == c; }
                ok = f;
            }
            for (uint32_t i = 0; i < 114 && ok; ++i) {
                uint16_t ki; std::memcpy(&ki, t + i * 10 + 4, 2);
                if (ki == 0xffff) continue;
                for (uint32_t j = 0; j < 114; ++j) {
                    uint16_t kj; std::memcpy(&kj, t + j * 10 + 4, 2);
                    if (j != i && kj == ki) { ok = false; break; }
                }
            }
            if (ok) { clearKeys(bind); std::memcpy(bind, t, 0x474); env->f3a3a0(); return; }
        }
    }
    env->f39910(1);
    clearKeys(bind);
    std::memcpy(bind, defaults, 0x474);
    env->f3a3a0();
    env->f39a60();
}
