// Retail D2Game's trivial thunks in the 0x2A470-0x2A560 table: static-block address getters, constant-0 returns and empty bodies.
// No 1.10f the 1.10f source counterpart; each is taken from the 1.13c disassembly. Offsets are module-relative image offsets.
#include <cstdint>

// 1.13c D2Game+0x2A470 -- returns the address of the module's static block at image offset 0xFA968.
uint32_t d2game_2a470() { return 0xFA968u; }
// 1.13c D2Game+0x2A480 -- returns the address of the module's static block at image offset 0xFA950.
uint32_t d2game_2a480() { return 0xFA950u; }
// 1.13c D2Game+0x2A4A0 -- returns 0.
uint32_t d2game_2a4a0() { return 0; }
// 1.13c D2Game+0x2A4B0 -- returns 0.
uint32_t d2game_2a4b0() { return 0; }
// 1.13c D2Game+0x2A4C0 -- returns 0.
uint32_t d2game_2a4c0() { return 0; }
// 1.13c D2Game+0x2A460 -- empty body (stdcall, 4 stack words dropped).
void d2game_2a460(uint32_t, uint32_t, uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A4D0 -- empty body (stdcall, 4 stack words dropped).
void d2game_2a4d0(uint32_t, uint32_t, uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A4E0 -- empty body (stdcall, 2 stack words dropped).
void d2game_2a4e0(uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A4F0 -- returns the module static block at image offset 0xFA898.
uint32_t d2game_2a4f0() { return 0xFA898u; }
// 1.13c D2Game+0x2A500 -- empty body.
void d2game_2a500(uint32_t) {}
// 1.13c D2Game+0x2A510 -- returns 0.
uint32_t d2game_2a510() { return 0u; }
// 1.13c D2Game+0x2A520 -- empty body.
void d2game_2a520() {}
// 1.13c D2Game+0x2A530 -- empty body.
void d2game_2a530(uint32_t, uint32_t) {}

#include <cstring>
// D2Net client-event pump body (caller 0x2E460, third loop): one 0x200-byte message from D2Net #10014 arrives in EAX
// (client id at +0, event type byte at +5). Types 1/0xFD forward a host game-list record through the callback table slot +0x30;
// 0xFA/0xFB/0xFC are connect / disconnect / update notices answered with a 0x10-byte reply. Retail's stack frame is reproduced
// as one byte array addressed by negative offsets so the overlapping packed copies match. The callees are the env's seams.
struct D2Game29CB0Env {
    uint32_t tablePresent;                  // DAT 0x6FD31C3C != 0
    uint32_t slot30Present, slot10;         // table+0x30 != 0 (function), table+0x10 value
    void (*slot30)(void* frame);            // table+0x30; fills the frame
    void (*strCopy)(void* dst, const void* src, uint32_t n); // Ordinal_501
    void (*send)(uint32_t list, uint32_t clientId, const void* data, uint32_t len); // 0xA35E, #10002
    void (*mode)(uint32_t clientId, void* out16);   // 0xA382
    void (*close)(uint32_t clientId);               // 0xA37C
    int32_t* counter;                       // DAT 0x6FD31F8C, Interlocked
};

// 1.13c D2Game+0x29CB0
void D2GAME_ClientEvent_29CB0(const uint8_t* msg, const D2Game29CB0Env* e)
{
    if (!e->tablePresent) return;
    uint8_t f[0x2a0] = {};
    auto A = [&](int n) { return f + (0x29c - n); };
    auto cp = [&](int d, int s, int n = 4) { std::memmove(A(d), A(s), n); };
    auto r32 = [&](int n) { int32_t v; std::memcpy(&v, A(n), 4); return v; };
    auto w32 = [&](int n, int32_t v) { std::memcpy(A(n), &v, 4); };
    uint32_t id; std::memcpy(&id, msg, 4);
    switch (msg[5]) {
    case 1: case 0xfd: {
        if (!e->slot30Present || !e->slot10) return;
        e->slot30(A(0x28c));
        *A(0x168) = msg[5];
        cp(0x167, 0x27c); cp(0x163, 0x278); cp(0x15f, 0x274); cp(0x15b, 0x270); cp(0x157, 0x26c);
        cp(0x153, 0x268); cp(0x14f, 0x264); cp(0x14b, 0x260); cp(0x147, 0x25c); cp(0x143, 0x258);
        e->strCopy(A(0x13f), A(0x254), 0x80);
        cp(0xbf, 0x1d4);
        int32_t a1d0 = r32(0x1d0), a1cc = r32(0x1cc), a1c4 = r32(0x1c4), a1c0 = r32(0x1c0);
        int32_t cnt = *e->counter;
        w32(0x5d, a1d0); w32(0x59, a1cc); w32(0x55, a1c4 - cnt);
        w32(0xbb, a1cc + a1d0); cp(0xb7, 0x1c8); w32(0x51, a1c0);
        cp(0xaf, 0x1bc); cp(0xab, 0x190); w32(0xb3, a1c0 + (a1c4 - cnt));
        cp(0xa7, 0x18c, 32);
        cp(0x87, 0x16c); cp(0x83, 0x1b8); cp(0x7f, 0x1b4); cp(0x7b, 0x1b0); cp(0x77, 0x1ac);
        cp(0x73, 0x1a8, 2); cp(0x71, 0x1a6); cp(0x6d, 0x1a2); cp(0x69, 0x19e); cp(0x65, 0x19a); cp(0x61, 0x196);
        std::memset(A(0x4d), 0, 0x4a);
        e->send(2, id, A(0x168), *A(0x168) == 1 ? 0x165 : 0xbd);
        return;
    }
    case 0xfa:
        e->mode(id, A(0x28c));
        *A(0x29c) = 0xfa;   // the sent packet is its own 16 bytes below the reply, only the type byte set
        __atomic_add_fetch(e->counter, 1, __ATOMIC_SEQ_CST);
        e->send(2, id, A(0x29c), 0x10);
        return;
    case 0xfb:
        e->mode(id, A(0x28c));
        *A(0x29c) = 0xfb;
        e->send(2, id, A(0x29c), 0x10);
        e->close(id);
        __atomic_sub_fetch(e->counter, 1, __ATOMIC_SEQ_CST);
        return;
    case 0xfc:
        e->mode(id, A(0x28c));
        *A(0x29c) = 0xfc;
        e->send(2, id, A(0x29c), 0x10);
        return;
    }
}
// 1.13c D2Game+0x2A540 -- empty body, drops 5 stack arguments (20 bytes popped).
void d2game_2a540(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A550 -- empty body.
void d2game_2a550(uint32_t, uint32_t) {}
// 1.13c D2Game+0x2A560 -- empty body.
void d2game_2a560(uint32_t) {}
