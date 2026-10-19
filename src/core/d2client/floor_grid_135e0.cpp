// 1.13c D2Client+0x135E0 -- build the 6x6 tile-grid records used by the high-quality floor path.
// Stage A transcription from build-wasm/ghidra-d2client/decomp/0x135E0.c; the FN signature and
// the D2Gfx-facing integration are intentionally left for the Stage B vector cycle.
#include <cstdint>
#include <emscripten.h>

struct D2ClientFloorGrid135E0Env {
    // Ordinal_10189 supplies the two coordinate bases (EBP and ESI in the retail body).
    void (*coordinate_bases)(int32_t* world_x, int32_t* screen_x_adjust);
    // Ordinal_10731 supplies the low-byte row origin.
    uint8_t (*row_origin)();
    // Ordinal_10023 transforms 16.16 world coordinates into record x/y; retail calls it even
    // when context is null.
    void (*world_to_screen)(void* context, int32_t world_x, int32_t world_y,
                            int32_t* screen_x, int32_t* screen_y);
    // Null-context retail path's helper, which also writes the record's first four bytes.
    void (*null_context_fill)(int32_t grid_x_times_8, uint8_t* color_bytes);
    uint32_t color;
};

// The retail body advances 12 bytes per cell for 36 cells. With corners_only set it visits only
// the 20 border cells; otherwise it visits all 36. The output coordinates and color are written
// only for visited records. Inputs expose the retail caller context and coordinate sources so
// the capture cycle can pin each import without pretending those imports are local constants.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_135e0(
    void* context, uint8_t* records, int32_t corners_only, const D2ClientFloorGrid135E0Env* env) {
    if (records == nullptr || env == nullptr) return;
    int32_t world_x_base = 0;
    int32_t screen_x_adjust = 0;
    if (env->coordinate_bases) env->coordinate_bases(&world_x_base, &screen_x_adjust);
    const int32_t row_base = env->row_origin ? (int32_t)env->row_origin() : 0;

    uint8_t* record = records;
    for (int32_t row = 0; row < 6; ++row) {
        for (int32_t col = 0; col < 6; ++col, record += 0xC) {
            if (corners_only != 0 && !((col == 0 || col == 5) && (row == 0 || row == 5))) continue;
            int32_t x = 0;
            int32_t y = 0;
            if (env->world_to_screen)
                env->world_to_screen(context, (world_x_base + col) * 0x10000,
                                     (row + row_base) * 0x10000, &x, &y);
            x += screen_x_adjust;
            if (context != nullptr) {
                record[0] = (uint8_t)env->color;
                record[1] = (uint8_t)(env->color >> 8);
                record[2] = (uint8_t)(env->color >> 24);
                record[3] = (uint8_t)(env->color >> 16);
            } else if (env->null_context_fill) {
                env->null_context_fill((world_x_base + col) * 8, record);
            }
            record[4] = (uint8_t)x;
            record[5] = (uint8_t)(x >> 8);
            record[6] = (uint8_t)(x >> 16);
            record[7] = (uint8_t)(x >> 24);
            record[8] = (uint8_t)y;
            record[9] = (uint8_t)(y >> 8);
            record[10] = (uint8_t)(y >> 16);
            record[11] = (uint8_t)(y >> 24);
        }
    }
}
