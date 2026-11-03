// 1.13c D2Client+0x39910 -- load the default key-bindings table. stdcall, one stack param `fromFile`.
// 0xD1E0(buf, 0) first (a path/profile string into a local). With fromFile set: 0xBFC8 fills the 0xDC-byte path, "default.key" (12 bytes at 0xD684C) is
// appended and it is opened for reading (0xC02E, access 0x80000000, disposition 3, flags 0x80); then 0x47A bytes are read (0xC034) and the file closed (0xC03A).
// A full read is copied into a fresh 0x47A-byte allocation (0xC010, "..\Source\D2Client\UI\CmdTbl.cpp" line 0x527). Any failure (no param, open -1,
// short read) builds "%s\\CMD\\%s\\default.key" (DATA\LOCAL, profile) and loads it with 0xD408 + 0xD810. A block with header {0x5357, 0x25, 0x47A}
// has its 0x474-byte table copied to 0xF4A98 and is freed with line 0x544; any other block is freed with line 0x549.
#include <cstdint>
#include <cstring>
struct D2c39910Env {
    void (*f10004)();                                                                  // 0xD1E0 (stack: buf, 0)
    void (*getDir)(uint8_t* buf, uint32_t size);                                       // 0xBFC8
    int32_t (*open)(const uint8_t* path, uint32_t access, uint32_t disp);              // 0xC02E
    void (*read)(uint32_t handle, uint8_t* buf, uint32_t len, uint32_t* count);        // 0xC034 (ECX, EDX, stack len, &count, 0)
    void (*close)(uint32_t handle);                                                    // 0xC03A
    uint8_t* (*alloc)(uint32_t size, uint32_t line);                                   // 0xC010 (ECX 0, EDX size, stack file, line, 0)
    void (*sprintf)();                                                                 // 0x3146 cdecl: (buf, fmt 0xD6810, 0xD349C, profile)
    uint32_t (*f10016)();                                                              // 0xD408 (stack 0)
    uint8_t* (*loadFile)(const uint8_t* path, uint32_t arg);                           // 0xD810 (EAX path, stack arg)
    void (*release)(uint8_t* p, uint32_t line);                                        // 0xBF86 (ECX p, EDX file 0xD6828, stack line, 0)
};
extern "C" void d2c_39910(uint32_t fromFile, uint8_t* table, const D2c39910Env* env) {
    static const char kDefaultKey[12] = { 'd','e','f','a','u','l','t','.','k','e','y','\0' };
    uint8_t path[0xdc + 16]; uint8_t rbuf[0x480]; uint8_t* blk = nullptr;
    uint32_t count = 0;
    env->f10004();
    bool done = false;
    if (fromFile) {
        std::memset(path, 0, sizeof path);
        env->getDir(path, 0xdc);
        uint32_t n = 0; while (path[n]) ++n;
        std::memcpy(path + n, kDefaultKey, 12);
        int32_t h = env->open(path, 0x80000000u, 3);
        if (h != -1) {
            env->read((uint32_t)h, rbuf, 0x47a, &count);
            env->close((uint32_t)h);
            if (count == 0x47a) {
                blk = env->alloc(0x47a, 0x527);
                std::memcpy(blk, rbuf, 0x47a);
                done = true;
            }
        }
    }
    if (!done) {
        env->sprintf();
        uint32_t v = env->f10016();
        blk = env->loadFile(path, v);
    }
    if (!blk) return;
    uint16_t a, b, c; std::memcpy(&a, blk, 2); std::memcpy(&b, blk + 2, 2); std::memcpy(&c, blk + 4, 2);
    uint32_t line;
    if (a == 0x5357 && b == 0x25 && c == 0x47a) { std::memcpy(table, blk + 6, 0x474); line = 0x544; }
    else line = 0x549;
    env->release(blk, line);
}
