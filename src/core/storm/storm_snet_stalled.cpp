// src/core/storm/storm_snet_stalled.cpp -- 12 former-STALLED 1.13c Storm.dll rows (SNET.CPP peer/message
// queues and their leaves), transcribed from build-wasm/ghidra-storm/decomp/0x<RVA>.c.
//
// Node/peer words keep retail's 32-bit layout (same convention as storm_batch_123b0.cpp); the module
// state those bodies read is held here as uint32 words named by retail address. Hidden register
// arguments (EAX/EBX/ESI/EDI/AL) are explicit parameters. Host seams, each named at its use:
//   g_snetCb      = *(DAT_6fc45834+0xc), the provider's message-release callback (null: skipped)
//   g_provider    = DAT_6fc45834, the bound SNP provider (0: no provider in the web build)
//   0x161E0       = event/worker handles are non-null tokens; no thread is created (no Win32 threads)
//   0x11F10       = MessageBoxA / the default window handle are host-seam tokens
#include <cstdint>
#include <cstring>
#include <Windows.h>

extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);
extern "C" int32_t Storm_Sub1B960(void* p, const char* file, int32_t line);
extern "C" uint16_t Storm_SubFF40(uint32_t, uint32_t, const uint8_t*);
extern "C" void Storm_Sub5440(void* table, uint32_t n);
uint32_t* Storm_Sub5040(uint32_t, int32_t param_2, uint32_t param_3, int32_t edi_src);
uint32_t Storm_Sub4EF0(uint32_t, uint32_t p2, uint32_t p3, uint32_t edi);
void Storm_Sub4390(void* a, uint32_t* obj, int32_t src);
void Storm_Sub4D60(uint8_t* node);
void Storm_Sub10E30(uint8_t* conn, const uint8_t* pkt);
uint32_t Storm_Sub10D20(const void* const* parts, const uint8_t* data, uint32_t length, uint32_t count);
void Storm_Sub10CB0(void);
int32_t* Storm_Sub12690(void);
uint32_t* Storm_Sub5170(uint32_t, int32_t param_2, uint32_t param_3, uint32_t edi);

namespace {
constexpr const char* kSnet = "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP";

uint32_t g_a18[3];            // 0x6FC45A18  user-event pool head; first node at [2] (0x6FC45A20)
uint32_t g_a3c[3];            // 0x6FC45A3C  connection-record head; the local peer at [2] (0x6FC45A44)
uint32_t g_a64[3];            // 0x6FC45A64  peer list head; first peer at [2] (0x6FC45A6C)
uint32_t g_provider;          // 0x6FC45834  bound SNP provider (0 = none)
uint32_t g_45830;             // 0x6FC45830  teardown time bias
uint32_t g_4583c;             // 0x6FC4583C  send pacing stamp
uint32_t g_41ab8;             // 0x6FC41AB8  bytes-queued counter
uint32_t g_45170;             // 0x6FC45170  peer-count threshold
uint32_t g_45344;             // 0x6FC45344  flag word (bit 1 cleared below the threshold)
uint8_t  g_41b4c = 0xFF;      // 0x6FC41B4C  0xFF = no archive/session
int32_t  g_lastError;         // 0x6FC45388
uint32_t g_4584c, g_45850, g_45854;  // event handle, worker flag, worker handle
CRITICAL_SECTION g_a24;       // 0x6FC45A24
void (*g_snetCb)(uint32_t, uint32_t, uint32_t);  // *(g_provider+0xC)
const char kEmpty[] = "";     // DAT_6fc36b65
char g_463f0[0x100];          // DAT_6fc463f0  version string buffer

uint32_t rd32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
void wr32(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }
uint16_t rd16(const uint8_t* p) { uint16_t v; std::memcpy(&v, p, 2); return v; }
void wr16(uint8_t* p, uint16_t v) { std::memcpy(p, &v, 2); }
uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
uint32_t A(const void* p) { return (uint32_t)(uintptr_t)p; }
uint32_t W(uint32_t base, uint32_t i) { return rd32(P(base) + i * 4); }
uint32_t first(uint32_t v) { return (int32_t)v > 0 ? v : 0; }   // (v & ((int)v < 1) - 1)

// The relative-link unlink repeated inline all through SNET.CPP (the 0x4C20 body).
void unlinkInline(uint8_t* n) {
    uint32_t link = rd32(n);
    if (link == 0) return;
    uint32_t tag = rd32(n + 4);
    uint8_t* prev = (int32_t)tag < 0 ? P(~tag) : P(A(n) + (tag - rd32(P(link) + 4)));
    wr32(prev, link);
    wr32(P(rd32(n)) + 4, rd32(n + 4));
    wr32(n, 0);
    wr32(n + 4, 0);
}

// Release one queued message node: provider callback (cb path) or SMemFree of its two buffers,
// then the 0x4D60 node teardown and the node itself ("__AUMESSAGE__", -2).
void freeMessage(uint8_t* n, bool cb) {
    if (cb) {
        if (g_snetCb) g_snetCb(rd32(n + 8), rd32(n + 12), rd32(n + 16));
    } else {
        uint32_t b = rd32(n + 12);
        if (rd32(n + 8)) Storm_Sub1B960(P(rd32(n + 8)), "delete", -1);
        if (b) Storm_Sub1B960(P(b), kSnet, 0x3b6);
    }
    Storm_Sub4D60(n);
    Storm_Sub1B960(n, "__AUMESSAGE__", -2);
}

// Two-byte packet checksum over [pkt+2, pkt+2+len) with the slot index as seed (shared tail of 0x11D90/0x12F40).
void stamp(uint8_t* pkt, uint32_t seed, uint32_t len) {
    uint16_t u = Storm_SubFF40(seed, len, pkt + 2);
    uint8_t b = (uint8_t)(-(int)(((u & 0xff) + (u >> 8)) % 0xff) - 1);
    pkt[0] = (uint8_t)(-1 - (int)((b + (u >> 8)) % 0xff));
    pkt[1] = b;
}

void copyClamped(uint8_t* dst, const uint32_t* src, uint32_t cap) {
    if (!src) return;
    uint32_t n = *src < cap ? *src : cap;
    std::memcpy(dst, src, n);
}
}  // namespace

// 1.13c Storm+0x5110
// Grow the 128-byte-entry table obj (EAX) towards n (ESI), then record n as the high-water mark.
void Storm_Sub5110(uint32_t* obj, uint32_t n) {
    if (obj[1] < n && obj[0] < n) {
        if (obj[3] == 0 && n >= 8) obj[3] = 8;   // 0x2C20 clamp: obj[3] = 8 when n >= 8
        Storm_Sub5440(obj, n);
    }
    obj[1] = n;
}

// 1.13c Storm+0x5170
// Allocate a "__AUUSEREVENT__" node of param_2+0x18 bytes, zero its link words, link into edi's list.
uint32_t* Storm_Sub5170(uint32_t, int32_t param_2, uint32_t param_3, uint32_t edi) {
    uint32_t* p = (uint32_t*)Storm_Sub1C580((uint32_t)param_2 + 0x18, "__AUUSEREVENT__", -2, param_3 | 8);
    if (p) { p[0] = 0; p[1] = 0; }
    if (edi != 0) Storm_Sub4390(nullptr, p, (int32_t)edi);
    return p;
}

// 1.13c Storm+0x11240
// SNET teardown: drain the user-event list (head word at 0x6FC45A20), unlink, bias time, free.
void Storm_Sub11240(void) {
    while ((int32_t)g_a18[2] > 0) {
        uint8_t* n = P(g_a18[2]);
        unlinkInline(n);
        wr32(n + 12, rd32(n + 12) + g_45830);
        // FUN_6fc25dd0('SNET', 1, node[2], &node[2]): audio-position notify, no host effect
        if (rd32(n + 16)) Storm_Sub1B960(P(rd32(n + 16)), kSnet, 0x735);
        Storm_Sub4D60(n);
        Storm_Sub1B960(n, "delete", -1);
    }
}

// 1.13c Storm+0x11320
// this (peer), param_1 (node address or 0), AL = queue index. Queue 2: relink param_1 on this+0x1c4.
// Otherwise free the head message of queue AL, then unlink and relink param_1 at this+AL*12+0x194.
void Storm_Sub11320(uint8_t* self, uint32_t param_1, uint8_t al) {
    uint8_t* q;
    if (al == 2) {
        q = param_1 ? P(param_1) : self + 0x1c4;
        unlinkInline(q);
        uint32_t head = rd32(self + 0x1c4);
        wr32(q, head);
        wr32(q + 4, rd32(P(head) + 4));
        wr32(P(head) + 4, param_1);
        wr32(self + 0x1c4, A(q));
        return;
    }
    uint32_t v = rd32(self + al * 12 + 0x198);
    if ((int32_t)v > 0) freeMessage(P(first(v)), rd32(P(first(v)) + 20) == 0);
    q = param_1 ? P(param_1) : self + al * 12 + 0x194;
    unlinkInline(q);
    uint32_t head = rd32(self + al * 12 + 0x194);
    wr32(q, head);
    wr32(q + 4, rd32(P(head) + 4));
    wr32(P(head) + 4, param_1);
    wr32(self + al * 12 + 0x194, A(q));
}

// 1.13c Storm+0x11470
// Ack-queue purge for queue param_2 of session param_1, acked up to sequence BX (hidden, explicit).
void Storm_Sub11470(uint8_t* sess, uint8_t qi, uint16_t bx) {
    bool any = false;
    uint8_t* h = sess + 0x134 + qi * 12;
    for (;;) {
        uint32_t v = rd32(h + 4);
        if ((int32_t)v < 1) break;
        uint8_t* n = P(first(v));
        uint16_t seq = rd16(P(rd32(n + 12)) + 4);
        if (bx == seq || (uint16_t)(bx - seq) >= 0x8000) break;
        any = true;
        freeMessage(n, rd32(n + 20) == 0);
    }
    if (qi == 2) wr16(sess + 0x1dc, bx);
    if (any && (int32_t)rd32(h + 4) > 0) {
        uint8_t* n = P(first(rd32(h + 4)));
        uint32_t t = GetTickCount();
        if (t - rd32(n + 0x1c) < 0x7fffffff) wr32(n + 0x1c, t);
    }
}

// 1.13c Storm+0x11D90
// Re-queue a copy of the size-byte packet src on conn: restamp seq/checksum, push it, free the copy.
int32_t Storm_Sub11D90(uint32_t size, uint8_t* conn, const uint8_t* src) {
    uint8_t* p = (uint8_t*)Storm_Sub1C580(size, kSnet, 0x559, 0);
    std::memcpy(p, src, size);
    uint16_t len = rd16(p + 2);
    wr16(p + 6, rd16(conn + 0x1de + p[8] * 2));
    stamp(p, p[8], len - 2u);
    const void* part = conn + 0x108;
    Storm_Sub10D20(&part, p, len, 1);
    wr16(conn + 0x1e6 + p[8] * 2, rd16(p + 6));
    wr32(conn + 0x1f0 + p[8] * 4, 0);
    Storm_Sub1B960(p, kSnet, 0x566);
    return 1;
}

// 1.13c Storm+0x11F10
// Normalise the SNP info blocks: in3c (EAX) -> out3c (EBX, 0x3c), p1 -> out10, p2 -> out50, p3 -> out14.
int32_t Storm_Sub11F10Full(const uint32_t* in3c, uint32_t* out3c, const uint32_t* p1, const uint32_t* p2,
                           const uint32_t* p3, uint32_t* out10, uint32_t* out50, uint32_t* out14) {
    std::memset(out3c, 0, 0x3c);
    copyClamped((uint8_t*)out3c, in3c, 0x3c);
    out3c[0] = 0x3c;
    if (!out3c[1]) out3c[1] = A(kEmpty);
    if (!out3c[2]) {
        // 0x11AF0: "<prefix> " + the info override string (ProductVersion has no wasm source)
        const char* pre = (const char*)P(out3c[1]);
        size_t n = std::strlen(pre) < 0xfe ? std::strlen(pre) : 0xfe;
        std::memcpy(g_463f0, pre, n);
        g_463f0[n] = ' ';
        g_463f0[n + 1] = 0;
        std::strncat(g_463f0, pre, sizeof g_463f0 - n - 2);
        out3c[2] = A(g_463f0);
    }
    if (!out3c[0xb]) out3c[0xb] = A(kEmpty);
    std::memset(out10, 0, 0x10);
    copyClamped((uint8_t*)out10, p1, 0x10);
    out10[0] = 0x10;
    if (!out10[1]) out10[1] = A(kEmpty);
    if (!out10[2]) out10[2] = A(kEmpty);
    std::memset(out50, 0, 0x50);
    copyClamped((uint8_t*)out50, p2, 0x50);
    out50[0] = 0x50;
    // host seams: DAT_6fc453d8 (default HWND) is 0 on the web; MessageBoxA is the host alert token
    if (!out50[8]) out50[8] = 1;
    std::memset(out14, 0, 0x14);
    copyClamped((uint8_t*)out14, p3, 0x14);
    out14[0] = 0x14;
    return 1;
}

// 1.13c Storm+0x11F10
// Callers (storm_batch_123b0.cpp) do not carry the hidden EAX/EBX pair: no 0x3c input, scratch output.
int32_t Storm_Sub11F10(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d, uint32_t* e, uint32_t* f) {
    uint32_t scratch[15];
    return Storm_Sub11F10Full(nullptr, scratch, a, b, c, d, e, f);
}

// 1.13c Storm+0x12690
// The local peer record: created on first use from the 0x5040 allocator, with its counters reset.
int32_t* Storm_Sub12690(void) {
    uint32_t p = g_a3c[2];
    if ((int32_t)p < 1) {
        uint32_t* r = Storm_Sub5040(0, 0, 0, (int32_t)A(g_a3c));
        p = A(r);
        for (int i = 0x42; i <= 0x45; i++) wr32(P(p) + i * 4, 0);
        P(p)[0x212] = 0xff;
        P(p)[0x213] = 0xff;
        wr32(P(p) + 0x47 * 4, GetTickCount());
    }
    return (int32_t*)P(p);
}

// 1.13c Storm+0x12740
// Release acked/stale messages for peer param_1 (0 = every peer) and the local peer.
void Storm_Sub12740(void* arg) {
    uint32_t param_1 = A(arg);
    uint8_t* self = (uint8_t*)Storm_Sub12690();
    if (!self) return;
    int16_t s1 = (int16_t)rd32(self + 0x76 * 4);
    uint32_t p10 = g_a64[2];
    int16_t s5 = s1;
    if ((int32_t)p10 > 0) {
        do {
            if ((uint16_t)(s5 - (int16_t)W(p10, 0x77)) < 0x7fff) s5 = (int16_t)W(p10, 0x77);
            p10 = W(p10, 1);
        } while ((int32_t)p10 > 0);
    }
    p10 = g_a64[2];
    for (int pass = 0; pass < 2; pass++) {
        uint32_t p6 = A(self), p8 = p10;
        bool joined = pass != 0;
        for (;;) {
            if (!joined) { p6 = p8; if ((int32_t)p8 <= 0) break; }
            joined = false;
            if (p6 == 0) break;
            if (param_1 == 0 || p6 == param_1) {
                uint8_t* peer = P(p6);
                for (;;) {
                    uint32_t v = rd32(peer + 0x60 * 4);
                    if ((int32_t)v < 1) break;
                    int16_t seq = (int16_t)rd16(P(rd32(P(first(v)) + 12)) + 4);
                    if (s1 == seq || (uint16_t)(s1 - seq) >= 0x8000) break;
                    p10 = rd32(peer + 0x60 * 4);
                    uint8_t* q8 = (int32_t)p10 < 1 ? (p10 = 0, peer + 0x5f * 4) : P(p10);
                    unlinkInline(q8);
                    q8 = p10 ? P(p10) : peer + 0x71 * 4;
                    unlinkInline(q8);
                    uint32_t head = rd32(peer + 0x71 * 4);
                    wr32(q8, head);
                    wr32(q8 + 4, rd32(P(head) + 4));
                    wr32(P(head) + 4, p10);
                    wr32(peer + 0x71 * 4, A(q8));
                }
                for (;;) {
                    uint32_t v = rd32(peer + 0x72 * 4);
                    p10 = g_a64[2];
                    if ((int32_t)v < 1) break;
                    int16_t seq = (int16_t)rd16(P(rd32(P(first(v)) + 12)) + 4);
                    if (s5 == seq || (uint16_t)(s5 - seq) >= 0x8000) break;
                    freeMessage(P(first(v)), pass == 0);
                }
            }
            p8 = W(p6, 1);
        }
    }
}

// 1.13c Storm+0x12F40
// Build and queue one SNET packet (type param_2, queue param_1, payload param_3[param_4]) on peer ebx.
// Returns the queue node, or 0 (buffers freed) when the queue allocation fails.
uint32_t* Storm_Sub12F40Peer(uint8_t* ebx, uint8_t param_1, char param_2, const uint32_t* param_3,
                             uint32_t param_4) {
    uint8_t* self = (uint8_t*)Storm_Sub12690();
    if (!self) return nullptr;
    uint32_t total = param_4 + 0xc;
    uint32_t* hdr = (uint32_t*)Storm_Sub1C580(0x10, kSnet, 0x39b, 0);
    uint32_t sz = (total - (total & 3)) + 4;
    if (sz < 4) sz = 4;
    uint8_t* pkt = (uint8_t*)Storm_Sub1C580(sz, kSnet, 0x39d, 0);
    for (int i = 0; i < 4; i++) hdr[i] = rd32(ebx + (0x42 + i) * 4);
    wr16(pkt + 2, (uint16_t)total);
    wr16(pkt + 6, rd16(ebx + param_1 * 2 + 0x1de));
    pkt[9] = (uint8_t)param_2;
    pkt[8] = param_1;
    pkt[10] = self[0x212];
    pkt[11] = 0;
    if (param_1 == 0 && param_2 == 1) {
        wr16(pkt + 4, 0);
    } else {
        uint16_t s = rd16(ebx + param_1 * 2 + 0x1cc);
        wr16(pkt + 4, s);
        wr16(ebx + param_1 * 2 + 0x1cc, (uint16_t)(s + 1));
    }
    if (param_3 && param_4) std::memcpy(pkt + 12, param_3, param_4);
    stamp(pkt, param_1, (uint16_t)total - 2u);
    uint32_t node = 0;
    bool skip = false;
    uint32_t headOff;
    if (ebx == self) headOff = param_1 * 3u + 0x58;
    else if (param_1 == 3) skip = true;
    else headOff = param_1 * 3u + 0x4c;
    if (!skip) {
        node = Storm_Sub4EF0(0, 0, 0, A(ebx + headOff * 4));
        if (node) {
            wr32(P(node) + 8, A(hdr));
            wr32(P(node) + 12, A(pkt));
            wr32(P(node) + 16, param_4 + 0xc);
            wr32(P(node) + 20, 1);
            wr32(P(node) + 24, GetTickCount());
        }
    }
    if (ebx != self) {
        Storm_Sub10E30(ebx, pkt);
        g_41ab8 += param_4;
    }
    if (node) { wr32(P(node) + 28, g_4583c); return (uint32_t*)P(node); }
    if (hdr) Storm_Sub1B960(hdr, "delete", -1);
    Storm_Sub1B960(pkt, kSnet, 0x3b6);
    return nullptr;
}

// 1.13c Storm+0x12F40
// Four-argument form used by storm_batch_44.cpp (EBX not carried there): addressed to the local peer.
int32_t Storm_Sub12F40(int32_t q, char type, uint32_t* data, uint32_t len) {
    return (int32_t)(uintptr_t)Storm_Sub12F40Peer((uint8_t*)Storm_Sub12690(), (uint8_t)q, type, data, len);
}

// 1.13c Storm+0x14F20
// Walk the peer list: for each peer flagged 8 with a live slot and a stale ack, queue a type-3 event,
// retire the slot, and broadcast the lowest-slot announcement (type 0x0D) once.
void Storm_Sub14F20(void) {
    uint32_t peer = first(g_a64[2]);
    uint8_t* local = (uint8_t*)Storm_Sub12690();
    if (!local) return;
    while (peer != 0) {
        uint8_t* pp = P(peer);
        uint8_t slot = pp[0x212];
        if ((pp[0x118] & 8) && slot != 0xff && (uint16_t)(rd16(pp + 0x1d8) - rd16(pp + 0x210)) < 0x8000) {
            uint32_t* ev = Storm_Sub5170(0, 0, 0, A(g_a18));
            ev[2] = 3;
            ev[3] = slot;
            uint32_t* buf = (uint32_t*)Storm_Sub1C580(4, kSnet, 0x8f3, 0);
            ev[4] = A(buf);
            *buf = rd32(pp + 0x20c);
            ev[5] = 4;
            uint8_t b = g_41b4c;
            bool live = b != 0xff;
            wr32(pp + 0x118, 0);
            pp[0x213] = pp[0x212];
            pp[0x212] = 0xff;
            if (live && rd32(local + 0x80 * 4) == 0) {
                uint32_t lo = 0xffffffffu;
                for (uint32_t q = g_a64[2]; (int32_t)q > 0; q = W(q, 1)) {
                    uint8_t s = P(q)[0x212];
                    if (s != 0xff) { if (W(q, 0x80) == 0) { if (s < lo) lo = s; } else lo = 0; }
                }
                if (b < lo) {
                    wr32(local + 0x80 * 4, 1);
                    for (uint32_t q = g_a64[2]; (int32_t)q > 0; q = W(q, 1)) {
                        if (P(q)[0x212] != 0xff) {
                            uint32_t arg = b;
                            Storm_Sub12F40Peer(P(q), 0, '\r', &arg, 4);
                            b = g_41b4c;
                        }
                    }
                }
            }
            uint8_t* l2 = (uint8_t*)Storm_Sub12690();
            if (l2 && rd32(l2 + 0x80 * 4) != 0) {
                EnterCriticalSection(&g_a24);
                uint32_t count = 0;
                if (g_provider == 0) { g_lastError = 0x4b4; SetLastError(0x4b4); }
                else if (g_41b4c == 0xff) { g_lastError = (int32_t)0x85100070; SetLastError(0x85100070); }
                else {
                    count = 1;
                    for (uint32_t q = g_a64[2]; (int32_t)q > 0; q = W(q, 1)) {
                        if (P(q)[0x212] != 0xff) count++;
                    }
                }
                LeaveCriticalSection(&g_a24);
                if (count < g_45170) g_45344 &= 0xfffffffdu;
                Storm_Sub10CB0();
            }
        }
        uint32_t nx = rd32(pp + 4);
        if ((int32_t)nx <= 0) break;
        peer = nx;
    }
}

// 1.13c Storm+0x161E0
// Lazily create the worker event and thread. Host seam: no Win32 threads on wasm, so the two handles
// are non-null tokens (the worker's work is done inline by the engine); *out gets the event token.
int32_t Storm_Sub161E0Out(uint32_t* out) {
    if (g_4584c == 0) g_4584c = 1;
    if (g_45854 == 0) { g_45850 = 0; g_45854 = 1; }
    if (out) *out = g_4584c;
    return (g_4584c != 0 && g_45854 != 0) ? 1 : 0;
}

// 1.13c Storm+0x161E0
int32_t Storm_Sub161E0(void) { return Storm_Sub161E0Out(nullptr); }
