// 1.13c D2Client+0x3F3D0 (302 B): spinner-control constructor, transcribed from the cached decompile.
// Object = uint32 array (retail 32-bit layout). Vtable stores and imports are host seams.
#include <cstdint>
#include <cstdio>
#include <emscripten.h>

struct D2ClientSpinnerEnv {
    uint32_t vtblBase, vtblSpinner;                // 0x6FB85048, 0x6FB86790
    uint32_t (*sub2b420)(uint32_t zero);           // FUN_6fadb420, returns char* handle
    uint32_t (*ord10021)(uint32_t* frame18);
    uint32_t (*ord10000)(uint32_t);
    int32_t  (*ord10060)(uint32_t);
    char* path;                                    // receives "DATA\\GLOBAL\\ui\\menu\\spinner" (248 B)
};

// 1.13c D2Client+0x3F3D0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t* d2_client_3f3d0(
    uint32_t* o, uint32_t p2, int32_t p3, uint32_t p4, uint32_t p5, D2ClientSpinnerEnv* env) {
    o[0] = env->vtblBase;
    o[0] = env->vtblSpinner;
    snprintf(env->path, 248, "%s\\ui\\menu\\spinner", "DATA\\GLOBAL");
    uint32_t h = env->sub2b420(0);
    o[2] = p2; o[0xb] = p4; o[0xf] = p5;
    uint32_t f[0x12] = {};
    o[0x14] = h; o[3] = (uint32_t)p3;
    o[0xd] = o[0xe] = o[0xc] = o[0x13] = o[0x10] = 0;
    f[0xd] = h;
    uint32_t r = env->ord10021(f);
    o[9] = env->ord10000(r);
    int32_t w = env->ord10060(r);
    o[10] = (uint32_t)w;
    o[5] = (uint32_t)p3;
    if (o[0xb] == 0) { o[4] = 1; o[6] = 1; o[7] = o[9] + 1 + (uint32_t)p3; }
    else { o[4] = 1; o[6] = (uint32_t)w; o[7] = (uint32_t)p3; }
    return o;
}
