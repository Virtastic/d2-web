// 1.13c D2Game+0x29CB0 -- D2Net client-event pump body (caller 0x2E460, third loop): one 0x200-byte
// message from D2Net #10014 arrives in EAX (client id at +0, event type byte at +5). Types 1/0xFD
// forward a host game-list record through the event callback table slot +0x30; 0xFA/0xFB/0xFC are
// connect / disconnect / update notices answered with a 0x10-byte reply. Retail's stack frame is
// reproduced as one byte array addressed by negative offsets so the overlapping packed copies match.
#include <cstdint>
#include <cstring>

struct D2Game29CB0Env {
    uint32_t tablePresent;                  // DAT 0x6FD31C3C != 0
    uint32_t slot30Present, slot10;         // table+0x30 != 0 (function), table+0x10 value
    void (*slot30)(void* frame);            // table+0x30; fills the frame, no visible args
    void (*strCopy)(void* dst, const void* src, uint32_t n); // Ordinal_501
    void (*send)(uint32_t list, uint32_t clientId, const void* data, uint32_t len); // 0xA35E, #10002
    void (*mode)(uint32_t clientId, void* out16);   // 0xA37C, #10017
    void (*close)(uint32_t clientId);               // 0xA382, #10000
    int32_t* counter;                       // DAT 0x6FD31F8C, Interlocked
};

void D2GAME_ClientEvent_29CB0(const uint8_t* msg, const D2Game29CB0Env* e)
{
    if (!e->tablePresent) return;
    uint8_t f[0x290] = {};
    auto A = [&](int n) { return f + (0x28c - n); };
    auto cp = [&](int d, int s, int n = 4) { std::memmove(A(d), A(s), n); };
    auto r32 = [&](int n) { int32_t v; std::memcpy(&v, A(n), 4); return v; };
    auto w32 = [&](int n, int32_t v) { std::memcpy(A(n), &v, 4); };
    uint32_t id; std::memcpy(&id, msg, 4);
    switch (msg[5]) {
    case 1: case 0xfd: {
        if (!e->slot30Present || !e->slot10) return;
        e->slot30(f);
        *A(0x168) = msg[5];
        cp(0x167, 0x27c); cp(0x163, 0x278); cp(0x15f, 0x274); cp(0x15b, 0x270); cp(0x157, 0x26c);
        cp(0x153, 0x268); cp(0x14f, 0x264); cp(0x14b, 0x260); cp(0x147, 0x25c); cp(0x143, 0x258);
        e->strCopy(A(0x13f), A(0x254), 0x80);
        cp(0xcb, 0x1e0);
        int32_t a1dc = r32(0x1dc), a1d8 = r32(0x1d8), a1d0 = r32(0x1d0), a1cc = r32(0x1cc);
        int32_t cnt = *e->counter;
        w32(0x69, a1dc); w32(0x65, a1d8); w32(0x61, a1d0 - cnt);
        w32(0xc7, a1d8 + a1dc); cp(0xc3, 0x1d4); w32(0x5d, a1cc);
        cp(0xbb, 0x1c8); cp(0xb7, 0x19c); w32(0xbf, a1cc + (a1d0 - cnt));
        cp(0xb3, 0x198, 32);
        cp(0x93, 0x178); cp(0x8f, 0x1c4); cp(0x8b, 0x1c0); cp(0x87, 0x1bc); cp(0x83, 0x1b8);
        cp(0x7f, 0x1b4, 2); cp(0x7d, 0x1b2); cp(0x79, 0x1ae); cp(0x75, 0x1aa); cp(0x71, 0x1a6); cp(0x6d, 0x1a2);
        std::memset(A(0x59), 0, 0x5a);
        e->send(2, id, A(0x174), *A(0x174) == 1 ? 0x165 : 0xbd);
        return;
    }
    case 0xfa:
        e->mode(id, A(0x28c));
        __atomic_add_fetch(e->counter, 1, __ATOMIC_SEQ_CST);
        e->send(2, id, A(0x28c), 0x10);
        return;
    case 0xfb:
        e->mode(id, A(0x28c));
        e->send(2, id, A(0x28c), 0x10);
        e->close(id);
        __atomic_sub_fetch(e->counter, 1, __ATOMIC_SEQ_CST);
        return;
    case 0xfc:
        e->mode(id, A(0x28c));
        e->send(2, id, A(0x28c), 0x10);
        return;
    }
}
