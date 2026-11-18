// 1.13c Fog+0x06A5A: MSVCRT _errno(): returns &__getptd()->_terrno. Wasm build is single-threaded
// per instance, so the per-thread data block reduces to one int.
static int g_fog_terrno;

extern "C" int* fog_errno_6a5a() {  // 1.13c Fog+0x06A5A
    return &g_fog_terrno;
}
