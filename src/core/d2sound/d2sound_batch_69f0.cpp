// src/core/d2sound/d2sound_batch_69f0.cpp -- 1.13c D2Sound.dll helpers reimplemented from the decompiles
// (0x69F0, 0x7090, 0x7AA0, 0x7BC0, 0x8790, 0x84A0). Retail offsets stay explicit byte offsets into the
// untyped 32-bit stream record; unported callees / unreadable retail data are host seams.
// Not ported (no faithful C form from the decompile): 0x6360 and 0x72D0 and 0x6A40 (implicit register
// args, lost call arguments), 0x9CF0 / 0xA61C / 0xA057 (x87 CRT math thunks).
#include <cstdint>
#include <initializer_list>
#include <cstring>
#include <unistd.h>
#include <Windows.h>

static inline uint32_t rd32(const void* p, uint32_t off) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + off, 4); return v; }
static inline void wr32(void* p, uint32_t off, uint32_t v) { std::memcpy((uint8_t*)p + off, &v, 4); }
static inline void* rdptr(const void* p, uint32_t off) { void* v; std::memcpy(&v, (const uint8_t*)p + off, sizeof v); return v; }
static void fatal69(uint32_t) { _exit(-1); }  // Ordinal_10265 -> Ordinal_10024 -> _exit(-1)
static void comRelease(void* itf) { void** vt = *(void***)itf; ((void (*)(void*))vt[2])(itf); }

// 1.13c D2sound+0x69F0
// Stop the stream (0x6980), release the COM objects at +0x10, +0x0C, +0x00 (clearing each slot), zero 0x60 bytes.
void D2sound_69F0(void* obj, void (*stop6980)(void*)) {
    stop6980(obj);
    for (uint32_t off : {0x10u, 0x0Cu, 0x00u}) {
        if (void* itf = rdptr(obj, off)) { comRelease(itf); wr32(obj, off, 0); }
    }
    std::memset(obj, 0, 0x60);
}

// 1.13c D2sound+0x7090
// Create the DirectSound buffer for stream `obj`; flags bit0 = stereo (double size), bit1 = 3D/control.
// Returns 1 and fills +0x04 size, +0x08 half size, +0x30 = -1, +0x34 = flags; 0 on failure.
// env: dsound = DAT_6F9C658C, hwMix = DAT_6F9C6584, has3d = DAT_6F9C6594, ds3d = DAT_6F9C6598,
// guidA/guidB = the IIDs at 0x6F9C01C4 / 0x6F9C01B4, guidP1/guidP2 = 0x6F9C01E8 / 0x6F9C0208.
struct D2sound7090Env { void* dsound; uint32_t hwMix, has3d, ds3d; const void *guidA, *guidB, *guidP1, *guidP2; };
uint32_t D2sound_7090(void* obj, uint32_t flags, const D2sound7090Env* e) {
    uint16_t ch = ((flags & 1) != 0) + 1;
    uint8_t wfx[18] = {};                                   // WAVEFORMATEX, 16-bit PCM @ 22050 Hz
    uint16_t v;
    v = 1; std::memcpy(wfx + 0, &v, 2);
    std::memcpy(wfx + 2, &ch, 2);
    uint32_t rate = 0x5622; std::memcpy(wfx + 4, &rate, 4);
    uint32_t avg = (uint32_t)(uint16_t)(ch * 2) * rate; std::memcpy(wfx + 8, &avg, 4);
    v = (uint16_t)(ch * 2); std::memcpy(wfx + 12, &v, 2);
    v = 16; std::memcpy(wfx + 14, &v, 2);
    uint32_t bytes = ((flags & 1) ? 0x20000u : 0u) + 0x20000u;
    uint32_t dsFlags = (e->hwMix ? 0x8000u : 0u) + 0xA0u;
    uint32_t ctl = flags & 2;
    dsFlags |= ctl ? 0x14u : 0x40u;
    uint32_t desc[5] = { 0x14, dsFlags, bytes, 0, 0 };      // DSBUFFERDESC
    void* wp = wfx; std::memcpy(&desc[4], &wp, sizeof wp);  // lpwfxFormat (host pointer width: seam only)
    void* dsb = nullptr;
    void** dvt = *(void***)e->dsound;
    if ((int32_t)((int32_t (*)(void*, void*, void**, void*))dvt[0xC / 4])(e->dsound, desc, &dsb, nullptr) < 0) return 0;
    std::memcpy((uint8_t*)obj, &dsb, sizeof dsb);
    auto ok = [&] { wr32(obj, 4, bytes); wr32(obj, 0xD * 4, flags); wr32(obj, 8, bytes >> 1); wr32(obj, 0xC * 4, 0xFFFFFFFF); return 1u; };
    if (!ctl) return ok();
    void** bvt = *(void***)dsb;
    void* itf3 = nullptr;
    if ((int32_t)((int32_t (*)(void*, const void*, void**))bvt[0])(dsb, e->guidA, &itf3) >= 0) {
        std::memcpy((uint8_t*)obj + 0xC, &itf3, sizeof itf3);
        if (!e->has3d) return ok();
        void* ps = nullptr;
        void** ivt = *(void***)itf3;
        if ((int32_t)((int32_t (*)(void*, const void*, void**))ivt[0])(itf3, e->guidB, &ps) >= 0) {
            std::memcpy((uint8_t*)obj + 0x10, &ps, sizeof ps);
            uint32_t support = 0;
            void** pvt = *(void***)ps;
            int32_t r = ((int32_t (*)(void*, const void*, uint32_t, uint32_t*))pvt[0x14 / 4])(ps, e->ds3d == 1 ? e->guidP1 : e->guidP2, 1, &support);
            if (r >= 0 && (support & 3) == 3) return ok();
            comRelease(ps);
        }
    }
    comRelease(dsb);
    return 0;
}

// 1.13c D2sound+0x7AA0
// Create the manual-reset event and the worker thread; fatal (message 0xBC) if the thread already exists.
// The thread body (0x7800) is not ported, so it arrives as `proc`.
static HANDLE g_evt65d0, g_thr65d4;
void D2sound_7AA0(LPTHREAD_START_ROUTINE proc) {
    if (g_thr65d4) fatal69(0xBC);
    g_evt65d0 = CreateEventA(nullptr, 1, 0, nullptr);
    DWORD id;
    g_thr65d4 = CreateThread(nullptr, 0, proc, nullptr, 0, &id);
}

// 1.13c D2sound+0x7BC0
// Clear the "loaded" word (+4) of every entry of the active music table. Entries are {name, loaded}
// 8-byte pairs; the table is the intro one on the first call (g_e0 latches, g_dc = DAT_6F9C65B4) when
// g_dc != 0, else the common one. count is DAT_6F9C5948 and must equal DAT_6F9C5990 (fatal 0x52);
// 0x53 if the index runs past count.
struct D2sound7BC0State { uint32_t latched, introSel, sel65b4, count, count5990; uint32_t *introTab, *commonTab; };
void D2sound_7BC0(D2sound7BC0State* s) {
    uint32_t count = s->count, sel = s->sel65b4;
    for (uint32_t i = 0; (int32_t)i < (int32_t)count; ) {
        if (s->latched == 0) { s->latched = 1; s->introSel = sel; }
        if (count != s->count5990) fatal69(0x52);
        if ((int32_t)count <= (int32_t)i) fatal69(0x53);
        uint32_t* tab = s->introSel ? s->introTab : s->commonTab;
        tab[i * 2 + 1] = 0;
        i++;
    }
}

// 1.13c D2sound+0x8790
// Walk RIFF sub-chunks {tag, len, data[len]} in `data` (remaining byte count *size) for `tag`.
// Returns the payload and its length in *outLen, or null when the tag is missing / the data is short.
int32_t* D2sound_8790(uint32_t* size, int32_t* data, int32_t tag, uint32_t* outLen) {
    uint32_t left = *size;
    for (;;) {
        if (left < 8) return nullptr;
        left = *size - 8; *size = left;
        uint32_t len = (uint32_t)data[1];
        if (data[0] == tag) { *outLen = len; return data + 2; }
        if (left < len) return nullptr;
        left -= len;
        data = (int32_t*)((uint8_t*)(data + 2) + len);
        *size = left;
    }
}

// 1.13c D2sound+0x84A0
// Read the first resource blob out of an LE image: map `path`, check "MZ" and the "LE" header
// (e_lfanew at +0x3C), take the record at header+0xB8 (Win32 error 0x714 if header+0xBC is 0).
// Copies it into `dest` (zeroed *size bytes first) when it fits and sets LastError 0, returns 1;
// otherwise *size = needed and LastError 0x7A. Not-LE / not-MZ -> LastError 0xB. Failure to open/map -> 0.
uint32_t D2sound_84A0(void* dest, const char* path, uint32_t* size) {
    HANDLE f = CreateFileA(path, 0x80000000, 1, nullptr, 3, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return 0;
    HANDLE m = CreateFileMappingA(f, nullptr, 2, 0, 0, nullptr);
    if (!m) return 0;
    uint8_t* v = (uint8_t*)MapViewOfFile(m, 4, 0, 0, 0);
    if (!v) return 0;
    uint32_t ret = 0, err;
    if (*(uint16_t*)v == 0x5A4D) {
        uint8_t* pe = v + rd32(v, 0x3C);
        if (rd32(pe, 0) != 0x454C) err = 0xB;
        else if (rd32(pe, 0xBC) == 0) { *size = 0; err = 0x714; }
        else {
            uint32_t rec = rd32(pe, 0xB8);
            uint32_t n = rd32(v, rec + 8);
            if (dest && n <= *size) {
                std::memset(dest, 0, *size);
                std::memcpy(dest, v + rec + 0xC, n);
                *size = n;
                SetLastError(0);
                return 1;
            }
            *size = n; err = 0x7A;
        }
    } else err = 0xB;
    SetLastError(err);
    return ret;
}

// 1.13c D2sound+0x6A40
// Silence a DirectSoundBuffer region (this = ESI = IDirectSoundBuffer*, stdcall(offset, bytes)): under the
// global CS at 0x6F9C6530, Lock(offset, bytes, &p1, &len1, 0,0,0) (vtbl+0x2c); on failure leave, return 0.
// Else zero len1 bytes at p1, Unlock(p1, len1, 0, 0) (vtbl+0x4c), leave, return 1. NULL this -> fatal (_exit(-1)).
uint32_t D2sound_6A40(void* dsb, uint32_t offset, uint32_t bytes, CRITICAL_SECTION* cs) {
    if (!dsb) _exit(-1);
    EnterCriticalSection(cs);
    void** vt = *(void***)dsb;
    void* p1 = nullptr; uint32_t len1 = bytes;
    if ((int)((int (__stdcall*)(void*, uint32_t, uint32_t, void**, uint32_t*, void*, void*, uint32_t))vt[0x2c / 4])(dsb, offset, bytes, &p1, &len1, nullptr, nullptr, 0) < 0) {
        LeaveCriticalSection(cs);
        return 0;
    }
    std::memset(p1, 0, len1);
    ((int (__stdcall*)(void*, void*, uint32_t, void*, uint32_t))vt[0x4c / 4])(dsb, p1, len1, nullptr, 0);
    LeaveCriticalSection(cs);
    return 1;
}
