// Shared vector file format, linked by BOTH the retail capture tool (Win32, retail DLLs) and the wasm
// engine so the two can never drift. Deliberately dumb: fixed little-endian header, then
// `count` records of `recordSize` bytes. Record layouts live here too.
//
//   +0  "D2VEC\0"            magic (6)
//   +6  u16 version          format version, 1
//   +8  u32 kind             D2VecKind
//   +12 u32 recordSize
//   +16 u32 count
//   +20 u32 reserved
//   +24 records...
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#pragma pack(push, 1)
struct D2VecHeader {
    char     magic[6];
    uint16_t version;
    uint32_t kind, recordSize, count, reserved;
};

// PRNG: one record = one seeded run. The retail seed struct is {lowSeed, highSeed}; `count`
// rolls are made (RollRandomNumber when nMax == 0, RollLimitedRandomNumber(nMax) otherwise),
// the final seed state is recorded, and `hash` is FNV-1a over the u32 result of every roll in
// order. A mismatch localises by re-running with smaller counts; storing every draw would put
// tens of MB in git for no extra evidence.
struct D2VecPrng {
    uint32_t lowIn, highIn, nMax, count;
    uint32_t lowOut, highOut, hash;
};

// DRLG: one record = one generated level. `hash` is FNV-1a over the level's subtile collision
// grid as the retail engine builds it (u16 per subtile, row-major, width*height cells, cells no
// room covers = 0xFFFF), the grid d2mapapi/libd2 compare cell-for-cell. Grids are ~11M cells
// per seed, so the vector keeps the hash; the retail capture tool writes the full grids to a scratch dir
// on request (D2CAPTURE_DRLG_FULL=<dir>) for diagnosis.
struct D2VecDrlg {
    uint32_t seed;
    uint8_t  difficulty, act;
    uint16_t level;
    int32_t  originX, originY;      // level position in subtiles (dwPosX*5, dwPosY*5)
    uint32_t width, height;         // in subtiles (dwSizeX*5, dwSizeY*5)
    uint32_t hash;
    uint32_t tombArea;              // ActMisc.RealTombArea for act 2 (which tomb is the real one), else 0
};
#pragma pack(pop)
static_assert(sizeof(D2VecHeader) == 24, "header is 24 bytes on every platform");
static_assert(sizeof(D2VecDrlg) == 32, "drlg record is 32 bytes on every platform");
static_assert(sizeof(D2VecPrng) == 28, "prng record is 28 bytes on every platform");

// TABLE: the in-memory excel table exactly as retail holds it after DATATBLS_LoadAllTxts --
// `count` records of `recordSize` bytes copied verbatim from sgptDataTables. One file per table
//; recordSize is sizeof the retail record struct, so a layout
// mismatch is refused by d2vec_read_header rather than compared byte-by-byte as garbage.
enum D2VecKind : uint32_t { D2VEC_PRNG = 1, D2VEC_DRLG = 2, D2VEC_ITEMGEN = 3, D2VEC_COMBAT = 4, D2VEC_TABLE = 5 };

static inline uint32_t d2vec_fnv1a_step(uint32_t h, uint32_t v) {
    for (int i = 0; i < 4; i++) { h ^= (v >> (8 * i)) & 0xFF; h *= 16777619u; }
    return h;
}
#define D2VEC_FNV_INIT 2166136261u

// Returns bytes written (0 on failure). Records are written verbatim; keep them packed and
// free of pointers.
static inline size_t d2vec_write(FILE* f, uint32_t kind, uint32_t recordSize, uint32_t count, const void* records) {
    D2VecHeader h = { {'D','2','V','E','C',0}, 1, kind, recordSize, count, 0 };
    if (fwrite(&h, sizeof h, 1, f) != 1) return 0;
    size_t n = (size_t)recordSize * count;
    if (n && fwrite(records, 1, n, f) != n) return 0;
    return sizeof h + n;
}

// Validates the header; returns false on a mismatch (wrong kind or record size is a versioning
// error, not a parity diff -- refuse rather than compare garbage).
static inline bool d2vec_read_header(FILE* f, uint32_t kind, uint32_t recordSize, D2VecHeader* h) {
    if (fread(h, sizeof *h, 1, f) != 1) return false;
    return memcmp(h->magic, "D2VEC", 6) == 0 && h->version == 1 && h->kind == kind && h->recordSize == recordSize;
}
