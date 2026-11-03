// 1.13c D2Game+0x54000 D2GAME_SAVE_WriteFileOnRealm.
#include "d2game_53f10.h"
#include <cstring>

static inline uint32_t rd32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
static inline void wr32(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }

// The four header dwords a version > 0x5B save carries that never decide "unchanged" (+0xC, +0x30, +0x34, +0xAB).
static const uint32_t kVolatile[4] = { 0x0C, 0x30, 0x34, 0xAB };

// 1.13c D2Game+0x54000: build the save stream (EC7F0) behind a size+2 word; if its checksum (53170) equals the client's (+0x18C)
// and the old save (49790/49760) has the same size and bytes (volatile header words masked on both sides), count it, call the
// relock callback (+0x38) and store s5 at client+0x68: return 0. Otherwise store s5 and the checksum; with a save callback
// (+0xC; IsBadCodePtr is fatal) hand it the stream, then attach the new save to the client (496B0). Return 1.
uint32_t D2GAME_SAVE_WriteFileOnRealm_54000(uint32_t c, uint8_t* player, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4,
                                           uint32_t s5, const D2SaveRealmEnv* e)
{
    if (*D2GAME_SAVE_WriteSaveFileFlag() == 0) return 0;
    uint8_t stream[0x2002];
    uint8_t* buf = stream + 2;
    uint32_t size = 0;
    e->createSaveFile((uint32_t)(uintptr_t)player, s0, buf, &size, 0x2000, c, s4);
    if ((int32_t)size <= 0) return 0;
    const uint32_t len = size + 2;
    stream[0] = (uint8_t)len; stream[1] = (uint8_t)(len >> 8);
    uint8_t* client = nullptr;
    if (player && rd32(player) == 0) client = (uint8_t*)(uintptr_t)rd32(e->playerData(player) + 0x9C);
    if (!client) { e->fatal(0x7EB); return 0; }
    const uint32_t prev = rd32(client + 0x18C);
    const uint32_t ck = e->checksum(buf, len - 2);
    bool same = false;
    if (ck == prev) {
        uint8_t* old = e->oldSave(client);
        const uint32_t osz = e->oldSaveSize(client);
        if (!old) { e->fatal(0xA3B); return 0; }
        if (osz == len - 2) {
            const uint32_t ov = rd32(old + 4), nv = rd32(buf + 4);
            uint32_t os[4] = {}, ns[4] = {};
            if (ov > 0x5B) for (int i = 0; i < 4; i++) { os[i] = rd32(old + kVolatile[i]); wr32(old + kVolatile[i], 0); }
            if (nv > 0x5B) for (int i = 0; i < 4; i++) { ns[i] = rd32(buf + kVolatile[i]); wr32(buf + kVolatile[i], 0); }
            same = true;
            for (int32_t i = 0; i < (int32_t)osz; i++) if (old[i] != buf[i]) { same = false; break; }
            if (ov > 0x5B) for (int i = 0; i < 4; i++) wr32(old + kVolatile[i], os[i]);
            if (nv > 0x5B) for (int i = 0; i < 4; i++) wr32(buf + kVolatile[i], ns[i]);
        }
    }
    if (same) {
        ++*D2GAME_SAVE_SameSaveCount();
        uint8_t* tbl = e->callbackTable();
        if (tbl && rd32(tbl + 0x38)) e->relock(rd32(tbl + 0x38), &s5, s1, s2);
        wr32(client + 0x68, s5);
        return 0;
    }
    wr32(client + 0x68, s5);
    wr32(client + 0x18C, ck);
    uint8_t* tbl = e->callbackTable();
    if (tbl && rd32(tbl + 0x0C)) {
        if (e->isBadCodePtr(rd32(tbl + 0x0C))) { e->fatal(0xAD1); return 0; }
        e->save(rd32(tbl + 0x0C), &s5, s1, s2, stream, len, s3);
        e->attachSave(client, len - 2, buf);
    }
    return 1;
}
