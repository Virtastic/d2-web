// src/core/d2cmp/d2cmp_14f20.cpp -- 1.13c D2CMP.dll 0x14F20, transcribed from the cached Ghidra
// decompile. Alpha batch lane: implementation + marker only, no spec, no vector.
#include <cstdint>
#include <cstring>

// Host seams: Fog file ordinals reached via 0x9070 (open, 10102), 0x8EF0 (read+size check, 10104,
// expects 0x1500 bytes), 0x9040 (close, 10103), and the fatal path (Fog 10265/10024 + _exit(-1)).
struct D2CMP14f20Env {
    void* (*openFile)(const char* path);                    // 0x9070; null on failure
    void  (*readChecked)(void* f, uint32_t size);           // 0x8EF0
    void  (*closeFile)(void* f);                            // 0x9040
    void  (*fatal)(uint32_t msgId);                         // Fog 10265(id) + 10024 + _exit(-1)
};

// 1.13c D2CMP+0x14F20 (418 bytes): load Data\Global\Items\Palette\<name>.dat (0x1500 bytes).
// 1.13c D2CMP+0x14F20
extern "C"
void d2cmp_14f20(const D2CMP14f20Env* env, const char* name, int value) {
    (void)value;
    char path[256];
    strcpy(path, "Data\\Global\\Items\\Palette\\");
    strcat(path, name);
    strcat(path, ".dat");
    void* f = env->openFile(path);
    if (!f) env->fatal(0x39);
    env->readChecked(f, 0x1500);
    env->closeFile(f);
}
