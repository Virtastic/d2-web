// 1.13c D2Client+0x5EE70: the Sounds row a submit of `id` plays. A row with Group Size g > 1 draws id + rand(g) on the
// client hero seed (0x89640), redrawing while the candidate matches one of the row's last picks (+0x64, +0x68): two
// of them for g >= 3, one for g == 2; for g <= 3 a first draw rand(g + 1) == 0 lowers that count by one.
// No Sounds table: the row itself. A row outside the table reads record 0 in retail (address 0): the row itself here.
#include <cstdint>
#include <cstring>

extern "C" int32_t d2_client_5ee70(const uint8_t* recs, int32_t count, int32_t id, uint32_t (*rand)(uint32_t)) {
    if (!recs || id < 0 || id >= count) return id;
    const uint8_t* r = recs + (size_t)id * 0x8E;
    const uint32_t g = r[0x3D];
    if (g <= 1) return id;
    int32_t avoid = (int32_t)g - 1 > 2 ? 2 : (int32_t)g - 1;
    if (g < 4 && rand(g + 1) == 0) --avoid;
    for (;;) {
        const int32_t cand = (int32_t)rand(g) + id;
        if (avoid < 1) return cand;
        int32_t i = 0;
        for (; i < avoid; ++i) { int32_t h; memcpy(&h, r + 0x64 + 4 * i, 4); if (h == cand) break; }
        if (i >= avoid) return cand;
    }
}
