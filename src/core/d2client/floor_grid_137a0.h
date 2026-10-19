#pragma once
#include <cstddef>
#include <cstdint>

struct D2ClientFloorVertex {
    uint8_t color[4];
    int32_t x;
    int32_t y;
};
static_assert(sizeof(D2ClientFloorVertex) == 12);
static_assert(offsetof(D2ClientFloorVertex, x) == 4);
static_assert(offsetof(D2ClientFloorVertex, y) == 8);

struct D2ClientFloorGridEnv {
    uint32_t (*ordinal_10731)(uint32_t act);
    void (*ordinal_10189)(uint32_t act, uint8_t* red, uint8_t* green, uint8_t* blue);
    // D2Client+0xA8230: ECX=x8, EAX=y8; four color output pointers.
    // The callee leaves RGB untouched when global +0xDE830 is zero.
    void (*sample_light_a8230)(int32_t x8, int32_t y8, uint8_t*, uint8_t*, uint8_t*, uint8_t*);
    uint32_t act;           // global +0x11C3B8
    int32_t screen_width;   // global +0xDBC48
};

extern "C" void d2_client_137a0(
    const D2ClientFloorGridEnv* env, D2ClientFloorVertex* out,
    int32_t ybase, int32_t light_x, int32_t light_y, uint32_t constant_color,
    int32_t shift_mode, uint32_t corners_only, int32_t xbase);
