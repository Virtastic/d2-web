// 1.13c D2Client+0xC0D30 (0x610 B, stdcall, one stack word = the hovered unit P, ret 4): the hover text above the unit
// under the cursor.
// Nothing in automap mode 3 (0x11C414) or without P; a tile unit (type 5) further than 100 from the player (D2Common 10522
// thunk 0xC190 (P, [0x11BBFC])) is skipped. The anchor (0xDBC54, 0xDBC58) comes from the D2Gfx view (10013 thunk 0xD07E set:
// 11080/11168 world position, 10012 visibility, 10023 screen projection with two out-pointers, shifted by a quarter of 0xDBC48
// in automap modes 1/2) or else from the 10651/11142 screen position less 0x119960-0x11C418 / 0x11995C-8.
// Then by type: player -> 0xC0450 (ECX = P, EAX = &colour-word, stack &x, buffer, 0x80) and D2Win 10070; monster -> the
// 0x1A1/0x1A2 shrine-text path (0x1230 row, 10003(word +6), 10971), the flag-bit-9 path (0xC07C0), the 0x164 path (0xC0390:
// ECX = P, EDI = &mode, stack buffer, &colour) or 0xC0850(P); object -> name / type-row text through D2Win 10085; item ->
// 0xBFEC0 unless 0xFADB4; tile -> 10093/10014/10633 and the tile text (strncpy 0xC7) through 10085.
// Every import and callee goes through e.H with the registers and stack words retail passes; the Unicode ctor runs real
// in retail and is not a call here; the CRT sprintf (0x3146) is done inline.
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
constexpr uint32_t kStr = 0xCEACC, kStrcpy = 0xCEAB8, kStrcat = 0xCEABC, kW2U = 0xCEAC0, kStrncpy = 0xCEAA8;
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
uint32_t str(const cl::Env& e, uint32_t ecx) { return e.H(kStr, 0, 0, 0, 0, 0, 0, ecx); }
void cpy(const cl::Env& e, uint32_t d, uint32_t s) { e.H(kStrcpy, 0, 0, 0, 0, 0, 0, d, s); }
void cat(const cl::Env& e, uint32_t d, uint32_t s) { e.H(kStrcat, 0, 0, 0, 0, 0, 0, d, s); }
void clearWords(const cl::Env& e) {
    e.W(G(0x11CA30), e.R(G(0x11CA30)) & 0xFFFF0000u);
    e.W(G(0x11C578), e.R(G(0x11C578)) & 0xFFFF0000u);
    e.W(G(0x11C780), e.R(G(0x11C780)) & 0xFFFF0000u);
}
}  // namespace
// 1.13c D2Client+0xC0D30
extern "C" void d2c_c0d30(uint32_t P, const cl::Env& e) {
    volatile int32_t L10 = 0, L14 = 0;
    volatile uint16_t b18[0x80];
    volatile char s118[0x80];
    volatile uint16_t b198[0x80];
    volatile uint16_t b298[0xC8];
    volatile uint16_t b428[0x100];
    if (e.R(G(0x11C414)) == 3 || P == 0) return;
    if (e.R(P) == 5 && (int32_t)e.H(0xC190, P, e.R(G(0x11BBFC))) > 100) return;
    uint32_t ecxv = 0;   // ECX at the switch (the only path that reaches a 16-bit ECX load reads its upper half)
    if (e.H(0xD07E)) {
        const uint32_t wx = e.H(0xC1AE, P), wy = e.H(0xC232, P);
        if (e.H(0xD09C, wx, wy) == 0) return;
        e.H(0xD084, wx, wy, 0, A(&L10), A(&L14));
        const int32_t v = (int32_t)e.R(G(0xDBC48)), q = (v + (int32_t)((uint32_t)(v >> 31) & 3u)) >> 2;
        const uint32_t m = e.R(G(0x11C414));
        if (m == 1) L10 = L10 - q;
        else if (m == 2) L10 = L10 + q;
        ecxv = (uint32_t)L14 + 0x10;
        e.W(G(0xDBC54), (uint32_t)L10);
        e.W(G(0xDBC58), ecxv);
    } else {
        const uint32_t ox = e.R(G(0x119960)) - e.R(G(0x11C418));
        e.W(G(0xDBC54), e.H(0xC1D2, P) - ox);
        const uint32_t oy = e.R(G(0x11995C)) - 8;
        e.W(G(0xDBC58), e.H(0xC1F6, P) - oy);
    }
    switch (e.R(P)) {
    case 0: {
        e.H(0xC0450, A(&L10), A(b18), 0x80, 0, 0, A(&L14), P);
        e.H(0xD360, e.R(G(0xDBC58)) - 0x48, (uint32_t)L10, 1, (uint32_t)L14, 0, 0, A(b18), e.R(G(0xDBC54)));
        return;
    }
    case 1: {
        const uint32_t cls = e.R(P + 4);
        if (cls == 0x1A1 || cls == 0x1A2) {
            const uint32_t g = e.H(0x1230, 0, 0, 0, 0, 0, cls);
            if (g == 0) { clearWords(e); return; }
            const uint32_t s = str(e, (ecxv & 0xFFFF0000u) | (e.R(g + 6) & 0xFFFF));
            e.H(0xC22C, 0, 0, 0, 0, 0, 0, P, s);
        } else if (((e.R(P + 0xC4) >> 9) & 1) == 0) {
            if (cls == 0x164) {
                L10 = 3;
                L14 = 0;
                e.Ed(A(&L10));
                const uint32_t ok = e.H(0xC0390, A(b18), A(&L14), 0, 0, 0, 0, P);
                e.Ej(0xC0390, A(&L10));
                if (ok) {
                    if (L10 == 3) return;
                    e.H(0xD360, e.R(G(0xDBC58)) - 0x48, (uint32_t)L10, 1, (uint32_t)L14, 0, 0, A(b18), e.R(G(0xDBC54)));
                    return;
                }
            }
            e.H(0xC0850, P);
            return;
        }
        e.H(0xC07C0, 0, 0, 0, 0, 0, P);
        clearWords(e);
        return;
    }
    case 2: {
        if (e.R(G(0x119721)) != 0 || e.R(G(0x113A68)) != 0) return;
        const uint32_t nm = e.H(0xA5D90, 0, 0, 0, 0, 0, P);
        const uint32_t cls = e.R(P + 4);
        const uint32_t dy = e.H(0xC1B4, P);
        const uint32_t fl = e.R8(e.H(0xC226, cls & 0xFFFF) + 0x167);
        if (fl & 4) {
            const uint32_t d = e.R(P + 0x14);
            cpy(e, A(b198), e.H(0xBE240, 0, 0, 0, 0, 0, 0, 0, 0, 0, e.R8(d + 4)));
            const uint32_t md = e.R(G(0x11C394));
            if (md != 0 && md != 1) {
                uint32_t src = e.H(0x9CE00, 0, 0, 0, 0, 0, 0, e.R(P + 0xC));
                if (src == 0) {
                    src = d + 0x28;
                    if (e.R8(src) == 0) src = 0;
                }
                if (src) {
                    // _sprintf(s118, " (%s) ", src)
                    uint32_t n = 0;
                    s118[n++] = ' '; s118[n++] = '(';
                    for (uint32_t c; n < 0x7C && (c = e.R8(src + n - 2)) != 0; ) s118[n++] = (char)c;
                    s118[n++] = ')'; s118[n++] = ' '; s118[n] = 0;
                    e.H(kW2U, 0x80, 0, 0, 0, 0, 0, A(b18), A(s118));
                    cat(e, A(b198), A(b18));
                }
            }
            e.H(0xD384, e.R(G(0xDBC58)) + dy, 0, 1, 0, 0, 0, A(b198), e.R(G(0xDBC54)));
            return;
        }
        if (fl & 0x80) return;
        if ((fl & 0x40) == 0) {
            e.H(0xD384, e.R(G(0xDBC58)) + dy, 0, 1, 0, 0, 0, nm, e.R(G(0xDBC54)));
            return;
        }
        const uint32_t nm2 = e.H(0xA5D90, 0, 0, 0, 0, 0, P);
        const uint32_t a = e.H(0xC1F0, P);
        if (a == 0) return;
        const uint32_t t = e.H(0xC214, e.H(0xC1DE, a));
        cpy(e, A(b428), nm2);
        cat(e, A(b428), str(e, 0xF9B));
        cat(e, A(b428), str(e, 0xF9E));
        cat(e, A(b428), e.H(0xD1DA, 0, 0, 0, 0, 0, 0, t + 0xF5));
        e.H(0xD384, e.R(G(0xDBC58)) + dy, 0, 1, 0, 0, 0, A(b428), e.R(G(0xDBC54)));
        return;
    }
    case 4:
        if (e.R(G(0xFADB4)) != 0) return;
        e.H(0xBFEC0, 0, 0, 0, 0, 0, P);
        return;
    case 5: {
        const uint32_t a = e.H(0xC1F0, P);
        const uint32_t t = e.H(0xC1A2, a, e.R(P + 4));
        const uint32_t s = e.H(0xC214, t);
        if (s == 0) return;
        const uint32_t r = e.H(0xC1E4, a, P);
        if (r == 0) return;
        const uint32_t x = (e.R(r + 0xC) >> 1) + e.R(r + 4) + e.R(G(0xDBC54));
        const uint32_t y = (e.R(r + 0x10) >> 1) + e.R(r + 8) + e.R(G(0xDBC58));
        uint32_t txt = s + 0x1BE;
        if (t == 8) {
            if (e.R(G(0x11C0A0)) == 0) {   // fatal: Fog 10024(file, 10265(0x60)), exit(-1)
                e.H(0xBF80, 0xB2000000u | 0xD359F, e.H(0xBF92, 0x60));
                e.H(0x3664, 0xFFFFFFFFu);
                return;
            }
            txt = str(e, e.R8(G(0x115591)) ? 0x13DE : 0x13DD);
        }
        b298[0] = 0;
        e.H(kStrncpy, 0xC7, 0, 0, 0, 0, 0, A(b298), txt);
        e.H(0xD384, y, 0, 1, 0, 0, 0, A(b298), x);
        return;
    }
    default:
        return;
    }
}
