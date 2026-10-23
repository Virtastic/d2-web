// src/core/d2client/d2client_batch_10110.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, RE-DONE first). Base 0x6FAB0000:
// globals named below are retail VAs, the RVA in each marker is retail's. No spec, no vector yet.
//
// Rows 0x10110, 0x10250, 0x10640, 0x10680 are an embedded zlib from the 1.0/1.1 lineage (blocks state by
// pointer, 0x18-byte inflate_state, 0x38-byte z_stream, error strings "invalid window size" ...). The block
// coder 0x0F4C0 and its alloc/free (0x0FF60, 0x0F3F0, 0x0F450, 0x10000, 0x0FFF0) are outside this batch and
// arrive through D2ClientInflateEnv; the init/reset/end bodies below are transcribed write-for-write.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

// ---------------------------------------------------------------------------------------------------------
// zlib (D2Client+0x10110 inflateReset, 0x10250 inflate, 0x10640 inflateEnd, 0x10680 inflateInit2_)
struct B81ZStream {                                  // sizeof == 0x38, checked by inflateInit2_ (param_3 != 0x38)
    uint32_t next_in;                                // 0x00
    uint32_t avail_in;                               // 0x04
    uint32_t total_in;                               // 0x08
    uint32_t next_out;                               // 0x0C
    uint32_t avail_out;                              // 0x10
    uint32_t total_out;                              // 0x14
    uint32_t msg;                                    // 0x18 (pointer)
    uint32_t state;                                  // 0x1C
    uint32_t zalloc;                                 // 0x20 (address of uint32_t* (*)(uint32_t, uint32_t, uint32_t))
    uint32_t zfree;                                  // 0x24 (address of void (*)(uint32_t, void*))
    uint32_t opaque;                                 // 0x28
    uint32_t data_type;                              // 0x2C
    uint32_t adler;                                  // 0x30
    uint32_t reserved;                               // 0x34
};
static_assert(sizeof(B81ZStream) == 0x38, "z_stream size");
static_assert(offsetof(B81ZStream, state) == 0x1C && offsetof(B81ZStream, zfree) == 0x24 &&
              offsetof(B81ZStream, adler) == 0x30, "z_stream layout");

struct B81ZState {                                   // inflateInit2_ allocates exactly 0x18
    uint32_t mode;                                   // 0x00
    uint32_t hold;                                   // 0x04 (header word; also 5 on the data-error paths)
    uint32_t check;                                  // 0x08 (DICTID / trailer word read from the input)
    uint32_t raw;                                    // 0x0C (1 = negative windowBits: no zlib header/trailer)
    uint32_t wbits;                                  // 0x10
    uint32_t blocks;                                 // 0x14 (pointer to the blocks state)
};
static_assert(sizeof(B81ZState) == 0x18, "inflate_state size");
static_assert(offsetof(B81ZState, raw) == 0x0C && offsetof(B81ZState, blocks) == 0x14, "inflate_state layout");

struct B81ZBlocks {                                  // only the fields these four functions touch
    uint32_t mode;                                   // 0x00: 4/5 = trees built, 6 = codes built
    uint32_t sub;                                    // 0x04: first union word (the trees/codes allocation)
    uint8_t  pad0[0x14];                             // 0x08..0x1B
    uint32_t bitk;                                   // 0x1C
    uint32_t bitb;                                   // 0x20
    uint8_t  pad1[4];                                // 0x24..0x27
    uint32_t window;                                 // 0x28
    uint8_t  pad2[4];                                // 0x2C..0x2F
    uint32_t read;                                   // 0x30
    uint32_t write;                                  // 0x34
    uint32_t check_fn;                               // 0x38 (uint32_t (*)(uint32_t))
    uint32_t check;                                  // 0x3C
};
static_assert(offsetof(B81ZBlocks, bitk) == 0x1C && offsetof(B81ZBlocks, window) == 0x28 &&
              offsetof(B81ZBlocks, read) == 0x30 && offsetof(B81ZBlocks, check_fn) == 0x38, "blocks layout");

// The block coder and the allocators this batch does not own. A null seam is the unlinked case: the call
// sites keep retail's control flow and the observable (stream/state words) is still produced.
struct D2ClientInflateEnv {
    uint32_t* (*zalloc)(uint32_t, uint32_t, uint32_t);   // +0x10000 default allocator
    void      (*zfree)(uint32_t, void*);                 // +0x0FFF0 default free
    void*     (*blocks_new)(B81ZStream*, int32_t);       // +0x0FF60 inflate_blocks_new
    int32_t   (*blocks)(B81ZBlocks*, B81ZStream*, int32_t); // +0x0F4C0 inflate_blocks
    void      (*blocks_end)();                           // +0x0F3F0 called when inflate_blocks returns 1
    void      (*blocks_free)(B81ZBlocks*, B81ZStream*);  // +0x0F450 inflate_blocks_free
};
static D2ClientInflateEnv g_inflate;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_inflate_set_env(const D2ClientInflateEnv* e) {
    if (e) g_inflate = *e; else g_inflate = D2ClientInflateEnv{};
}

static const char kBadWindow[] = "invalid window size";
static const char kBadMethod[] = "unknown compression method";
static const char kBadHeader[] = "incorrect header check";
static const char kBadData[] = "incorrect data check";
static const char kNeedDict[] = "need dictionary";

// 1.13c D2Client+0x10110
// inflateReset(strm): zero total_in/total_out/msg, mode = raw ? 7 (TYPE) : 0 (HEAD), then the inlined
// blocks reset -- free the trees/codes allocation through strm->zfree for modes 4/5/6, read = write =
// window, mode/bitk/bitb = 0, and the blocks' check function re-initialised with (0), its value copied to
// blocks+0x3C and strm->adler.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_10110(B81ZStream* z) {
    if (z == nullptr || z->state == 0) return -2;              // Z_STREAM_ERROR
    B81ZState* st = (B81ZState*)(uintptr_t)z->state;
    z->total_out = 0;
    z->total_in = 0;
    z->msg = 0;
    st->mode = st->raw != 0 ? 7u : 0u;
    B81ZBlocks* b = (B81ZBlocks*)(uintptr_t)st->blocks;
    if (b == nullptr) return 0;                                // retail dereferences; init always awards blocks
    if (b->mode == 4 || b->mode == 5 || b->mode == 6) {
        if (z->zfree) ((void (*)(uint32_t, void*))(uintptr_t)z->zfree)(z->opaque, (void*)(uintptr_t)b->sub);
    }
    b->read = b->window;
    b->write = b->window;
    b->mode = 0;
    b->bitk = 0;
    b->bitb = 0;
    if (b->check_fn != 0) {
        const uint32_t v = ((uint32_t (*)(uint32_t))(uintptr_t)b->check_fn)(0);
        b->check = v;
        z->adler = v;
    }
    return 0;
}

// 1.13c D2Client+0x10640
// inflateEnd(strm): requires strm, strm->state and strm->zfree; frees the blocks state (0x0F450) when
// present, then strm->state through strm->zfree and clears it.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_10640(B81ZStream* z) {
    if (z == nullptr || z->state == 0 || z->zfree == 0) return -2;
    B81ZState* st = (B81ZState*)(uintptr_t)z->state;
    if (st->blocks != 0 && g_inflate.blocks_free) g_inflate.blocks_free((B81ZBlocks*)(uintptr_t)st->blocks, z);
    ((void (*)(uint32_t, void*))(uintptr_t)z->zfree)(z->opaque, (void*)(uintptr_t)z->state);
    z->state = 0;
    return 0;
}

extern "C" int32_t d2_client_10110(B81ZStream*);

// 1.13c D2Client+0x10680
// inflateInit2_(version, strm, windowBits, stream_size): version[0] == '1' and stream_size == 0x38 or
// Z_VERSION_ERROR; default zalloc/zfree (+0x10000/+0x0FFF0) when null; allocate 0x18, clear blocks/raw;
// negative windowBits = raw (raw = 1, negate); 8..15 only; store wbits, blocks_new (+0x0FF60); success is
// inflateReset, failure is inflateEnd then Z_MEM_ERROR (-4).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_10680(const char* version, B81ZStream* z, int32_t windowBits,
                                                        uint32_t stream_size) {
    if (version == nullptr || version[0] != '1' || stream_size != 0x38) return -6;  // Z_VERSION_ERROR
    if (z == nullptr) return -2;
    z->msg = 0;
    if (z->zalloc == 0) { z->zalloc = (uint32_t)(uintptr_t)g_inflate.zalloc; z->opaque = 0; }
    if (z->zfree == 0) z->zfree = (uint32_t)(uintptr_t)g_inflate.zfree;
    B81ZState* st = z->zalloc
        ? (B81ZState*)(uintptr_t)((uint32_t* (*)(uint32_t, uint32_t, uint32_t))(uintptr_t)z->zalloc)(
              z->opaque, 1, sizeof(B81ZState))
        : nullptr;
    z->state = (uint32_t)(uintptr_t)st;
    if (st == nullptr) return -4;                              // Z_MEM_ERROR
    st->blocks = 0;
    st->raw = 0;
    if (windowBits < 0) { windowBits = -windowBits; st->raw = 1; }
    if (windowBits < 8 || windowBits > 15) { d2_client_10640(z); return -2; }
    st->wbits = (uint32_t)windowBits;
    st->blocks = (uint32_t)(uintptr_t)(g_inflate.blocks_new ? g_inflate.blocks_new(z, windowBits) : nullptr);
    if (st->blocks != 0) { d2_client_10110(z); return 0; }
    d2_client_10640(z);
    return -4;
}

// 1.13c D2Client+0x10250
// inflate(strm, flush): the 14-state machine. Returns Z_BUF_ERROR (-5) while starved, the flush-derived
// value ((flush != 4) - 1) & 0xFFFFFFFB (0, or -5 under Z_FINISH) when a stage completes without ending,
// 2 at DICTID, 1 at stream end, -3 at BAD, -2 for a bad stream/state/mode. Blocks runs at mode 7; the
// 4-byte big-endian readers chain 2..5 (DICTID, compared into strm->adler) and 8..11 (trailer, compared
// with the blocks' computed hold). Every data-error path also stores 5 in state+4 before BAD.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_10250(B81ZStream* z, int32_t flush) {
    if (z == nullptr || z->state == 0 || z->next_in == 0) return -2;
    B81ZState* st = (B81ZState*)(uintptr_t)z->state;
    if (st->mode > 0xD) return -2;
    const int32_t ok = (int32_t)(((uint32_t)(flush != 4) - 1u) & 0xFFFFFFFBu);
    int32_t ret = -5;
    for (;;) {
        switch (st->mode) {
        case 0:                                                 // HEAD: first header byte
            if (z->avail_in == 0) return ret;
            ++z->total_in; --z->avail_in;
            st->hold = *(const uint8_t*)(uintptr_t)z->next_in;
            ++z->next_in;
            if ((st->hold & 0xF) == 8) {
                if ((st->hold >> 4) + 8 <= st->wbits) { st->mode = 1; ret = ok; break; }
                st->mode = 0xD; z->msg = (uint32_t)(uintptr_t)kBadWindow; st->hold = 5; ret = ok; break;
            }
            st->mode = 0xD; z->msg = (uint32_t)(uintptr_t)kBadMethod; st->hold = 5; ret = ok; break;
        case 1: {                                               // FLAGS: second byte, % 31, FDICT -> 2
            if (z->avail_in == 0) return ret;
            --z->avail_in; ++z->total_in;
            const uint8_t b = *(const uint8_t*)(uintptr_t)z->next_in;
            st->hold = st->hold * 0x100 + b;
            ++z->next_in;
            if (st->hold % 0x1F != 0) {
                st->mode = 0xD; z->msg = (uint32_t)(uintptr_t)kBadHeader; st->hold = 5; ret = ok; break;
            }
            if ((b & 0x20) != 0) { st->mode = 2; ret = ok; goto dictid; }
            st->mode = 7; ret = ok;
            break;
        }
        case 2:                                                 // DICTID, big endian, 4 bytes (2..5)
        dictid:
            if (z->avail_in == 0) return ret;
            --z->avail_in; ++z->total_in;
            st->check = (uint32_t)*(const uint8_t*)(uintptr_t)z->next_in << 24;
            ++z->next_in; st->mode = 3; ret = ok;
            // fall through
        case 3:
            if (z->avail_in == 0) return ret;
            --z->avail_in; ++z->total_in;
            st->check += (uint32_t)*(const uint8_t*)(uintptr_t)z->next_in << 16;
            ++z->next_in; st->mode = 4; ret = ok;
            // fall through
        case 4:
            if (z->avail_in == 0) return ret;
            --z->avail_in; ++z->total_in;
            st->check += (uint32_t)*(const uint8_t*)(uintptr_t)z->next_in << 8;
            ++z->next_in; st->mode = 5; ret = ok;
            // fall through
        case 5:
            if (z->avail_in == 0) return ret;
            --z->avail_in; ++z->total_in;
            st->check += *(const uint8_t*)(uintptr_t)z->next_in;
            ++z->next_in;
            z->adler = st->check;
            st->mode = 6;
            return 2;                                           // Z_NEED_DICT
        case 6:                                                 // DICT
            st->mode = 0xD;
            z->msg = (uint32_t)(uintptr_t)kNeedDict;
            st->hold = 0;
            return -2;
        case 7: {                                               // blocks
            const int32_t r0 = g_inflate.blocks
                ? g_inflate.blocks((B81ZBlocks*)(uintptr_t)st->blocks, z, ret) : -2;
            if (r0 == -3) { st->mode = 0xD; st->hold = 0; ret = -3; break; }
            int32_t r = r0;
            if (r == 0) r = ok;
            if (r != 1) return r;
            if (g_inflate.blocks_end) g_inflate.blocks_end();
            if (st->raw == 0) { st->mode = 8; ret = ok; goto trailer; }
            st->mode = 0xC; ret = ok;
            break;
        }
        case 8:                                                 // trailer, big endian, 4 bytes (8..11)
        trailer:
            if (z->avail_in == 0) return ret;
            --z->avail_in; ++z->total_in;
            st->check = (uint32_t)*(const uint8_t*)(uintptr_t)z->next_in << 24;
            ++z->next_in; st->mode = 9; ret = ok;
            // fall through
        case 9:
            if (z->avail_in == 0) return ret;
            --z->avail_in; ++z->total_in;
            st->check += (uint32_t)*(const uint8_t*)(uintptr_t)z->next_in << 16;
            ++z->next_in; st->mode = 10; ret = ok;
            // fall through
        case 10:
            if (z->avail_in == 0) return ret;
            --z->avail_in; ++z->total_in;
            st->check += (uint32_t)*(const uint8_t*)(uintptr_t)z->next_in << 8;
            ++z->next_in; st->mode = 11; ret = ok;
            // fall through
        case 11:
            if (z->avail_in == 0) return ret;
            --z->avail_in; ++z->total_in;
            st->check += *(const uint8_t*)(uintptr_t)z->next_in;
            ++z->next_in;
            if (st->hold == st->check) { st->mode = 0xC; return 1; }
            st->mode = 0xD; z->msg = (uint32_t)(uintptr_t)kBadData; st->hold = 5; ret = ok;
            break;
        case 12:                                                // DONE
            return 1;
        case 13:                                                // BAD
            return -3;
        default:
            return -2;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------
// D2Client's own rows.

// +0x11C564  DAT_6fbcc564: the one-shot latch of 0x10950.
struct D2ClientUserEnv {
    int32_t (*get_user_name)(char* buf, uint32_t* len);              // IAT GetUserNameA
    int32_t (*sstrcmp)(const char*, const char*, uint32_t);          // Storm #508 SStrCmp (via 0xBF6E)
    int32_t (*message_box)(const char* text, const char* caption);   // IAT MessageBoxA(NULL, text, caption, 0)
    const char* help_text;                                           // retail: the command-line options banner
    const char* caption;                                             // retail: "Diablo II"
};

// 1.13c D2Client+0x10950
// One-shot (latch +0x11C564): GetUserNameA(name, 0xF), SStrCmp(name, "rseis", 0x7FFFFFFF); a nonzero result
// pops the command-line options MessageBox. Returns 0.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_10950(uint32_t* latch, const D2ClientUserEnv* env) {
    if (*latch == 0) {
        *latch = 1;
        char name[16] = {};
        uint32_t len = 0xF;
        int32_t r = 0;
        if (env && env->get_user_name && env->sstrcmp) {
            env->get_user_name(name, &len);
            r = env->sstrcmp(name, "rseis", 0x7FFFFFFF);
        }
        if (r != 0 && env && env->message_box) env->message_box(env->help_text, env->caption);
    }
    return 0;
}

// +0x0EBC48  DAT_6fb8bc48 screen width, +0x0F9E18 DAT_6fba9e18 H, +0x1233F4 DAT_6fbd33f4 thumb Y.
// 0x11020/0x11110 are two of the six HUD scrollbar hit boxes dispatched by 0x11A80 (hit D and hit A,
//); 0x11180 is its region probe over three rects +0x0FA8BA2, stride 0x1A.

// 1.13c D2Client+0x11020
// Hit D: x in [w - m - 0x1F, w - m - 0x12], m = (w - 0x26C) / 2; y in ((h - 0x1A4) / 2 + 0x2F, thumbY).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_11020(int32_t x, int32_t y, int32_t w, int32_t h, int32_t thumbY) {
    const int32_t m = (w - 0x26C) / 2;
    return (x >= (w - m) - 0x1F && x <= (w - m) - 0x12 && y > (h - 0x1A4) / 2 + 0x2F && y < thumbY) ? 1 : 0;
}

// 1.13c D2Client+0x11110
// Hit A: x in [m + 0x12, m + ((w - 2m - 0x31) / 3 + 6) * 3], m = (w - 0x26C) / 2; y in
// [(h - 0x1A4) / 2 + 0x28, (h - 0x1A4) / 2 + 0x136].
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_11110(int32_t x, int32_t y, int32_t w, int32_t h) {
    const int32_t m = (w - 0x26C) / 2;
    if (!(x >= m + 0x12 && x <= m + ((w + m * -2 - 0x31) / 3 + 6) * 3)) return 0;
    const int32_t n = (h - 0x1A4) / 2;
    return (y >= n + 0x28 && y <= n + 0x136) ? 1 : 0;
}

// 1.13c D2Client+0x11180
// Region probe: the first of three 0x1A-byte records at +0x0FA8BA2 whose (x1, x2) at +0/+4 and (y1, y2) at
// +8/+0xC contain (x, y); 3 when none does. The loads are unaligned.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_11180(int32_t x, int32_t y, const uint8_t* table) {
    int32_t i = 0;
    for (uint32_t off = 0; off <= 0x4D; off += 0x1A, ++i) {
        int32_t x1, x2, y1, y2;
        std::memcpy(&x1, table + off, 4);
        std::memcpy(&x2, table + off + 4, 4);
        std::memcpy(&y1, table + off + 8, 4);
        std::memcpy(&y2, table + off + 0xC, 4);
        if (!(x < x1 || x > x2 || y < y1 || y > y2)) return i;
    }
    return i;
}

// 0x13390 and 0x13450 are the two 6x6 light-grid builders of the per-floor-tile submit 0x13CE0
// (src/core/d2client/floor_13ce0.cpp): each writes 36 12-byte records, mode 1/2 shifting x by -/+W/4.
struct D2ClientGridEnv {
    // Ordinal_10023: (x << 16, y << 16, param_3, &x, &y) - the screen-space pair is the whole observable.
    void (*world_to_screen)(int32_t x, int32_t y, uint32_t p3, int32_t* ox, int32_t* oy);
};

// 1.13c D2Client+0x13390
// Records are filled only at param_4 == 0 or on the four corner cells; the offset-shifted x comes from the
// 0x10023 transform, the y straight through. Bytes 0..3 of every filled record are 0xFF.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_13390(uint8_t* out, int32_t mode, int32_t a, int32_t b,
                                                     uint32_t p3, int32_t cornersOnly, uint32_t w,
                                                     const D2ClientGridEnv* env) {
    int32_t shift = 0;                                    // local_4: mode 1 -> -W/4, mode 2 -> +W/4
    if (mode == 1) shift = -((int32_t)(w + (int32_t)(w >> 31 & 3u)) >> 2);
    else if (mode == 2) shift = (int32_t)(w + (int32_t)(w >> 31 & 3u)) >> 2;
    uint8_t* rec = out;
    for (int32_t row = 0; row < 6; ++row)
        for (int32_t col = 0; col < 6; ++col, rec += 0xC) {
            if (cornersOnly != 0 && !((col == 0 || col == 5) && (row == 0 || row == 5))) continue;
            int32_t ox = 0, oy = 0;
            if (env && env->world_to_screen)
                env->world_to_screen((a + col) * 0x10000, (row + b) * 0x10000, p3, &ox, &oy);
            ox += shift;
            rec[4] = (uint8_t)ox; rec[5] = (uint8_t)(ox >> 8); rec[6] = (uint8_t)(ox >> 16); rec[7] = (uint8_t)(ox >> 24);
            rec[8] = (uint8_t)oy; rec[9] = (uint8_t)(oy >> 8); rec[10] = (uint8_t)(oy >> 16); rec[11] = (uint8_t)(oy >> 24);
            rec[0] = 0xFF; rec[1] = 0xFF; rec[2] = 0xFF; rec[3] = 0xFF;
        }
}

// 1.13c D2Client+0x13450
// The direct isometric variant: no transform. mode 1/2 shift the y base by -/+W/4; then
// x = (col - row) * 0x10 + yBase, y = xBase + (col + row) * 8, on the same 20-cell mask.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_13450(uint8_t* out, int32_t mode, int32_t xBase, int32_t yBase,
                                                     int32_t cornersOnly, uint32_t w) {
    if (mode == 1) yBase -= ((int32_t)(w + (uint32_t)(w >> 31 & 3u)) >> 2);
    else if (mode == 2) yBase += ((int32_t)(w + (uint32_t)(w >> 31 & 3u)) >> 2);
    uint8_t* rec = out;
    for (int32_t row = 0; row < 6; ++row)
        for (int32_t col = 0; col < 6; ++col, rec += 0xC) {
            if (cornersOnly != 0 && !((col == 0 || col == 5) && (row == 0 || row == 5))) continue;
            const int32_t rx = (col - row) * 0x10 + yBase;
            const int32_t ry = xBase + (col + row) * 8;
            rec[4] = (uint8_t)rx; rec[5] = (uint8_t)(rx >> 8); rec[6] = (uint8_t)(rx >> 16); rec[7] = (uint8_t)(rx >> 24);
            rec[8] = (uint8_t)ry; rec[9] = (uint8_t)(ry >> 8); rec[10] = (uint8_t)(ry >> 16); rec[11] = (uint8_t)(ry >> 24);
            rec[0] = 0xFF; rec[1] = 0xFF; rec[2] = 0xFF; rec[3] = 0xFF;
        }
}

// +0x143E0 is the send path (src/core/d2client/packet_builders.cpp): buffer in ECX, byte count in EBX.
// +0x0D359F DAT_6fb8359f is the assert context string; the fatal tail is Fog #10265(code) ->
// Fog #10024(context, eip) -> _exit(-1).
struct D2ClientSendEnv {
    void (*send)(const uint8_t* buf, uint32_t size);   // +0x143E0
    void (*fatal)(uint32_t code);                      // Fog #10265 / #10024
    void (*exit_)(int32_t code);
};

// 1.13c D2Client+0x14720
// Packet 0x66: [0x66][len:u16 LE][payload:len], sent with size len + 3. len outside 0..0x1FD is fatal
// 0x442, the buffer bound (len + 3 < 0x201, dead after the first test) is 0x443.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_14720(int32_t len, const uint8_t* payload, const D2ClientSendEnv* env) {
    uint8_t buf[0x203];
    if (len < 0 || len > 0x1FD) {
        if (env && env->fatal) env->fatal(0x442);
    } else if ((uint32_t)len + 3u < 0x201u) {
        buf[0] = 0x66;
        buf[1] = (uint8_t)len;
        buf[2] = (uint8_t)((uint32_t)len >> 8);
        if (len > 0) std::memcpy(buf + 3, payload, (size_t)len);
        if (env && env->send) env->send(buf, (uint32_t)len + 3u);
        return;
    } else {
        if (env && env->fatal) env->fatal(0x443);
    }
    if (env && env->exit_) env->exit_(-1);
}

// 1.13c D2Client+0x14850
// Packet 0x5D: [0x5D][param_3:u8][param_4 != 0:u8][param_2:u32 LE] -- seven bytes written; the send
// length is EBX and is not in the decompile (7 is what the body fills).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_14850(uint32_t unused, uint32_t value, uint8_t b, int32_t flag,
                                                     const D2ClientSendEnv* env) {
    (void)unused;
    uint8_t buf[7];
    buf[0] = 0x5D;
    buf[1] = b;
    buf[2] = flag != 0 ? 1 : 0;
    buf[3] = (uint8_t)value;
    buf[4] = (uint8_t)(value >> 8);
    buf[5] = (uint8_t)(value >> 16);
    buf[6] = (uint8_t)(value >> 24);
    if (env && env->send) env->send(buf, 7);
}
