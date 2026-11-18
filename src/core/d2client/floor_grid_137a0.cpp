// 1.13c D2Client+0x137A0. Builds a 6x6 floor grid. Retail's EAX input and
// seven stack arguments are explicit; 
#include <bit>
#include "floor_grid_137a0.h"

static int32_t floor_grid_word(uint32_t value) {
    return std::bit_cast<int32_t>(value);
}

extern "C" void d2_client_137a0(
    const D2ClientFloorGridEnv* env, D2ClientFloorVertex* out,
    int32_t ybase, int32_t light_x, int32_t light_y, uint32_t constant_color,
    int32_t shift_mode, uint32_t corners_only, int32_t xbase) {
    const uint8_t intensity = static_cast<uint8_t>(env->ordinal_10731(env->act));
    const int32_t quarter_width = env->screen_width / 4;
    if (shift_mode == 1) xbase = floor_grid_word(uint32_t(xbase) - uint32_t(quarter_width));
    else if (shift_mode == 2) xbase = floor_grid_word(uint32_t(xbase) + uint32_t(quarter_width));
    uint8_t red = 0, green = 0, blue = 0;
    env->ordinal_10189(env->act, &red, &green, &blue);

    for (int32_t row = 0; row < 6; ++row) {
        for (int32_t col = 0; col < 6; ++col) {
            if (corners_only && !((col == 0 || col == 5) && (row == 0 || row == 5))) continue;
            D2ClientFloorVertex& v = out[row * 6 + col];
            v.x = floor_grid_word(uint32_t(xbase) + uint32_t((col - row) * 16));
            v.y = floor_grid_word(uint32_t(ybase) + uint32_t((col + row) * 8));
            if (constant_color) {
                v.color[0] = intensity;
                v.color[1] = red;
                v.color[2] = green;
                v.color[3] = blue;
            } else {
                env->sample_light_a8230(
                    floor_grid_word((uint32_t(light_x) + uint32_t(col)) * 8u),
                    floor_grid_word((uint32_t(light_y) + uint32_t(row)) * 8u),
                    &v.color[0], &v.color[1], &v.color[2], &v.color[3]);
            }
        }
    }
}
