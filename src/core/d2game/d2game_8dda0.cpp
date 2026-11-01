// 1.13c D2Game+0x8DDA0 -- SCmd command dispatch on AL.
// AL = command, ECX = unit/object handle (opaque), EDX = client (EBX at the 0x8D3D0 calls), plain ret.
// Ordinals: 0xA6C4 #10020, 0xAFD6 #10728, 0xA92E #10012, 0xAFDC #10516, 0xA8A4 #10202.
#include "d2game_8d6_common.h"
struct D2Game8DDA0Env {
    uint32_t (*ord10516)(uint32_t p);                                                  // 0xAFDC, one stack word
    uint32_t (*ord10202)(uint32_t p, uint32_t level, uint32_t line, const char* file); // 0xA8A4, four stack words
    uint32_t (*ord10020)(uint32_t p);                                                  // 0xA6C4, AL result used
    uint32_t (*ord10728)(uint32_t p);                                                  // 0xAFD6
    void     (*ord10012)(uint32_t p, uint32_t v);                                      // 0xA92E, two stack words
    void     (*sub8D3D0)(uint32_t client, uint32_t id, uint32_t arg, uint32_t flag);
};
// 1.13c D2Game+0x8DDA0
void d2game_port_8dda0(uint8_t cmd, uint32_t p, uint32_t client, const D2Game8DDA0Env* env)
{
    static const char kFile[] = "..\\Source\\D2Game\\GAME\\SCmd.cpp";
    if (cmd == '8') {
        uint32_t edi = 0;
        if (env->ord10516(p) != 0 && env->ord10202(p, 0x10, 0x9C2, kFile) == 0) edi = 1;
        env->sub8D3D0(client, 0xB, 0, edi);
    } else if (cmd == '9') {
        // The saved word is the pushed ECX slot with only its low byte replaced by the #10020 result.
        const uint32_t saved = (p & 0xFFFFFF00u) | (env->ord10020(p) & 0xFFu);
        env->ord10012(p, env->ord10728(p));
        env->sub8D3D0(client, 0xC, 0, 0);
        env->ord10012(p, saved);
    }
}
