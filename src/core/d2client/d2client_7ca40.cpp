// 1.13c D2Client+0x7CA40 (306 B, ECX=param_1, EDX=param_2, EAX=point {x,y}, one stack word, ret 4).
// From the disassembly: if Ordinal_11067(param_1) < [0x113ACE] (unsigned) { Ordinal_10422(0,param_1); if param_2: [param_2+0xA4]=0 }.
// D2Win 0xD3A8 (ECX=0xD) saves the old font; Fog alloc a node (0x659); node[0] = 0xD378(ECX=stack word, EDX=&node[1], 0xB4);
// 0x7BF60; the point moves up by 0x32 + 15*node[1] + 3 and left by half the u16 width at node+0x10. The rect
// {x, y, x+w, y+h(u16 node+0x12)} is a stack local: while [0x113AE4] < 0x10 (signed) 0x7AF20(&rect, count) may reject the slot
// (then 0x7B3D0, free, return without restoring the font); otherwise count++, 0x7BAD0(ECX=node, ESI=&rect), 0x7B3D0(EAX=node[0]),
// free, and 0xD3A8 (ECX = the saved font).
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7ca40(uint32_t param_1, uint32_t param_2, int32_t* pt, int32_t* count_c3ae4, uint32_t lim_c3ace,
                     uint32_t (*o11067)(uint32_t), void (*o10422)(uint32_t, uint32_t),
                     uint32_t (*font)(uint32_t ecx), uint32_t (*o10042)(uint32_t, uint32_t), uint32_t (*o10059)(uint32_t, uint32_t),
                     void (*sub_7bf60)(void), uint8_t (*sub_7af20)(int32_t* rect, int32_t idx),
                     void (*sub_7bad0)(uint32_t node, int32_t* rect), void (*sub_7b3d0)(uint32_t eax),
                     void (*free_)(uint32_t node, uint32_t, uint32_t), uint32_t stk) {
    if (o11067(param_1) < lim_c3ace) {
        o10422(0, param_1);
        if (param_2) *(uint32_t*)(uintptr_t)(param_2 + 0xa4) = 0;
    }
    const uint32_t saved = font(0xd);
    const uint32_t node = o10042(0x659, 0);
    uint32_t* n = (uint32_t*)(uintptr_t)node;
    n[0] = o10059(stk, 0xb4);
    sub_7bf60();
    const uint16_t w = *(const uint16_t*)(uintptr_t)(node + 0x10);
    const int32_t y = pt[1] - 0x32;
    pt[1] = y;
    pt[0] -= (int32_t)(w >> 1);
    pt[1] = y - (int32_t)n[1] * 0xf - 3;
    int32_t rect[4];
    rect[0] = pt[0];
    rect[1] = pt[1];
    rect[3] = pt[1] + *(const uint16_t*)(uintptr_t)(node + 0x12);
    rect[2] = pt[0] + w;
    if (*count_c3ae4 < 0x10 && sub_7af20(rect, *count_c3ae4) == 0) {
        sub_7b3d0(n[0]);
        free_(node, 0x68a, 0);
        return;
    }
    (*count_c3ae4)++;
    sub_7bad0(node, rect);
    sub_7b3d0(n[0]);
    free_(node, 0x68a, 0);
    font(saved);
}
