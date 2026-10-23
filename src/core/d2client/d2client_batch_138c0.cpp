// src/core/d2client/d2client_batch_138c0.cpp -- three UNMAPPED 1.13c D2Client.dll rows reimplemented
// from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, RE-DONE first).
// Base 0x6FAB0000; every RVA below is retail's. The D2Client globals 0x138C0 and 0x13A00 read are
// env fields (a later capture pins each at its retail RVA); the unrecovered D2Client callees and the
// D2Common/Storm/D2Gfx imports are seams. No spec, no vector yet.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>
#include "floor_grid_137a0.h"

// One env for the three rows. Globals first (retail RVA of each), then the seams.
struct D2ClientBatch138C0Env {
    // ---- globals this file reads; the vector must pin each ----
    uint32_t act;                     // +0x11C3B8  DAT_6fbcc3b8, the active act passed to D2Common
    uint32_t light_table;             // +0x105208  0x30 x 0x30 entries, 8 bytes each
    int32_t view_tile_x;              // +0x104DF0  DAT_6fbb4df0, column origin subtracted in 0xA8230
    int32_t view_tile_y;              // +0x104DF4  DAT_6fbb4df4, row origin subtracted in 0xA8230
    uint32_t light_rgb_enabled;       // +0x0DE830  DAT_6fb8e830: nonzero enables entry bytes 5..7
    int32_t light_quality;            // +0x0F17C8  DAT_6fba17c8, signed compare with 4
    uint32_t draw_ctx;                // +0x11C414  DAT_6fbcc414, the 8th word to D2Gfx 10076

    // ---- imports and unrecovered D2Client callees ----
    int32_t (*get_environment_intensity)(uint32_t act);  // 0xC5A4 D2Common #10731: act+4, env+0xC
    void (*get_act_light_color)(uint32_t act, uint8_t* red, uint8_t* green, uint8_t* blue);
                                                         // 0xC5B6 D2Common #10189: env+0x18/0x19/0x1A
    uint32_t (*str_copy)(char* dst, const char* src, uint32_t n);  // 0xBE96 Storm #501 SStrCopy (ret 0xC)
    uint32_t (*gfx_10013)(void);                         // 0xD07E D2Gfx #10013
    uint32_t (*gfx_10047)(void);                         // 0xD0C6 D2Gfx #10047
    uint32_t (*get_tick_count)(void);                    // [0x6FB7EF5C] kernel32!GetTickCount
    // 0xD1C8 D2Gfx #10076: stack words in retail order (sub, block, x, y, pos5, pos6, alpha, ctx, layer).
    void (*draw_floor)(uint32_t sub, uint32_t block, int32_t x, int32_t y, int32_t pos5, int32_t pos6,
                       int32_t alpha, uint32_t ctx, uint32_t layer);
    // 0x137A0 uses its own typed environment; retail stack words are
    // {block, y, lightX, lightY, constantColor, shiftMode, cornersOnly}, EAX=x.
    const D2ClientFloorGridEnv* floor_grid_137a0;
    // 0x135E0: ECX = ctx, stack { block, pos5, pos6, height, ctx, sub }.
    void (*floor_135e0)(uint32_t block, int32_t pos5, int32_t pos6, int32_t height, uint32_t ctx, uint32_t sub);
    // 0x143E0: the packet pointer is the one stack arg, the byte count arrives in EBX.
    void (*send_message)(const uint8_t* packet, uint32_t length);
    // retail fatal path: Fog #10265 (0xBF92) -> Fog #10024 (0xBF80) -> _exit(0x3664) with -1.
    void (*fatal)(uint32_t code);
};

// The room view 0x13A00 walks. Offsets are retail's; pointers are the low 32 bits as in FloorCtx.
struct FloorDrawCtx {
    uint8_t p0[0x14];
    int32_t left;      // +0x14
    int32_t top;       // +0x18
    int32_t right;     // +0x1C
    int32_t bottom;    // +0x20
    int32_t view_x;    // +0x24
    int32_t view_y;    // +0x28
    uint8_t p1[0xEAA8 - 0x2C];
    uint32_t grid;     // +0xEAA8  cells of 0x24 bytes, each holding a node list at +0x1C
    uint32_t p2;       // +0xEAAC  unread by this function
    int32_t grid_size; // +0xEAB0  cells walked = grid_size * grid_size
};
static_assert(offsetof(FloorDrawCtx, left) == 0x14 && offsetof(FloorDrawCtx, view_y) == 0x28 &&
              offsetof(FloorDrawCtx, grid) == 0xEAA8 && offsetof(FloorDrawCtx, grid_size) == 0xEAB0, "ctx");

struct FloorDrawNode {
    uint8_t p0[4];
    int32_t x;        // +0x04  >> 3 is the 5th word to D2Gfx 10076
    int32_t y;        // +0x08  >> 3 is the 6th word to D2Gfx 10076
    uint32_t tile;    // +0x0C
    uint32_t next;    // +0x10
};
static_assert(offsetof(FloorDrawNode, x) == 4 && offsetof(FloorDrawNode, tile) == 0xC &&
              offsetof(FloorDrawNode, next) == 0x10, "node");

struct FloorDrawTile {
    int32_t x;             // +0x00
    int32_t y;             // +0x04
    uint8_t p0[0x14 - 8];
    uint32_t flags;        // +0x14  layer bits 14..17, 0x400 (alpha 0), 0x408 skip mask, 0x20000 drawn
    uint32_t sub;          // +0x18  graphic, its uint16 at +0x04 is the height
    uint8_t p1[0x24 - 0x1C];
    uint32_t fade_flags;   // +0x24  bit 1: the fade below is live
    uint8_t alpha;         // +0x28
    uint8_t fade_from;     // +0x29
    uint8_t fade_to;       // +0x2A
    uint8_t p2[1];
    uint32_t fade_deadline;// +0x2C
};
static_assert(offsetof(FloorDrawTile, flags) == 0x14 && offsetof(FloorDrawTile, sub) == 0x18 &&
              offsetof(FloorDrawTile, alpha) == 0x28 && offsetof(FloorDrawTile, fade_deadline) == 0x2C, "tile");

struct FloorDrawSub { uint8_t p0[4]; uint16_t height; };   // +0x04
static_assert(offsetof(FloorDrawSub, height) == 4, "sub");

// ---------------------------------------------------------------------------------------------------------
// 0x138C0

// 1.13c D2Client+0x138c0 (311 B): fill one 8 x 8 tile light block (64 cells of 12 bytes, row stride 0x60).
// ECX = block, EAX = mode, two stack words = tile x (arg1) and y (arg2).
// mode != 0: solid act colours -- D2Common 10731 gives the intensity, 10189 the red/green/blue; the two stack
// word slots are only scratch (retail reuses them as the colour out-params).
// mode == 0: per cell, 0xA8230 samples the light table: index col = ((x*8-8)+8i) >> 3 minus +0x104DF0 and
// row = ((y*8-8)+8j) >> 3 minus +0x104DF4, both clamped to 0..0x2F; entry = table + (row*0x30+col)*8;
// dest[0] = entry[4], and only when +0x0DE830 is nonzero dest[1..3] = entry[5..7] (red, green, blue).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_138c0(uint8_t* block, int32_t mode, int32_t x, int32_t y,
                                                     const D2ClientBatch138C0Env* env) {
    if (mode != 0) {
        uint8_t intensity = (uint8_t)env->get_environment_intensity(env->act);
        uint8_t red = 0, green = 0, blue = 0;
        env->get_act_light_color(env->act, &red, &green, &blue);
        for (int32_t row = 0; row < 8; row++) {
            uint8_t* cell = block + (size_t)(uint32_t)row * 0x60;
            for (int32_t col = 0; col < 8; col++, cell += 0xC) {
                cell[0] = intensity;
                cell[1] = red;
                cell[2] = green;
                cell[3] = blue;
            }
        }
        return;
    }
    const uint8_t* table = (const uint8_t*)(uintptr_t)env->light_table;
    for (int32_t j = 0; j < 8; j++) {
        uint8_t* cell = block + (size_t)(uint32_t)j * 0x60;
        for (int32_t i = 0; i < 8; i++, cell += 0xC) {
            // The lea/sar of 0xA8230's caller: the products are exact, so this is x-1+i / y-1+j.
            int32_t col = ((x * 8 - 8) + i * 8) >> 3;
            int32_t row = ((y * 8 - 8) + j * 8) >> 3;
            col -= env->view_tile_x;
            row -= env->view_tile_y;
            if (col < 0) col = 0; else if (col > 0x2F) col = 0x2F;
            if (row < 0) row = 0; else if (row > 0x2F) row = 0x2F;
            const uint8_t* entry = table + (size_t)(uint32_t)(row * 0x30 + col) * 8;
            cell[0] = entry[4];
            if (env->light_rgb_enabled != 0) {
                cell[1] = entry[5];
                cell[2] = entry[6];
                cell[3] = entry[7];
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------
// 0x13A00

// 1.13c D2Client+0x13A00 (654 B): per-layer floor draw pass over the room grid. Layers 1..4 test bits 14..17
// of tile+0x14. A live fade (tile+0x24 bit 1) interpolates tile+0x28 from fade_from to fade_to against
// GetTickCount and the deadline at +0x2C only while the quality snapshot is >= 4 and tick < deadline; otherwise
// it snaps to fade_to and clears bit 1. Alpha 0 sets 0x400, then any tile with 0x408 set is skipped. Screen
// position is tile x + 0x50 - view_x and tile y - uint16(sub+4) - 0x50 - view_y, clipped to the +0x14 rect.
// Quality < 4 fills the 768-byte block through 0x138C0; otherwise it builds the 432-byte block through
// 0x137A0 (when D2Gfx 10013 returned 0) or 0x135E0. D2Gfx 10076 receives (sub, block, x, y, pos5, pos6,
// alpha, DRAW_CTX, 0) and the tile's 0x20000 bit is set on every submitted tile.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_13a00(FloorDrawCtx* ctx, const D2ClientBatch138C0Env* env) {
    uint32_t tick = env->get_tick_count();
    int32_t quality = env->light_quality;            // +0x0F17C8 snapshot for the fade branch
    uint32_t gfx = env->gfx_10013();                 // D2Gfx #10013: nonzero picks 0x135E0
    uint32_t sub_word = env->gfx_10047();            // D2Gfx #10047: last stack word of 0x137A0/0x135E0
    uint32_t draw_ctx = env->draw_ctx;               // +0x11C414
    int32_t dx = 0x50 - ctx->view_x;
    int32_t dy = -0x50 - ctx->view_y;
    int32_t cells = ctx->grid_size * ctx->grid_size;
    for (uint32_t layer = 1; layer < 5; layer++) {
        uint32_t mask = layer << 14;
        if (cells == 0) continue;
        uint8_t* cell = (uint8_t*)(uintptr_t)ctx->grid + 0x1C;   // grid base is re-read per layer
        for (int32_t n = cells; n != 0; n--) {
            for (uint32_t node = *(const uint32_t*)cell; node != 0;
                 node = ((const FloorDrawNode*)(uintptr_t)node)->next) {
                FloorDrawNode* nd = (FloorDrawNode*)(uintptr_t)node;
                FloorDrawTile* t = (FloorDrawTile*)(uintptr_t)nd->tile;
                uint32_t flags = t->flags;
                if ((flags & mask) != mask) continue;
                if ((t->fade_flags & 2) != 0) {
                    if (quality >= 4 && tick < t->fade_deadline) {
                        // 32-bit unsigned tick distance, signed 32-bit product, truncating /500 (the
                        // 0x10624DD3 magic) -- exactly C's signed division.
                        int32_t span = (int32_t)t->fade_to - (int32_t)t->fade_from;
                        int32_t v = span * (int32_t)(tick - t->fade_deadline + 500u);
                        t->alpha = (uint8_t)((int32_t)t->fade_from + v / 500);
                    } else {
                        t->fade_flags &= ~2u;
                        t->alpha = t->fade_to;
                    }
                    if (t->alpha == 0) flags |= 0x400; else flags &= ~0x400u;
                    t->flags = flags;
                }
                if ((t->flags & 0x408) != 0) continue;
                FloorDrawSub* sub = (FloorDrawSub*)(uintptr_t)t->sub;
                int32_t sx = t->x + dx;
                int32_t sy = (t->y - (int32_t)sub->height) + dy;
                if ((uintptr_t)ctx + 0x14 == 0) env->fatal(0x76);   // retail tests param_1+0x14; dead
                if (!(ctx->left <= sx && sx < ctx->right && ctx->top <= sy && sy < ctx->bottom)) continue;
                int32_t pos5 = nd->x >> 3;
                int32_t pos6 = nd->y >> 3;
                uint32_t block;
                if (env->light_quality < 4) {                // the global is re-read per tile
                    static uint8_t light_block[768];         // auStack_300, S+0x1F8
                    d2_client_138c0(light_block, (int32_t)sub->height, pos5, pos6, env);
                    block = (uint32_t)(uintptr_t)light_block;
                } else {
                    static D2ClientFloorVertex high_grid[36];   // auStack_4b0, S+0x48
                    uint8_t* high_block = reinterpret_cast<uint8_t*>(high_grid);
                    if (gfx == 0)
                        d2_client_137a0(env->floor_grid_137a0, high_grid,
                                         sy, pos5, pos6, sub->height, static_cast<int32_t>(draw_ctx), sub_word, sx);
                    else
                        env->floor_135e0((uint32_t)(uintptr_t)high_block, pos5, pos6,
                                         (int32_t)sub->height, draw_ctx, sub_word);
                    block = (uint32_t)(uintptr_t)high_block;
                }
                env->draw_floor(t->sub, block, sx, sy, pos5, pos6, (int32_t)t->alpha, draw_ctx, 0);
                t->flags |= 0x20000;
            }
            cell += 0x24;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------
// 0x147A0

// 1.13c D2Client+0x147A0 (105 B): D2SendMsgToAll -- the packet arrives in ESI (reference/PlugY/
// Commons/D2Funcs.h line 314 names it at this RVA; reference/D2MOO PlrMsg.cpp reads the same layout).
// Buffer = 3 header bytes, then SStrCopy(buf+3, data+3, 0x100), then the string at data+0x103 when the
// address data+0x103 is nonzero (retail's `lea/test`, so always for a real pointer) copied with a 0x10 cap,
// then one NUL, then a second NUL: the send gets the buffer pointer and its byte count in EBX.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_147a0(const uint8_t* data, const D2ClientBatch138C0Env* env) {
    uint8_t buf[0x204];
    buf[0] = data[0];
    buf[1] = data[1];
    buf[2] = data[2];
    uint32_t len = env->str_copy((char*)buf + 3, (const char*)data + 3, 0x100);
    uint8_t* p = buf + 3 + len + 1;                  // past the terminator SStrCopy wrote
    if ((uintptr_t)data + 0x103 != 0) {
        uint32_t n = env->str_copy((char*)p, (const char*)data + 0x103, 0x10);
        p += n + 1;
    } else {
        *p++ = 0;
    }
    *p = 0;
    env->send_message(buf, (uint32_t)(p - buf) + 1);
}
