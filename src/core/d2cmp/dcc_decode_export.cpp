// Browser entry for the DCC direction decoder: builds the in-memory header the 1.13c driver reads
// (directions at +0x14, frames per direction at +0x18, direction offsets from +0x1C), runs the
// driver on one direction with its own static work areas, and hands the decoded canvas back
// frame by frame. The pixel decode is the verified codec; this file only frames its input/output.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "d2cmp_dcc.h"

struct DccDecodeEnv {
    void (*lock)(); void (*unlock)(); uint8_t* (*alloc)(void*, uint32_t); D2CmpFatalFn fatal; void (*log)(const char*);
    void (*fileError)(const char*); DccFrame* frames; DccCell* cells; uint8_t* pixbuf; uint8_t* canvas; uint32_t ext32;
};
uint8_t* d2cmp_140a0(const uint8_t*, const uint8_t*, uint32_t*, void*, int32_t, uint32_t*, int32_t, int32_t, const char*, const DccDecodeEnv*);

namespace {
const uint32_t kCanvas = 0x1d4c0, kPixBuf = 0x40000;
DccFrame g_frames[256];
DccCell g_cells[0x6ddd];
uint8_t g_pixbuf[kPixBuf], g_canvas[kCanvas];
std::vector<uint8_t> g_block, g_pix;
std::vector<int32_t> g_meta;   // per frame: width, height, xoff, yoff, bottomUp, pixel offset
bool g_fatal;
void nop() {}
uint8_t* alloc(void*, uint32_t size) { g_block.assign(size + 64, 0); return g_block.data(); }
void fatal(uint32_t) { g_fatal = true; }
void logm(const char*) { g_fatal = true; }
void fileError(const char*) { g_fatal = true; }
}

extern "C" {
// Decodes direction `dir` of the DCC file at `file`; returns its frame count, or 0 on a refusal.
EMSCRIPTEN_KEEPALIVE int d2_dcc_decode_direction(const uint8_t* file, uint32_t len, uint32_t dir) {
    if (len < 19 || file[0] != 0x74) return 0;
    uint32_t dirs = file[2], fpd; memcpy(&fpd, file + 3, 4);
    if (dir >= dirs || dirs > 64 || !fpd || fpd > 256 || len < 15 + dirs * 4) return 0;
    std::vector<uint8_t> hdr(0x1c + dirs * 4 + 4, 0);
    // D2CMP 0x13010's parsed header: +4 version, +8 the tag dword (file+7), +0x14 directions, +0x18 frames per direction.
    uint32_t d32 = dirs, ver = file[1], tag; memcpy(&tag, file + 7, 4);
    memcpy(&hdr[4], &ver, 4); memcpy(&hdr[8], &tag, 4);
    memcpy(&hdr[0x14], &d32, 4); memcpy(&hdr[0x18], &fpd, 4);
    for (uint32_t i = 0; i < dirs; i++) memcpy(&hdr[0x1c + i * 4], file + 15 + i * 4, 4);
    uint32_t off; memcpy(&off, file + 15 + dir * 4, 4);
    if (off >= len) return 0;
    g_fatal = false;
    memset(g_frames, 0, sizeof g_frames); memset(g_cells, 0, sizeof g_cells);
    memset(g_pixbuf, 0, sizeof g_pixbuf); memset(g_canvas, 0, sizeof g_canvas);
    uint32_t total = 0, out[3 + 65] = {0};
    const DccDecodeEnv env{nop, nop, alloc, fatal, logm, fileError, g_frames, g_cells, g_pixbuf, g_canvas, 0};
    d2cmp_140a0(hdr.data(), file + off, &total, nullptr, 0, out, (int32_t)dir, 1, "", &env);
    if (g_fatal) return 0;
    g_meta.assign(fpd * 6, 0); g_pix.clear();
    // The canvas is shared by every frame of the direction, so the pixels come from each frame's sprite block (the
    // run-length body the driver wrote when the frame finished): rows run last-to-first for a top-down frame.
    size_t at = 0;
    for (uint32_t f = 0; f < fpd; f++) {
        const DccFrame& fr = g_frames[f];
        if (fr.w <= 0 || fr.h <= 0) return 0;
        int32_t* m = &g_meta[f * 6];
        m[0] = fr.w; m[1] = fr.h; m[2] = fr.xoff; m[3] = fr.yoff; m[4] = fr.bottomUp; m[5] = (int32_t)g_pix.size();
        if (at + 0x23 + fr.coded > g_block.size()) return 0;
        const uint8_t* p = g_block.data() + at + 0x20; const uint8_t* end = p + fr.coded;
        g_pix.resize(g_pix.size() + (size_t)fr.w * fr.h, 0);
        uint8_t* img = g_pix.data() + m[5];
        int32_t row = 0, x = 0;
        while (p < end && row < fr.h) {
            uint8_t c = *p++;
            if (c == 0x80) { row++; x = 0; continue; }
            if (c & 0x80) { x += c & 0x7f; continue; }
            if (x + c > fr.w || p + c > end) return 0;
            memcpy(img + (size_t)(fr.bottomUp ? row : fr.h - 1 - row) * fr.w + x, p, c);
            p += c; x += c;
        }
        if (row != fr.h || p != end) return 0;
        at += (size_t)fr.coded + 0x23;
    }
    return (int)fpd;
}
EMSCRIPTEN_KEEPALIVE const int32_t* d2_dcc_frame_meta() { return g_meta.data(); }
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_dcc_pixels() { return g_pix.data(); }
EMSCRIPTEN_KEEPALIVE uint32_t d2_dcc_pixels_len() { return (uint32_t)g_pix.size(); }
}
