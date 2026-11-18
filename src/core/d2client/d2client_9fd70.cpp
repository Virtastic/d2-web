// 1.13c D2Client+0x9FD70 -- dialog cancel/close handler (env-seam; retail globals passed in).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2C9fd70G {            // retail globals, RVAs from base 0x6FAB0000
    int32_t* state;           // bb214
    int32_t* flag210;         // bb210
    int32_t  v21c;            // bb21c
    uint32_t unit60;          // *(bcbbfc+0x60), valid when unitId != nullptr
    const uint32_t* unitId;   // &*(bcbbfc+0xC); null when bcbbfc==0
    int32_t* g834; int32_t* g858; int32_t* g854; int32_t* g864;  // bcb834/858/854/864
};
struct D2C9fd70Env {
    uint32_t (*ord_11017)(uint32_t);   // Ordinal_11017
    void (*fn_88a70)(int, int, int, int);
    void (*send_packet)(const uint8_t* p17);  // 0x143E0
    void (*fn_48150)(void);
    void (*fn_9f9e0)(void);
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_9fd70(const D2C9fd70G* g, const D2C9fd70Env* env) {
    if (*g->state == 3) return 0;
    if (*g->state == 2) {
        *g->g834 = g->unitId ? env->ord_11017(g->unit60) : 0;
        *g->g858 = 0; *g->g854 = 5;
        *g->g864 = *g->g834 ? 4 : 1;
    }
    env->fn_88a70(0, 0, 0, 0);
    if (*g->flag210 == 0) {
        uint8_t q[17] = {'D'};
        uint32_t id = g->unitId ? *g->unitId : 0xffffffffu, z = 0, two = 2;
        std::memcpy(q + 1, &id, 4); std::memcpy(q + 5, &g->v21c, 4);
        std::memcpy(q + 9, &z, 4);  std::memcpy(q + 13, &two, 4);
        env->send_packet(q);
    } else env->fn_48150();
    env->fn_9f9e0();
    return 1;
}
