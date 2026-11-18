// 1.13c D2Client+0x39A60 -- write the default key-bindings file. Opens the bindings file with 0x38E80(esi=2, ebx=0x40000000); -1 ends.
// Otherwise writes a 0x476-byte block (word 0x25, then a copy of the 114-entry bind table at 0x11A2E0), closes it, builds "<cwd>default.key"
// (0xBFC8 fills the 0xDC-byte path, the 12 bytes at 0xD684C are appended) and opens it for writing (0xC02E, ECX path, EDX 0x40000000, stack
// 0,0,2,0x80,0); -1 ends. Else writes the 6-byte header {0x5357, 0x25, 0x47A} and the 0x474-byte table to it and closes it.
#include <cstdint>
#include <cstring>
struct D2c39A60Env {
    int32_t (*f38e80)(uint32_t esi, uint32_t ebx);
    void (*write)(uint32_t handle, const uint8_t* buf, uint32_t len);   // 0xC046 (ECX handle, EDX buf, stack len, &written, 0)
    void (*close)(uint32_t handle);                                      // 0xC03A
    void (*getDir)(uint8_t* buf, uint32_t size);                         // 0xBFC8 (ECX buf, EDX 0xDC)
    int32_t (*open)(const uint8_t* path, uint32_t access, uint32_t disp);// 0xC02E
};
extern "C" void d2c_39a60(const uint8_t* table, const D2c39A60Env* env) {
    static const char kDefaultKey[12] = { 'd','e','f','a','u','l','t','.','k','e','y','\0' };   // 0xD684C
    int32_t h = env->f38e80(2, 0x40000000);
    if (h == -1) return;
    uint8_t blk[0x476];
    const uint16_t w25 = 0x25;
    std::memcpy(blk, &w25, 2);
    std::memcpy(blk + 2, table, 0x474);
    env->write((uint32_t)h, blk, 0x476);
    env->close((uint32_t)h);
    uint8_t path[0xdc + 16]; std::memset(path, 0, sizeof path);
    env->getDir(path, 0xdc);
    uint32_t n = 0; while (path[n]) ++n;
    std::memcpy(path + n, kDefaultKey, 12);
    int32_t f = env->open(path, 0x40000000, 2);
    if (f == -1) return;
    const uint16_t hdr[3] = { 0x5357, 0x25, 0x47a };
    env->write((uint32_t)f, (const uint8_t*)hdr, 6);
    env->write((uint32_t)f, table, 0x474);
    env->close((uint32_t)f);
}
