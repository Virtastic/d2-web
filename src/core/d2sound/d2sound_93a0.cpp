// src/core/d2sound/d2sound_93a0.cpp -- the DirectSound bring-up, with its module globals and callees as seams.
#include <cstdint>
#include <cstring>

// The module globals the bring-up reads and writes, and the addresses it hands out. The cells are the globals themselves;
// the a* words are what the routine passes as the address of a global or a constant (the engine's own pointers).
struct D2SoundDsInit {
    uint32_t *g6588, *g6590, *g6594, *g6598, *g659c, *g65a0, *g65a8;   // ready flag, 3D flag, EAX flag, EAX mode, EAX emulation, entered, 3D ready
    uint32_t *g64c0, *g64ec, *g6520;                                   // the caps block (size word, hardware 3D buffer count), the listener seed
    uint32_t *g658c, *g652c, *g6528, *g6524, *g64b8;                   // device, primary buffer, EAX buffer, its 3D buffer, the property set
    uint32_t a6530, a658c, a652c, a6528, a6524, a64b8, a64c0;
    uint32_t fmtCreate, fmtCoop, fmtPrimary, fmtCaps, fmt3d, fmtFormat, fmtEax;
    uint32_t iid3dBuffer, iidPropertySet, guidEax2, guidEax1;
    void (*initCs)(uint32_t cs);
    void (*fatal)();                                       // re-entry: logs and exits, never returns
    int32_t (*create)(uint32_t guid, uint32_t out, uint32_t outer);
    uint32_t (*window)();
    uint32_t (*hrText)(int32_t hr);
    void (*log1)(uint32_t fmt);
    void (*log2)(uint32_t fmt, uint32_t a);
    void (*log3)(uint32_t fmt, uint32_t a, uint32_t b);
    void (*teardown)();
    uint32_t (*eaxFormat)(uint32_t one, void* wfx);
    uint32_t (*eaxCheck)(uint32_t mode);
    void (*eaxRelease)();
    void (*bufferInit)(uint32_t primary, uint32_t zero, uint32_t bytes);
    void (*listenerInit)();
    void (*seedInit)(uint32_t seed);
    void (*channelInit)();
    void (*setVolume)(uint32_t vol);
};

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t slot(uint32_t obj, uint32_t off) { return rd32(rd32(obj) + off); }
template <class F> F com(uint32_t obj, uint32_t off) { return (F)(uintptr_t)slot(obj, off); }
using C1 = int32_t (*)(uint32_t);
using C2 = int32_t (*)(uint32_t, uint32_t);
using C3 = int32_t (*)(uint32_t, uint32_t, uint32_t);
using C4 = int32_t (*)(uint32_t, uint32_t, uint32_t, uint32_t);
using C7 = int32_t (*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
uint32_t ptr(const void* p) { return (uint32_t)(uintptr_t)p; }

// the EAX property-set chain on a fresh secondary buffer: create, 3D-buffer QI, property-set QI, QuerySupport(guid, id) & 3 == 3
bool eaxChain(const D2SoundDsInit& s, uint32_t* desc, uint32_t guid, uint32_t id, uint32_t* support) {
    if (com<C4>(*s.g658c, 0xC)(*s.g658c, ptr(desc), s.a6528, 0) < 0) return false;
    if (com<C3>(*s.g6528, 0)(*s.g6528, s.iid3dBuffer, s.a6524) < 0) return false;
    if (com<C3>(*s.g6524, 0)(*s.g6524, s.iidPropertySet, s.a64b8) < 0) return false;
    if (com<C4>(*s.g64b8, 0x14)(*s.g64b8, guid, id, ptr(support)) < 0) return false;
    return (*support & 3) == 3;
}
}  // namespace

// 1.13c D2sound+0x93A0
void D2Sound_93A0(const D2SoundDsInit& s) {
    if (*s.g6590 && *s.g65a8) return;
    if (*s.g65a0) { s.fatal(); return; }
    s.initCs(s.a6530);
    *s.g65a0 = 1;
    int32_t hr = s.create(0, s.a658c, 0);
    if (hr < 0) { s.log2(s.fmtCreate, s.hrText(hr)); s.teardown(); return; }
    uint32_t ds = *s.g658c;
    C3 coop = com<C3>(ds, 0x18);
    uint32_t wnd = s.window();
    hr = coop(*s.g658c, wnd, 2);
    if (hr < 0) { s.log2(s.fmtCoop, s.hrText(hr)); s.teardown(); return; }
    uint32_t desc[5] = { 0x14, *s.g6590 ? 0x11u : 1u, 0, 0, 0 };   // DSBUFFERDESC: primary buffer (| 3D)
    hr = com<C4>(*s.g658c, 0xC)(*s.g658c, ptr(desc), s.a652c, 0);
    if (hr < 0) { s.log2(s.fmtPrimary, s.hrText(hr)); s.teardown(); return; }
    *s.g64c0 = 0x60;
    hr = com<C2>(*s.g658c, 0x10)(*s.g658c, s.a64c0);
    if (hr < 0) { s.log2(s.fmtCaps, s.hrText(hr)); s.teardown(); return; }
    if (*s.g6590 && *s.g64ec < 0x10) { s.log3(s.fmt3d, *s.g64ec, 0x10); s.teardown(); return; }
    uint8_t wfx[20] = {};   // WAVEFORMATEX: PCM, 2 channels, 22050 Hz, 88200 B/s, align 4, 16 bits, cbSize 0
    const uint16_t w0[2] = { 1, 2 }; const uint32_t w1[2] = { 0x5622, 0x15888 }; const uint16_t w2[3] = { 4, 0x10, 0 };
    memcpy(wfx, w0, 4); memcpy(wfx + 4, w1, 8); memcpy(wfx + 12, w2, 6);
    hr = com<C2>(*s.g652c, 0x38)(*s.g652c, ptr(wfx));
    if (hr < 0) { s.log2(s.fmtFormat, s.hrText(hr)); com<C1>(*s.g652c, 8)(*s.g652c); s.teardown(); return; }
    *s.g6598 = 0;
    if (*s.g6594) {
        uint8_t wfx2[0x28] = {};
        uint32_t fmt = s.eaxFormat(1, wfx2);
        uint32_t desc2[5] = { 0x14, 0x12, 0x400, 0, fmt };   // a 3D secondary buffer carrying the EAX format
        uint32_t support = 0;
        if (eaxChain(s, desc2, s.guidEax2, 1, &support) && s.eaxCheck(2)) {
            *s.g6598 = 2;
        } else {
            s.eaxRelease();
            if (!*s.g6598) {
                bool ok = eaxChain(s, desc2, s.guidEax1, 0, &support) && *s.g64b8;
                if (ok) {
                    uint32_t val = 0;
                    ok = com<C7>(*s.g64b8, 0x10)(*s.g64b8, s.guidEax1, 2, 0, 0, ptr(&val), 4) >= 0;
                }
                if (ok) { *s.g659c = 1; *s.g6598 = 1; }
                else {
                    s.eaxRelease();
                    if (!*s.g6598) { s.log1(s.fmtEax); s.teardown(); return; }
                }
            }
        }
    }
    uint32_t caps[5] = { 0x14, 0, 0, 0, 0 };   // DSBCAPS of the primary buffer
    com<C2>(*s.g652c, 0xC)(*s.g652c, ptr(caps));
    s.bufferInit(*s.g652c, 0, caps[2]);
    com<C4>(*s.g652c, 0x30)(*s.g652c, 0, 0, 1);
    s.listenerInit();
    s.seedInit(*s.g6520);
    s.channelInit();
    s.setVolume(0xFF);
    *s.g6588 = 1;
}
