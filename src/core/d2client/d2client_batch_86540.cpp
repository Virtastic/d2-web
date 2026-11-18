// src/core/d2client/d2client_batch_86540.cpp -- 1.13c D2Client+0x86540 (599 B): an item packet naming a record (the packet's owner) and a unit: the unit is
// updated or created, placed into the record's inventory and the screens refreshed. Every callee is a typed seam in retail's argument order and register use.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c86540Env {
    void     (*dec11105)(const uint8_t* body, int32_t len, uint32_t z, uint8_t* hdr);                 // thunk 0xCA90
    const uint32_t* buckets;                                                                             // unit hash table 0x10AE08, link +0xE4
    uint8_t*  player;                                                                                 // 0x11BBFC
    uint8_t* (*fn54e20)(uint32_t eax, uint32_t edx, uint32_t idx);                                    // 0xA4E20: EAX = id & 0x7F, EDX = table entry, stack = index
    void     (*fatal)(uint32_t code);                                                                 // Ordinal 10265 + 10024 + _exit(-1)
    int      (*ord10197)(uint32_t v, uint32_t mask);                                                  // 0xCA3C
    void     (*fn32490)(uint8_t* esiRec, uint8_t* unit);                                              // 0x82490: ESI = record, stack = unit
    uint8_t* (*fn333f0)(const uint8_t* pkt, void* out, uint32_t z, uint32_t flag);                    // 0x833F0
    void     (*fn32980)(uint8_t* eaxUnit, uint32_t z, uint32_t hdr8, uint32_t z2, uint32_t hdr10);    // 0x82980
    void     (*ord10058)(uint8_t* unit, uint32_t v);                                                  // 0xC9DC
    int      (*ord10405)(uint8_t* inv, uint8_t* unit, uint32_t mode);                                 // 0xCA6C
    int      (*fn31b40)(void);                                                                        // 0x81B40
    void     (*fn193c0)(uint8_t* rec);                                                                // 0x693C0, EDX = record
    void     (*ord10138)(uint8_t* inv, uint8_t* unit);                                                // 0xCA18
    void     (*fn18cb0)(uint8_t* eaxRec, uint32_t edx);                                               // 0x68CB0
    void     (*fn19a90)(uint32_t eaxAl, uint8_t* ecxRec);                                             // 0x69A90
    void     (*fn1a220)(uint8_t* eaxRec);                                                             // 0x6A220
    void     (*fn31b70)(uint8_t* eaxUnit);                                                            // 0x81B70
    void     (*ord10466)(uint8_t* inv, uint32_t z);                                                   // 0xC7D8
    void     (*ord11017)(uint8_t* inv);                                                               // 0xC202
    void     (*fn62d0)(void);                                                                         // 0x162D0
    void     (*fn31ff0)(uint32_t z);                                                                  // 0x81FF0
    void     (*fn32fe0)(uint8_t* rec);                                                                // 0x82FE0
    void     (*fn6e2b0)(void);                                                                        // 0xBE2B0
    void     (*fn31aa0)(uint8_t* esiUnit);                                                            // 0x81AA0
    void     (*fn7ca0)(int32_t v);                                                                    // 0x47CA0
};

static uint8_t* rp86540(const uint8_t* p, size_t o) { uint32_t v; memcpy(&v, p + o, 4); return (uint8_t*)(uintptr_t)v; }   // retail pointers in structs are 32-bit words
template <class T> static T rd86540(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }

// 1.13c D2Client+0x86540 (stdcall 1: the packet)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_86540(const uint8_t* pkt, const D2c86540Env* e) {
    uint8_t hdr[24] = {0};
    e->dec11105(pkt + 0xd, (int32_t)pkt[2] - 0xd, 0, hdr);
    uint8_t* pl = e->player;
    if (!pl) return;
    uint32_t idx = pkt[8];
    uint8_t* rec = e->fn54e20(rd86540<uint32_t>(pkt, 9) & 0x7f, 0x6fbba608u + (idx << 9), idx);
    if (!rec) return;
    uint32_t id = rd86540<uint32_t>(pkt, 4);
    uint8_t* u = nullptr;
    for (uint8_t* p = (uint8_t*)(uintptr_t)e->buckets[id & 0x7f]; p; p = rp86540(p, 0xE4))
        if (rd86540<uint32_t>(p, 0xC) == id) { if (rd86540<int32_t>(p, 0) != 4) { e->fatal(0x5a); return; } u = p; break; }
    uint8_t* lu = u;
    uint8_t* edi = u;
    uint32_t flags = rd86540<uint32_t>(hdr, 0xc);
    if (u && !e->ord10197(flags, 1)) {
        e->fn32980(u, 0, hdr[8], 0, rd86540<uint32_t>(hdr, 0x10));
    } else if (u) {
        e->fn32490(rec, u);
        lu = edi = e->fn333f0(pkt, &lu, 0, 0);
    } else {
        lu = edi = e->fn333f0(pkt, hdr, 0, rec == pl ? 1u : 0u);
    }
    if (edi) e->ord10058(edi, rd86540<uint32_t>(hdr, 0x11));
    uint8_t* inv = rp86540(rec, 0x60);
    if (!inv) return;
    uint32_t kind = hdr[0x11];
    uint32_t mode = (kind == 0xb || kind == 0xc) ? 4 : 3;
    if (!e->ord10405(inv, edi, mode)) return;
    if (!e->fn31b40()) return;
    e->fn193c0(rec);
    if (mode == 3) e->ord10138(inv, lu);
    e->fn18cb0(rec, 0);
    int32_t t = rd86540<int32_t>(rec, 0);
    if (t < 0 || t > 1) return;
    uint32_t k3 = pkt[3];
    if (k3 == 0 || k3 == 1) e->fn19a90(k3, rec);
    e->fn1a220(rec);
    if (rec == pl) {
        e->fn31b70(lu);
        if (!e->ord10197(flags, 8)) { e->ord10466(inv, 0); e->ord11017(inv); e->fn62d0(); }
    }
    if (kind < 0xb) e->fn31ff0(0);
    e->fn32fe0(rec);
    e->fn32fe0(rec);
    e->fn6e2b0();
    e->fn31aa0(lu);
    e->fn7ca0(lu ? rd86540<int32_t>(lu, 0xc) : -1);
}
