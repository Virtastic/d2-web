// D2Net forwarders: each passes the server singleton (DAT_6fbfb424) and its one argument to a Fog import; the import is `imp`.
#include <cstdint>
typedef void (*FogImp)(uint32_t server, uint32_t a);
// 1.13c D2Net+0x5F40: (server, a) -> import at IAT +0x8074
extern "C" void d2net_sub_5f40(uint32_t server, uint32_t a, FogImp imp) { imp(server, a); }
// 1.13c D2Net+0x6050: (server, a) -> import at IAT +0x8054
extern "C" void d2net_sub_6050(uint32_t server, uint32_t a, FogImp imp) { imp(server, a); }
// 1.13c D2Net+0x6150: (server, a) -> import at IAT +0x8050
extern "C" void d2net_sub_6150(uint32_t server, uint32_t a, FogImp imp) { imp(server, a); }
// 1.13c D2Net+0x6220: (server, a) -> import at IAT +0x800C
extern "C" void d2net_sub_6220(uint32_t server, uint32_t a, FogImp imp) { imp(server, a); }
// 1.13c D2Net+0x70D0: mode (DAT_6fbfb418) 1 or 2 -> 1; else init flag (DAT_6fbfb244) 0 -> 0; else the locked read of DAT_6fbfb420 (`locked`).
extern "C" uint32_t d2net_sub_70d0(uint32_t mode, uint32_t flag, uint32_t (*locked)()) { return mode == 1 || mode == 2 ? 1 : flag ? locked() : 0; }
// Forwarders that also return the Fog import's result (retail leaves it in eax): the server singleton first, then the stack arguments.
typedef uint32_t (*FogImp1)(uint32_t server);
typedef uint32_t (*FogImp3)(uint32_t server, uint32_t a, uint32_t b);
typedef uint32_t (*FogImp4)(uint32_t server, uint32_t a, uint32_t b, uint32_t c);
// 1.13c D2Net+0x5F30: () -> import at IAT +0x806C (thunk +0x5ED0)
extern "C" uint32_t d2net_sub_5f30(uint32_t server, FogImp1 imp) { return imp(server); }
// 1.13c D2Net+0x6110: (a, b, c) -> import at IAT +0x8000 (thunk +0x5E8E)
extern "C" uint32_t d2net_sub_6110(uint32_t server, uint32_t a, uint32_t b, uint32_t c, FogImp4 imp) { return imp(server, a, b, c); }
// 1.13c D2Net+0x6290: (a, b) -> import at IAT +0x808C (thunk +0x5F00)
extern "C" uint32_t d2net_sub_6290(uint32_t server, uint32_t a, uint32_t b, FogImp3 imp) { return imp(server, a, b); }
// Void forwarders (the Fog import's result is discarded).
// 1.13c D2Net+0x6240: () -> import at IAT +0x807C (thunk +0x5EE8)
extern "C" void d2net_sub_6240(uint32_t server, void (*imp)(uint32_t)) { imp(server); }
// 1.13c D2Net+0x62B0: (a) -> import at IAT +0x805C (thunk +0x5EB8)
extern "C" void d2net_sub_62b0(uint32_t server, uint32_t a, void (*imp)(uint32_t, uint32_t)) { imp(server, a); }
// 1.13c D2Net+0x6130: (a) -> import at IAT +0x8048 (thunk +0x5E9A)
extern "C" void d2net_sub_6130(uint32_t server, uint32_t a, void (*imp)(uint32_t, uint32_t)) { imp(server, a); }
// 1.13c D2Net+0x6270: (a, b) -> import at IAT +0x8088 (thunk +0x5EFA); b is popped (ret 8) and never pushed, no server
extern "C" void d2net_sub_6270(uint32_t a, uint32_t, void (*imp)(uint32_t)) { imp(a); }
// 1.13c D2Net+0x61B0: (a, b) -> import at IAT +0x8064 (thunk +0x5EC4), pushed as (server, 0, a, b)
extern "C" void d2net_sub_61b0(uint32_t server, uint32_t a, uint32_t b, void (*imp)(uint32_t, uint32_t, uint32_t, uint32_t)) { imp(server, 0, a, b); }
// 1.13c D2Net+0x60B0 / 0x60F0 / 0x60D0 (ords 10025 / 10027 / 10029): (a, b, c) -> import at IAT +0x8060 / +0x8004 / +0x8014, result returned
extern "C" uint32_t d2net_sub_60b0(uint32_t server, uint32_t a, uint32_t b, uint32_t c, FogImp4 imp) { return imp(server, a, b, c); }
extern "C" uint32_t d2net_sub_60f0(uint32_t server, uint32_t a, uint32_t b, uint32_t c, FogImp4 imp) { return imp(server, a, b, c); }
extern "C" uint32_t d2net_sub_60d0(uint32_t server, uint32_t a, uint32_t b, uint32_t c, FogImp4 imp) { return imp(server, a, b, c); }
// 1.13c D2Net+0x6020 (ord 10021): a == 0 -> 0 with no call; else (server, a, b) -> import at IAT +0x801C, result returned
extern "C" uint32_t d2net_sub_6020(uint32_t server, uint32_t a, uint32_t b, FogImp3 imp) { return a ? imp(server, a, b) : 0; }
// 1.13c D2Net+0x6090 (ord 10000): (server, id, srcPath, line) -> import at IAT +0x8008, no result. The path is the module string at RVA 0x9298, line 0x1cb.
typedef void (*FogImp4v)(uint32_t server, uint32_t id, uint32_t path, uint32_t line);
extern "C" void d2net_sub_6090(uint32_t server, uint32_t id, uint32_t path, FogImp4v imp) { imp(server, id, path, 0x1cb); }
// 1.13c D2Net+0x6590 (ord 10005): (a, b) -> import at IAT +0x8090 as (a, 3, 4000, b, cb0..cb3); the returned handle goes to DAT_6fbfb424 (the caller's store) and is the result.
typedef uint32_t (*FogImp8)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
extern "C" uint32_t d2net_sub_6590(uint32_t a, uint32_t b, const uint32_t cb[4], FogImp8 imp) { return imp(a, 3, 4000, b, cb[0], cb[1], cb[2], cb[3]); }
// 1.13c D2Net+0x7200 (dequeue, under the CS): node = *headSlot (DAT_6fbfb3fc); none -> -1. Mode 2 (DAT_6fbfb418) and a node younger than 500 ms (tick - node+0x208) -> -1.
// Else copy min(len, 0x204) bytes of the node to dst, *headSlot := node+0x20c, free the node (release = the Fog free import), return node+0x204. The CS imports are not modelled.
extern "C" uint32_t d2net_sub_7200(uint8_t* dst, uint32_t len, uint32_t* headSlot, uint32_t mode, uint32_t (*tick)(), void (*release)(uint8_t*)) {
    uint8_t* node = (uint8_t*)(uintptr_t)*headSlot;
    if (!node || (mode == 2 && tick() - *(uint32_t*)(node + 0x208) < 0x1f4)) return 0xFFFFFFFFu;
    const uint32_t n = len < 0x204 ? len : 0x204, size = *(uint32_t*)(node + 0x204);
    for (uint32_t i = 0; i < n; ++i) dst[i] = node[i];
    *headSlot = *(uint32_t*)(node + 0x20c); release(node); return size;
}
// 1.13c D2Net+0x61F0 (void): pushes (server, &local, 1) with local = 0xB0 to the import at IAT +0x804c; DAT_6fbfb424 is then cleared (the caller's store).
extern "C" void d2net_sub_61f0(uint32_t server, void (*imp)(uint32_t, const uint8_t*, uint32_t)) { const uint8_t b = 0xB0; imp(server, &b, 1); }
// 1.13c D2Net+0x7540 (ord 10034, edi = mode): queue head/tail cleared, CS + zeroed 0x7bc-byte block set up (not modelled), mode stored, sub_63E0 run; modes 1/2 return 1 (other modes start the receive thread, not modelled).
extern "C" uint32_t d2net_sub_7540(uint32_t mode) { return mode == 1 || mode == 2; }
// 1.13c D2Net+0x7120 (stdcall, 1 arg): mode (DAT_6fbfb418) not 1/2 -> under the CS (Fog import, not modelled) DAT_6fbfb420 := v.
extern "C" uint32_t d2net_sub_7120(uint32_t mode, uint32_t* slot, uint32_t v) { if (mode != 1 && mode != 2) { *slot = v; return 0; } return mode; }   // eax at return as retail leaves it: the mode (1/2), else 0
// 1.13c D2Net+0x7240 (fastcall ecx = a, eax passed through): r = Fog#19(server DAT_6fbfb248, a, eax, 0); r != -1 -> r.
// Else Fog#111() and, if mode (DAT_6fbfb418) is not 1/2, DAT_6fbfb420 := 0 under the CS; returns 0.
extern "C" uint32_t d2net_sub_7240(uint32_t server, uint32_t a, uint32_t eaxv, uint32_t mode, uint32_t* slot, uint32_t (*q)(uint32_t, uint32_t, uint32_t, uint32_t), void (*f111)()) {
    const uint32_t r = q(server, a, eaxv, 0);
    if (r != 0xFFFFFFFFu) return r;
    f111(); if (mode != 1 && mode != 2) *slot = 0;
    return 0;
}
// 1.13c D2Net+0x63E0 (ord 10002, stdcall(flag, client, buf, len)): the outgoing dispatcher. Mode (DAT_6fbfb418) 1 or 2 queues the
// run and returns its length. Else flag 0 with buf[0] == 0xaf (the always-uncompressed opcode) and flag 2 only get decoded;
// everything else is decoded and then compressed (Fog #10223) into a 0x40c-byte frame: n < 0xef sends frame[1..] with the
// one-byte length in frame[1] (n+1), otherwise frame[0..] with the two-byte prefix (n+2; frame[0] = 0xf0 | (n+2) >> 8). The send
// is Fog #10157 on the socket DAT_6fbfb424; a 0 from it makes the result 0. Retail _exit(-1)s on len > 0x204 and on compress 0:
// those two paths are not modelled.
extern "C" uint32_t d2net_sub_63e0(uint32_t flag, uint32_t client, uint8_t* buf, uint32_t len, uint32_t mode, uint32_t sock,
                                   void (*queue)(const uint8_t*, uint32_t), void (*decode)(const uint8_t*, uint32_t),
                                   uint32_t (*compress)(uint8_t*, uint32_t, const uint8_t*, uint32_t),
                                   uint32_t (*send)(uint32_t, uint32_t, const uint8_t*, uint32_t)) {
    if (mode == 1 || mode == 2) { queue(buf, len); return len; }
    if (flag != 0 || buf[0] != 0xaf) {
        decode(buf, len);
        if (flag != 2) {
            uint8_t frame[0x40c];
            const uint32_t n = compress(frame + 2, 0x408, buf, len);
            if (!n) return 0;
            const uint32_t m = n + 1;
            if (m < 0xf0) { frame[1] = (uint8_t)m; return send(sock, client, frame + 1, m) ? m : 0; }
            const uint32_t m2 = m + 1;
            frame[0] = (uint8_t)((m2 >> 8) | 0xf0); frame[1] = (uint8_t)m2;
            return send(sock, client, frame, m2) ? m2 : 0;
        }
    }
    return send(sock, client, buf, len) ? len : 0;
}
