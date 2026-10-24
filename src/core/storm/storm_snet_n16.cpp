// src/core/storm/storm_snet_n16.cpp -- twelve 1.13c Storm.dll SNet rows (0x5110 0x5170 0x11240 0x11320
// 0x11470 0x11D90 0x11F10 0x12690 0x12740 0x12F40 0x14F20 0x161E0) from the cached decompiles
// (build-wasm/ghidra-storm/decomp/0x<RVA>.c), register arguments checked against the retail disassembly.
// Records keep retail's 32-bit field layout and 32-bit link words (addresses truncated, as the other
// Storm batch files do). Register arguments are explicit parameters. SEH frames are omitted.
//
// List model (0x4390/0x4C20): an object is {pad, link, tag}; its anchor is {link, tag}. Empty = link
// self, tag = ~anchor (as 0x51F0). New nodes go in at the head with EDI=2, so the tail (anchor.tag
// while > 0) is the oldest and node.tag points toward the head. The three module lists are lazily set
// empty on first touch; the provider table (0x6FC45834) is never initialised in ours, so its callback
// slot is guarded.
#include <cstdint>
#include <cstring>
#include <Windows.h>

extern "C" {
void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);
int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
uint32_t* Storm_Sub5040(uint32_t, int32_t extra, uint32_t flags, int32_t edi);
uint16_t Storm_SubFF40(uint32_t, uint32_t bytes, const uint8_t* data);
uint32_t Storm_Sub10D20(const void* const* parts, const uint8_t* data, uint32_t length, uint32_t count);
void Storm_Sub10CB0(void);
void Storm_Sub10E30(uint8_t* conn, const uint8_t* pkt);
void Storm_Sub5440(void* tbl, uint32_t n);
}
void Storm_Sub4C20(uint8_t* node);
void Storm_Sub4D60(uint8_t* node);
void Storm_Sub4390(uint32_t* eax, uint8_t* this_, uint8_t* node_arg, int32_t mode);
#include "storm_batch_25220.h"
namespace storm25220 { void versionString(State&, char*, const char*, uint32_t, const char*); }

namespace {
const char kSnet[] = "..\\3rdParty\\STORM\\SOURCE\\SNET.CPP";
const char kMsg[] = ".?AUMESSAGE@@";
const char kEvt[] = ".?AUUSEREVENT@@";

uint32_t A(const void* p) { return (uint32_t)(uintptr_t)p; }
uint8_t* P(uint32_t a, uintptr_t off = 0) { return (uint8_t*)(uintptr_t)a + off; }
uint32_t& U32(uint32_t a, uintptr_t off = 0) { return *(uint32_t*)P(a, off); }
uint16_t& U16(uint32_t a, uintptr_t off) { return *(uint16_t*)P(a, off); }
uint8_t& U8(uint32_t a, uintptr_t off) { return *P(a, off); }

struct List { uint32_t pad, link, tag; };            // retail object; list anchor = &link
List g_events;                                       // 0x6FC45A18 (link 5A1C, tag 5A20)
List g_self;                                         // 0x6FC45A3C (tail = tag = 0x6FC45A44, own record)
List g_conns;                                        // connection list (tag = 0x6FC45A6C)
void ready(List& l) { if (!l.link) { l.link = A(&l.link); l.tag = ~A(&l.link); } }
void ensure() { ready(g_events); ready(g_self); ready(g_conns); }
uint32_t tail(List& l) { ensure(); return (int32_t)l.tag > 0 ? l.tag : 0; }

uintptr_t* g_prov;                                   // 0x6FC45834 provider table; +0xc = free callback
uint32_t g_base;                                     // 0x6FC45830
uint32_t g_stamp;                                    // 0x6FC4583C
uint32_t g_sent;                                     // 0x6FC41AB8 bytes sent to peers
uint8_t g_provId = 0xff;                             // 0x6FC41B4C
uint32_t g_minPlayers, g_flags, g_lastError;         // 0x6FC45170 0x6FC45344 0x6FC45388
CRITICAL_SECTION g_lock;                             // 0x6FC45A24
char g_version[0x100];                               // 0x6FC463F0
HANDLE g_event, g_thread; uint32_t g_threadFlag;     // 0x6FC4584C 0x6FC45854 0x6FC45850

uint32_t node(uint32_t cap, const char* tag, int32_t line) {
    return A(Storm_Sub1C580(cap, tag, line, 0));
}
// 0x4390 with EAX = list object, EDI = mode
void link(uint32_t obj, uint32_t n, int32_t mode) {
    Storm_Sub4390((uint32_t*)P(obj), nullptr, P(n), mode);
}
// the message free shared by 0x11320 0x11470 0x12740
void drop(uint32_t m, bool callback) {
    if (callback) {
        if (g_prov) ((void (*)(uint32_t, uint32_t, uint32_t))g_prov[3])(U32(m, 8), U32(m, 12), U32(m, 16));
    } else {
        uint32_t data = U32(m, 12);
        if (U32(m, 8)) Storm_Sub1B960(P(U32(m, 8)), "delete", -1);
        if (data) Storm_Sub1B960(P(data), kSnet, 0x3b6);
    }
    Storm_Sub4D60(P(m));
    Storm_Sub1B960(P(m), kMsg, -2);
}
// the two-byte packet check (0x11D90 / 0x12F40): FF40 over bytes 2.. then folded into bytes 0..1
void seal(uint32_t pkt) {
    uint16_t v = Storm_SubFF40(U8(pkt, 8), U16(pkt, 2) - 2u, P(pkt, 2));
    uint8_t lo = v & 0xff, hi = v >> 8;
    uint8_t b = (uint8_t)(-(int)((lo + hi) % 0xff) - 1);
    U8(pkt, 0) = (uint8_t)(-1 - (int)((b + hi) % 0xff));
    U8(pkt, 1) = b;
}
}

// 1.13c Storm+0x05110
// Grow a 128-byte-entry table (EAX=EBX=table {cap, count, data, step}, ESI=n) to hold n entries, the
// capacity rounded up to the step (step comes from 0x2C20 when unset), then record n as the count.
static void Storm_Sub5110(uint32_t* tbl, uint32_t n) {
    if (tbl[1] < n && tbl[0] < n) {
        uint32_t step = tbl[3];
        if (step == 0) {                       // 0x2C20(n): largest power of two <= n below 8, else 8
            if (n < 8) { step = n; while (step & (step - 1)) step &= step - 1; if (!step) step = 1; }
            else { tbl[3] = 8; step = 8; }
        }
        uint32_t r = n % step;
        Storm_Sub5440(tbl, r ? n + step - r : n);
    }
    tbl[1] = n;
}

// 1.13c Storm+0x05170
// Allocate an event node (0x18 + extra bytes, "(AUUSEREVENT)"), clear its link words and, when
// edi != 0, link it into the list object `anchor` (EAX) with mode edi.
uint32_t Storm_Sub5170(uint32_t anchor, int32_t extra, uint32_t flags, int32_t edi) {
    uint32_t n = A(Storm_Sub1C580((uint32_t)extra + 0x18, kEvt, -2, flags | 8));
    if (n) { U32(n, 0) = 0; U32(n, 4) = 0; }
    if (edi) link(anchor, n, edi);
    return n;
}

// queue one user event {type, id, optional data copy}; the shape of 0x11C20 / 0x124B0's call sites
void Storm_Sub5170_queueEvent(uint32_t type, int32_t id, void* copy, uint32_t len) {
    ensure();
    uint32_t e = Storm_Sub5170(A(&g_events), 0, 0, 2);
    U32(e, 8) = type; U32(e, 12) = (uint32_t)id;
    if (copy && len) { U32(e, 16) = A(copy); U32(e, 20) = len; }
}

// 1.13c Storm+0x11240
// Drain the user-event list from its tail: dispatch each node as an 'SNET' SEVT command (no handler
// table exists in ours, so the dispatch finds nothing), free its data and the node.
static void Storm_Sub11240(void) {
    ensure();
    for (uint32_t n; (n = tail(g_events)) != 0;) {
        Storm_Sub4C20(P(n));
        U32(n, 12) += g_base;
        // 0x35DD0(0x534E4554, 1, n[2], &n[2]): ponytail: SEVT handlers are never registered here
        if (U32(n, 16)) Storm_Sub1B960(P(U32(n, 16)), kSnet, 0x735);
        Storm_Sub4D60(P(n));
        Storm_Sub1B960(P(n), "delete", -1);
        if (tail(g_events) == n) break;    // list not advancing (retail would spin)
    }
}

// 1.13c Storm+0x11320
// conn (ECX), slot (AL), msg: slot 2 just queues msg on the connection's list at +0x1C4; any other slot
// first drops the oldest message of its +0x198 list, then queues msg on the +0x194 list.
void Storm_Sub11320(uint32_t conn, uint8_t slot, uint32_t msg) {
    if (slot == 2) { link(conn + 0x1c0, msg, 2); return; }
    uint32_t t = U32(conn, slot * 12u + 0x198);
    if ((int32_t)t > 0) drop(t, U32(t, 20) == 0);
    link(conn + slot * 12u + 0x190, msg, 2);
}

// 1.13c Storm+0x11470
// conn (param_1), slot (param_2), seq (BX): drop messages from the tail of the +0x134 list while their
// sequence is older than seq; slot 2 remembers seq at +0x1DC; refresh the new tail's send time.
void Storm_Sub11470(uint32_t conn, uint8_t slot, uint16_t seq) {
    bool dropped = false;
    uint32_t anchor = conn + 0x134 + slot * 12u;
    for (;;) {
        uint32_t t = U32(anchor, 4);
        if ((int32_t)t <= 0) break;
        uint16_t s = U16(U32(t, 12), 4);
        if (seq == s || (uint16_t)(seq - s) >= 0x8000) break;
        dropped = true;
        drop(t, U32(t, 20) == 0);
    }
    if (slot == 2) U16(conn, 0x1dc) = seq;
    uint32_t t = U32(anchor, 4);
    if (dropped && (int32_t)t > 0) {
        DWORD now = GetTickCount();
        if (now - U32(t, 0x1c) < 0x7fffffff) U32(t, 0x1c) = now;
    }
}

// 1.13c Storm+0x11D90
// conn (param_1), pkt (param_2), size (EAX): copy the packet, stamp its sequence word and check bytes,
// stage it on the connection's 16-byte send record (0x10D20), note the slot's last sequence; returns 1.
uint32_t Storm_Sub11D90(uint32_t conn, const uint8_t* pkt, uint32_t size) {
    uint32_t p = A(Storm_Sub1C580(size, kSnet, 0x559, 0));
    std::memcpy(P(p), pkt, size);
    uint16_t len = U16(p, 2);
    uint8_t slot = U8(p, 8);
    U16(p, 6) = U16(conn, 0x1de + slot * 2u);
    seal(p);
    const void* part = P(conn, 0x108);
    Storm_Sub10D20(&part, P(p), len, 1);
    U16(conn, 0x1e6 + slot * 2u) = U16(p, 6);
    U32(conn, 0x1f0 + slot * 4u) = 0;
    Storm_Sub1B960(P(p), kSnet, 0x566);
    return 1;
}

// 1.13c Storm+0x11F10
// Fill the provider-info blocks: each destination is zeroed, the caller's block copied in (bounded by
// its own size word and the retail size 0x3C/0x10/0x50/0x14), the size word set, and defaults filled.
// srcA/dstA are EAX/EBX; the version string for a missing +8 uses 0x11AF0 with no override.
uint32_t Storm_Sub11F10(const uint32_t* srcA, uint32_t* dstA, const uint32_t* srcB, const uint32_t* srcC,
                        const uint32_t* srcD, uint32_t* dstB, uint32_t* dstC, uint32_t* dstD) {
    static const char kEmpty[] = "";                     // retail 0x6FC36B65
    auto fill = [](const uint32_t* s, void* d, uint32_t maxBytes, uint32_t total) {
        std::memset(d, 0, total);
        if (s) std::memcpy(d, s, *s < maxBytes ? *s : maxBytes);
    };
    fill(srcA, dstA, 0x3c, 0x3c);
    dstA[0] = 0x3c;
    if (!dstA[1]) dstA[1] = A(kEmpty);
    if (!dstA[2]) {
        static storm25220::State st;
        st.setLastError = [](uint32_t e) { g_lastError = e; };
        storm25220::versionString(st, g_version, (const char*)(uintptr_t)dstA[1], 0x100, nullptr);
        dstA[2] = A(g_version);
    }
    if (!dstA[0xb]) dstA[0xb] = A(kEmpty);
    fill(srcB, dstB, 0x10, 0x10);
    dstB[0] = 0x10;
    if (!dstB[1]) dstB[1] = A(kEmpty);
    if (!dstB[2]) dstB[2] = A(kEmpty);
    fill(srcC, dstC, 0x50, 0x50);
    dstC[0] = 0x50;
    // retail defaults dstC[2] to the host window (DAT_6fc453d8) and dstC[8] to &MessageBoxA; both stay 0 here
    fill(srcD, dstD, 0x14, 0x14);
    dstD[0] = 0x14;
    return 1;
}

// bridge for the existing six-argument callers (storm_batch_123b0.cpp): EAX absent, EBX a scratch block
static int32_t Storm_Sub11F10(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d, uint32_t* e, uint32_t* f) {
    uint32_t scratch[15];
    return (int32_t)Storm_Sub11F10(nullptr, scratch, a, b, c, d, e, f);
}

// 1.13c Storm+0x12690
// The local player's own connection record: the tail of the 0x6FC45A3C list, created (0x214 bytes,
// mark bytes +0x212/+0x213 = 0xFF, start tick at +0x11C) when the list is empty.
uint32_t Storm_Sub12690(void) {
    uint32_t c = tail(g_self);
    if (!c) {
        c = A(Storm_Sub5040(A(&g_self), 0, 0, 0));
        if (!c) return 0;
        link(A(&g_self), c, 2);
        U32(c, 0x108) = U32(c, 0x10c) = U32(c, 0x110) = U32(c, 0x114) = 0;
        U8(c, 0x212) = 0xff;
        U8(c, 0x213) = 0xff;
        U32(c, 0x11c) = GetTickCount();
    }
    return c;
}

// 1.13c Storm+0x12740
// Retire acknowledged messages (all connections, or only `only`): move sent messages older than our
// sequence from the +0x180 list to the +0x1C4 list, then free those older than the lowest peer ack.
void Storm_Sub12740(uint32_t only) {
    uint32_t cur = Storm_Sub12690();
    if (!cur) return;
    int16_t mine = (int16_t)U16(cur, 0x1d8), lowest = mine;
    for (uint32_t c = tail(g_conns); (int32_t)c > 0; c = U32(c, 4))
        if ((uint16_t)(lowest - U16(c, 0x1dc)) < 0x7fff) lowest = (int16_t)U16(c, 0x1dc);
    for (int pass = 0; pass < 2; pass++) {
        uint32_t c = pass ? cur : tail(g_conns);
        for (bool first = pass != 0; first || (int32_t)c > 0; first = false, c = U32(c, 4)) {
            if (!c) break;
            if (only && c != only) continue;
            for (;;) {
                uint32_t t = U32(c, 0x180);
                if ((int32_t)t <= 0) break;
                uint16_t s = U16(U32(t, 12), 4);
                if ((uint16_t)mine == s || (uint16_t)((uint16_t)mine - s) >= 0x8000) break;
                Storm_Sub4C20(P(t));
                link(c + 0x1c0, t, 2);
            }
            for (;;) {
                uint32_t t = U32(c, 0x1c8);
                if ((int32_t)t <= 0) break;
                uint16_t s = U16(U32(t, 12), 4);
                if ((uint16_t)lowest == s || (uint16_t)((uint16_t)lowest - s) >= 0x8000) break;
                drop(t, pass == 0);
            }
        }
    }
}

// 1.13c Storm+0x12F40
// Queue a packet {slot, type, len bytes of data} for connection conn (EBX): build the 16-byte send record
// and the sealed packet; the local record keeps it on its slot list, a peer gets it staged now (0x10E30)
// and byte-counted. Returns the queued message node or 0.
uint32_t Storm_Sub12F40(uint32_t conn, uint8_t slot, char type, const void* data, uint32_t len) {
    uint32_t cur = Storm_Sub12690();
    if (!cur) return 0;
    uint32_t total = len + 0xc;
    uint32_t rec = node(0x10, kSnet, 0x39b);
    uint32_t sz = (total - (total & 3)) + 4;
    uint32_t pkt = node(sz < 4 ? 4 : sz, kSnet, 0x39d);
    for (int i = 0; i < 4; i++) U32(rec, i * 4) = U32(conn, 0x108 + i * 4);
    U16(pkt, 2) = (uint16_t)total;
    U16(pkt, 6) = U16(conn, 0x1de + slot * 2u);
    U8(pkt, 9) = (uint8_t)type;
    U8(pkt, 8) = slot;
    U8(pkt, 10) = U8(cur, 0x212);
    U8(pkt, 0xb) = 0;
    if (slot == 0 && type == 1) U16(pkt, 4) = 0;
    else { U16(pkt, 4) = U16(conn, 0x1cc + slot * 2u); U16(conn, 0x1cc + slot * 2u)++; }
    if (data && len) std::memcpy(P(pkt, 0xc), data, len);
    seal(pkt);
    uint32_t m = 0;
    if (conn == cur || slot != 3) {
        m = A(Storm_Sub1C580(0x20, kMsg, -2, 8));
        if (m) {
            link(conn + (conn == cur ? slot * 12u + 0x160 : slot * 12u + 0x130), m, 2);
            U32(m, 8) = rec; U32(m, 12) = pkt; U32(m, 16) = len + 0xc; U32(m, 20) = 1;
            U32(m, 24) = GetTickCount();
        }
    }
    if (conn != cur) { Storm_Sub10E30(P(conn), P(pkt)); g_sent += len; }
    if (m) { U32(m, 28) = g_stamp; return m; }
    if (rec) Storm_Sub1B960(P(rec), "delete", -1);
    Storm_Sub1B960(P(pkt), kSnet, 0x3b6);
    return 0;
}

// 1.13c Storm+0x14F20
// Reap connections flagged as leaving (+0x118 bit 3, sequence caught up): queue a type-3 user event with
// their id, mark the slot free, and when we are the lowest remaining id announce the drop (0x12F40 type
// 0x0D) and refresh the provider player count.
static void Storm_Sub14F20(void) {
    uint32_t n = tail(g_conns);
    uint32_t cur = Storm_Sub12690();
    if (!cur) return;
    do {
        if (!n) return;
        uint8_t id = U8(n, 0x212);
        if ((U32(n, 0x118) & 8) && id != 0xff && (uint16_t)(U16(n, 0x1d8) - U16(n, 0x210)) < 0x8000) {
            uint32_t e = Storm_Sub5170(A(&g_events), 0, 0, 2);
            U32(e, 8) = 3; U32(e, 12) = id;
            uint32_t d = A(Storm_Sub1C580(4, kSnet, 0x8f3, 0));
            U32(e, 16) = d; U32(d) = U32(n, 0x20c); U32(e, 20) = 4;
            uint8_t prov = g_provId;
            U32(n, 0x118) = 0;
            U8(n, 0x213) = U8(n, 0x212);
            U8(n, 0x212) = 0xff;
            if (prov != 0xff && U32(cur, 0x200) == 0) {
                uint32_t lo = 0xffffffff;
                for (uint32_t c = tail(g_conns); (int32_t)c > 0 && c; c = U32(c, 4))
                    if (U8(c, 0x212) != 0xff) { if (U32(c, 0x200) == 0) { if (U8(c, 0x212) < lo) lo = U8(c, 0x212); } else lo = 0; }
                if (prov < lo) {
                    U32(cur, 0x200) = 1;
                    for (uint32_t c = tail(g_conns); (int32_t)c > 0 && c; c = U32(c, 4))
                        if (U8(c, 0x212) != 0xff) { uint32_t v = g_provId; Storm_Sub12F40(c, 0, 13, &v, 4); }
                }
            }
            uint32_t cur2 = Storm_Sub12690();
            if (cur2 && U32(cur2, 0x200)) {
                EnterCriticalSection(&g_lock);
                uint32_t count = 0;
                if (!g_prov) g_lastError = 0x4b4;
                else if (g_provId == 0xff) g_lastError = 0x85100070;
                else {
                    count = 1;
                    for (uint32_t c = tail(g_conns); (int32_t)c > 0 && c; c = U32(c, 4))
                        if (U8(c, 0x212) != 0xff) count++;
                }
                LeaveCriticalSection(&g_lock);
                if (count < g_minPlayers) g_flags &= ~2u;
                Storm_Sub10CB0();
            }
        }
        n = U32(n, 4);
    } while ((int32_t)n > 0);
}

// 1.13c Storm+0x161E0
// Ensure the SNet wake event and its worker thread exist, hand the event out through *outEvent (ESI), and
// return 1 when both are live. ponytail: wasm has no worker thread (the retail routine at 0x6FC06150 is
// the provider receive pump, which 0x124B0 already runs inline), so the thread handle is a live token.
uint32_t Storm_Sub161E0(uint32_t* outEvent) {
    if (!g_event) g_event = CreateEventA ? CreateEventA(nullptr, 0, 0, nullptr) : (HANDLE)(uintptr_t)1;
    if (!g_thread) { g_threadFlag = 0; g_thread = (HANDLE)(uintptr_t)1; }
    if (outEvent) *outEvent = A(g_event);
    return g_event && g_thread;
}

static int32_t Storm_Sub161E0(void) { return (int32_t)Storm_Sub161E0(nullptr); }   // bridge for storm_batch_123b0.cpp
