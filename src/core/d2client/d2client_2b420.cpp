// 1.13c D2Client+0x2B420: D2LoadImage . EAX = path string, stack arg (ret 4) goes to the 0xC0BE import (Ordinal 10070),
// which yields a root string. Local 260-byte buffer = path + root (concatenated); then 0xD408 (no stack arguments), 0xD810(buffer in EAX; the 0xD408 value, 0; ret 8)
// gives an archive handle, and the 0xC184 import (6 stack words: archive, &slot, "..\Source\D2Client\CORE\ARCHIVE.CPP", 0x56, -1, buffer) fills `slot`, which is returned.
#include <cstdint>
#include <cstring>
struct D2Client2b420Env {
    uint32_t (*root)(uint32_t arg);                                  // 0xC0BE (Ordinal 10070), 1 stack word
    uint32_t (*d408)();                                              // 0xD408 import thunk, no stack arguments
    uint32_t (*load)(uint32_t buf, uint32_t a1, uint32_t a2);        // 0xD810, buffer in EAX, ret 8
    void (*open)(uint32_t archive, uint32_t* slot, uint32_t file, uint32_t line, uint32_t flags, uint32_t buf); // 0xC184 import, 6 stack words
};
static constexpr uint32_t kFileRva = 0xD6AB0;   // the "..\Source\D2Client\CORE\ARCHIVE.CPP" literal, logged as a module address
static uint32_t cstrLen(uint32_t a) { uint32_t n = 0; while (*(const uint8_t*)(uintptr_t)(a + n)) ++n; return n; }
extern "C" uint32_t d2client_2b420_run(uint32_t path, uint32_t arg, const D2Client2b420Env* env) { // 1.13c D2Client+0x2B420
    char buf[0x104 + 0x100];   // retail's buffer is 260 bytes and the copy is unbounded; the slack only keeps an oversized input inside our frame
    const uint32_t pl = cstrLen(path);
    std::memcpy(buf, (const void*)(uintptr_t)path, pl + 1);
    const uint32_t root = env->root(arg);
    std::memcpy(buf + pl, (const void*)(uintptr_t)root, cstrLen(root) + 1);
    const uint32_t a1 = env->d408();
    const uint32_t archive = env->load((uint32_t)(uintptr_t)buf, a1, 0);
    uint32_t slot = 0;
    env->open(archive, &slot, 0xB2000000u | kFileRva, 0x56, 0xFFFFFFFFu, (uint32_t)(uintptr_t)buf);
    return slot;
}
