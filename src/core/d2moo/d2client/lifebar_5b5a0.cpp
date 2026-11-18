// 1.13c D2Client+0x5B5A0 life bar (46px). Seams: find (9D630), colour (D2Win #10190), fill (D2gfx #10014); head = [0x11C4D4]; cc = 4 bytes at 0x11C288.
#include <cstdint>
typedef uint32_t (*Rd32)(uint32_t addr);
struct LifeBarEnv {
    Rd32 rd; uint32_t (*find)(uint32_t ecx, int32_t id); uint32_t (*colour)(int32_t, int32_t, int32_t);
    void (*fill)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
};
void d2_client_5b5a0(const LifeBarEnv* e, uint32_t byId, uint32_t ecx, int32_t id, int32_t x, uint32_t head, uint8_t* cc) {
    int32_t life;
    if (byId == 0) {
        uint32_t r = e->find(ecx, id); if (!r) return;
        life = (int32_t)e->rd(r + 0x14);
    } else {
        uint32_t n = head; if (!n) return;
        while ((int32_t)e->rd(n + 8) != id) { n = e->rd(n + 0x30); if (!n) return; }
        life = (int32_t)e->rd(n + 0x1C);
    }
    if (!life) return;
    int32_t x1 = x + 0x2E;
    if (life < 1) life = 0; else if (life > 99) life = 100;
    int32_t mid = (life * 0x2E) / 100 + x;
    if (!cc[3]) { cc[3] = 1; cc[0] = (uint8_t)e->colour(0, 0x80, 0); cc[1] = (uint8_t)e->colour(0xC0, 0xC0, 0); cc[2] = (uint8_t)e->colour(0xFF, 0, 0); }
    uint8_t c = cc[0]; if (life < 0x32) c = cc[1]; if (life < 0x19) c = cc[2];
    e->fill(x, 0xE, mid, 0x13, c, 5); e->fill(mid, 0xE, x1, 0x13, 0, 5);
}
