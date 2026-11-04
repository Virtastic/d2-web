// src/core/d2sound/d2sound_batch_67c0.cpp -- 14 1.13c D2Sound.dll functions reimplemented from the
// decompiles in build-wasm/ghidra-d2sound/decomp/ (0x<RVA>.c), checked against the retail bytes at
// 0x6F9B0000+<RVA>. These are the module's internal stream/mixer helpers (ordinals 10003..10026 and
// the master-volume ramp). They all run on the 32-bit stream record, so every retail offset stays an
// explicit byte offset into an untyped object (rd32/wr32/rdptr below).
//
// The unmapped internal callees (0x6680, 0x68D0, 0x69F0, 0x6CE0, 0x6E20, 0x7090, 0x7480, 0x7740,
// 0x8790, 0x8830, 0xACCC) are host seams -- the src/core/d2client/cursor_init_166c0.cpp pattern:
// a function that reaches one takes a D2SoundEnv* AFTER its retail arguments, which keep their order.
#include <cstdint>
#include <initializer_list>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <Windows.h>   // CRITICAL_SECTION no-ops and the deterministic GetTickCount

// ---- byte access into the retail stream record ------------------------------------------------
static inline uint32_t rd32(const void* p, uint32_t off) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + off, 4); return v; }
static inline void wr32(void* p, uint32_t off, uint32_t v) { std::memcpy((uint8_t*)p + off, &v, 4); }
static inline void* rdptr(const void* p, uint32_t off) { void* v; std::memcpy(&v, (const uint8_t*)p + off, sizeof v); return v; }
static inline uint8_t rd8(const void* p, uint32_t off) { return ((const uint8_t*)p)[off]; }
static inline float rdf(const void* p, uint32_t off) { float v; std::memcpy(&v, (const uint8_t*)p + off, 4); return v; }

// Retail's fatal path is Ordinal_10265(message id) -> Ordinal_10024(&0x6F9C0684, text) -> _exit(-1);
// only the exit is modelled (the two ordinals are the module's message helper).
static void d2sound_fatal(uint32_t) { _exit(-1); }

// ---- module state; addresses are the retail ones, all zero-initialised unless noted -------------
static CRITICAL_SECTION g_lock;        // 0x6F9C6530  guards every stream field below
static CRITICAL_SECTION g_initLock;    // 0x6F9C6548  0x83C0's init interlock
static void*    g_streams[8];          // 0x6F9C6560..0x6F9C657C  created by 0x83C0, drained by 0x8100
static void*    g_primary;             // 0x6F9C6580  the primary stream (0x7FB0/0x7FE0)
static uint32_t g_soundOn;             // 0x6F9C658C  nonzero once the device is open
static uint32_t g_masterVolume = 0xFF; // 0x6F9C5994  0..0xFF; 0x83C0 pins it to 0xFF
static uint32_t g_initialized;         // 0x6F9C65B8
static uint32_t g_firstPick;           // 0x6F9C65C4  nonzero: take entry 0 once
static uint32_t g_stopFlag;            // 0x6F9C65C0  cleared by 0x7FB0/0x7FE0
static void*    g_list;                // 0x6F9C65CC  nodes appended by 0x8640, walked by 0x83C0
static uint32_t g_flag5900 = 1;        // 0x6F9C5900  tested by 0x7650
static uint32_t g_ds3d;                // 0x6F9C6598  1 = the DS3D property path in 0x6C00
static uint32_t g_silent;              // 0x6F9C659C  nonzero forces 0x6C00's value to 0
// The attenuation curve's constants (0x6F9C0964..0x6F9C097C), retail values.
static float  g_c0964 = 0.0005f;       // 0x3A03126F
static double g_c0968 = 10.0;          // 0x4024000000000000
static float  g_c0970 = -10000.0f;     // 0xC61C4000
static float  g_c0974 = 1.0f;          // 0x3F800000
static float  g_c0978 = 0.0f;          // 0x00000000
static float  g_c097C = 0.0001f;       // 0x38D1B717

// 0x8640's list node: value first, next at +0x04 (8 bytes from Fog #10042).
struct D2SoundNode { uint32_t value; D2SoundNode* next; };

// Host seams for the unmapped internal callees, in their retail argument order.
struct D2SoundEnv {
    void (*releaseStream)(void* obj);                                         // 0x6680
    void (*stopAll)(void* obj);                                               // 0x69F0 (EAX = obj)
    void (*select)(void* obj, uint32_t field34);                              // 0x7090 (EAX = obj, stack = +0x34)
    void (*stopStream)(void* obj, uint32_t a, uint32_t b, uint32_t c);        // 0x68D0
    void (*setPan)(void* obj, float v);                                       // 0x6CE0
    void (*setVolume)(void* obj, uint32_t v);                                 // 0x6E20 (ECX = obj, EDX = v)
    void (*setMasterVolume)(uint32_t v);                                      // 0x7480 (ECX = v)
    void* (*createStream)(uint32_t a, uint32_t b);                            // 0x7740 -> new stream
    int32_t* (*findChunk)(uint32_t* size, const int32_t* data, uint32_t tag,
                          uint32_t* outLen);                                  // 0x8790
    uint16_t (*attenuate)(float a, float b, float c);                         // 0x8830
    uint64_t (*mixValue)(void);                                               // 0xACCC
    int32_t (*storm258)(uint32_t h, uint32_t* a, uint8_t* b);                 // Storm.dll#258 (0x5F4A thunk)
    void* (*loadFile)(void* name, int32_t* outSize);                          // 0x6360 (name = 0: the pending file)
    void (*shutdownWorker)(void);                                             // Ordinal_256 + SetEvent/Wait/CloseHandle of the worker thread (0x65D0/0x65D4)
};

// 1.13c D2Sound+0x67C0
// Is the stream active? Reads +0x48 (the same word 0x6B80 sets at +0x4C's neighbour).
uint32_t D2Sound_67C0(void* obj) {
    if (!obj) d2sound_fatal(0x37A);
    return rd32(obj, 0x48) != 0 ? 1u : 0u;
}

// 1.13c D2Sound+0x68D0
// If flag bit 1 (+0x34) is set, call method +0x4C of the interface at +0xC with (a, b, c, 0).
void D2Sound_68D0(void* obj, uint32_t a, uint32_t b, uint32_t c) {
    if (!obj || !rd32(obj, 0)) d2sound_fatal(0x302);
    if (rd32(obj, 0x34) & 2) {
        const uint32_t iface = rd32(obj, 0xC);
        const uint32_t vt = rd32((const void*)(uintptr_t)iface, 0);
        ((void (*)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))(uintptr_t)rd32((const void*)(uintptr_t)vt, 0x4C))(iface, a, b, c, 0);
    }
}

// 1.13c D2Sound+0x6920
// The handle/value word at +0x38 (0x6B80 snapshots it into +0x50).
uint32_t D2Sound_6920(void* obj) {
    if (!obj) d2sound_fatal(0x2E4);
    return rd32(obj, 0x38);
}

// 1.13c D2Sound+0x6980
// Tear the stream down: release +0x48's backing object, call interface method +0x48, clear +0x1C.
void D2Sound_6980(void* obj, const D2SoundEnv* env) {
    if (!obj || !rd32(obj, 0)) d2sound_fatal(0x292);
    EnterCriticalSection(&g_lock);
    if (rd32(obj, 0x48)) env->releaseStream(obj);
    void* iface = rdptr(obj, 0);
    void** vt = *(void***)iface;
    ((void (*)(void*))vt[0x48 / 4])(iface);
    wr32(obj, 0x1C, 0);
    LeaveCriticalSection(&g_lock);
}

// 1.13c D2Sound+0x6B80
// Start (or restart) playback: mark playing (+0x4C), snapshot +0x38 into +0x50, store the caller's
// word in +0x54 and a deadline (now, now + ms) in +0x58/+0x5C. No-op before the device opens.
void D2Sound_6B80(void* obj, uint32_t arg, int32_t ms) {
    if (!obj) d2sound_fatal(0x39E);
    if (!g_soundOn) return;
    EnterCriticalSection(&g_lock);
    wr32(obj, 0x4C, 1);
    wr32(obj, 0x50, rd32(obj, 0x38));
    wr32(obj, 0x54, arg);
    uint32_t now = GetTickCount();
    wr32(obj, 0x58, now);
    wr32(obj, 0x5C, now + (uint32_t)ms);
    LeaveCriticalSection(&g_lock);
}

// Test hook: sets the sound-on flag (0x6F9C658C) that the fade-start function tests.
void D2Sound_fnSetSoundOn(uint32_t v) { g_soundOn = v; }
// Test hooks (FN probes): install the retail globals DAT_6f9c6580 (primary stream) and DAT_6f9c6560..657C + DAT_6f9c65B8
// (the 8 stream slots and the init flag) at the host values the descriptor pins.
void D2Sound_fnSetPrimary(void* p) { g_primary = p; }
// FN-probe hooks for the scalar module globals: DAT_6f9c658c (sound-on), DAT_6f9c5994 (master volume), DAT_6f9c65c0
// (music stop flag), DAT_6f9c65c4 (first-pick flag), DAT_6f9c65cc (the stream list head), and the music-track tables.
void D2Sound_fnSetCore(uint32_t soundOn, uint32_t vol, uint32_t stopFlag, uint32_t firstPick) {
    g_soundOn = soundOn; g_masterVolume = vol; g_stopFlag = stopFlag; g_firstPick = firstPick;
}
void D2Sound_fnGetCore(uint32_t* vol, uint32_t* stopFlag, uint32_t* firstPick) {
    *vol = g_masterVolume; *stopFlag = g_stopFlag; *firstPick = g_firstPick;
}
void D2Sound_fnSetList(void* l) { g_list = l; }
void D2Sound_fnSetStreams(void* const* slots, uint32_t initialized) {
    for (int i = 0; i < 8; i++) g_streams[i] = slots[i];
    g_initialized = initialized;
}

// The interface method behind +0x10 is IKsPropertySet::Set(this, guid, id, inst, instLen, data, len).
static void d2sound_setProp(void* iface, const void* guid, uint32_t id, const void* data, uint32_t len) {
    void** vt = *(void***)iface;
    ((void (*)(void*, const void*, uint32_t, void*, uint32_t, const void*, uint32_t))vt[0x10 / 4])
        (iface, guid, id, nullptr, 0, data, len);
}

// The two property-set GUIDs, copied byte-for-byte from 0x6F9C01E8 and 0x6F9C0208.
static const uint8_t kGuid01E8[16] = { 0xC0, 0x6F, 0x4E, 0x4A, 0x41, 0xC3, 0xD1, 0x11,
                                       0xB7, 0x3A, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 };
static const uint8_t kGuid0208[16] = { 0xA7, 0xA6, 0x06, 0x03, 0x24, 0xB2, 0xD2, 0x11,
                                       0x99, 0xE5, 0x00, 0x00, 0xE8, 0xD8, 0xC7, 0x22 };

// Guest-layout COM access (declared ahead of 0x6F30 below): the retail record holds 4-byte pointers and vtable slots are 4-byte code addresses.
static inline uint32_t g32(uint32_t addr, uint32_t off);
// IKsPropertySet::Set(iface, guid, id, 0, 0, data, len): vtable +0x10, stdcall, the interface a guest dword.
static void d2sound_prop32(uint32_t iface, const uint8_t* guid, uint32_t id, const void* data, uint32_t len) {
    typedef int32_t (*PropFn)(void*, const void*, uint32_t, uint32_t, uint32_t, const void*, uint32_t);
    ((PropFn)(uintptr_t)g32(g32(iface, 0), 0x10))((void*)(uintptr_t)iface, guid, id, 0, 0, data, len);
}
// FN probe hook: DAT_6f9c6598 (the DS3D mode) and DAT_6f9c659c (silent) as the descriptor pins them.
void D2Sound_fnSetModes(uint32_t ds3d, uint32_t silent) { g_ds3d = ds3d; g_silent = silent; }

// 1.13c D2Sound+0x6C00 (thiscall, float on the stack, ret 4)
// Push a float into the stream's +0x10 interface. silent (DAT_659C) forces the value to 0.0f. Mode 1: the value is replaced by -1.0f
// unless it equals 0.0f (a NaN is replaced too: fucompp unordered reaches the substitution), then property 1 of GUID 0x6F9C01E8 gets
// the float. Any other mode: v = trunc(0x8830(0, 1, value)) and property 5, then property 4, of GUID 0x6F9C0208 get that dword.
void D2Sound_6C00(void* obj, float value, const D2SoundEnv* env) {
    if (!obj) d2sound_fatal(0x359);
    if (g_silent) value = 0.0f;
    const uint32_t iface = rd32(obj, 0x10);
    if (!iface) return;
    if (g_ds3d == 1) {
        if (!(value == g_c0978)) value = -1.0f;
        d2sound_prop32(iface, kGuid01E8, 1, &value, 4);
        return;
    }
    env->attenuate(0.0f, 1.0f, value);
    uint32_t mixed = (uint32_t)env->mixValue();
    d2sound_prop32(iface, kGuid0208, 5, &mixed, 4);
    d2sound_prop32(iface, kGuid0208, 4, &mixed, 4);
}

// 1.13c D2Sound+0x7510
// Ramp the master volume down to 0 in steps of g_masterVolume / (percent/20), sleeping 20 ms a step.
void D2Sound_7510(uint32_t percent, const D2SoundEnv* env) {
    if (!g_soundOn) return;
    if ((int32_t)percent < 1) d2sound_fatal(0x387);
    // Retail divides by (percent/20) unsigned; percent < 20 traps there and here (division by zero).
    uint32_t step = g_masterVolume / (percent / 0x14);
    uint32_t v = g_masterVolume;
    while (v != 0) {
        usleep(20000);                       // retail Sleep(0x14)
        v = (step < v) ? v - step : 0;
        env->setMasterVolume(v);
    }
}

// 1.13c D2Sound+0x7650
// Stop a stream that is not already playing (+0x48 must be 0): reset it (or zero the cached fields when
// 0x5900 is set), stop the +0x00 interface, pin the +0x0C interface's two levels, stop the stream either
// through +0x34&2's helper or the interface's restart method, then clear pan and volume.
void D2Sound_7650(void* obj, const D2SoundEnv* env) {
    if (!obj || !rd32(obj, 0)) d2sound_fatal(0x226);
    if (rd32(obj, 0x48)) d2sound_fatal(0x227);
    EnterCriticalSection(&g_lock);
    if (!g_flag5900) {
        uint32_t field34 = rd32(obj, 0x34);
        env->stopAll(obj);
        env->select(obj, field34);
    } else {
        wr32(obj, 0x14, 0);
        wr32(obj, 0x18, 0);
        wr32(obj, 0x20, 0);
        wr32(obj, 0x24, 0);
        wr32(obj, 0x2C, 0);
        wr32(obj, 0x40, 0);
        wr32(obj, 0x44, 0);
    }
    // COM objects are retail 32-bit pointers: their words are guest dwords (vtable at +0, slot n at vtable + 4n).
    const uint32_t iface = rd32(obj, 0);
    const uint32_t vt = rd32((const void*)(uintptr_t)iface, 0);
    ((void (*)(uint32_t, uint32_t))(uintptr_t)rd32((const void*)(uintptr_t)vt, 0x34))(iface, 0);
    const uint32_t aux = rd32(obj, 0x0C);
    if (aux) {
        const uint32_t avt = rd32((const void*)(uintptr_t)aux, 0);
        ((void (*)(uint32_t, float, uint32_t))(uintptr_t)rd32((const void*)(uintptr_t)avt, 0x40))(aux, 100.0f, 0);
        ((void (*)(uint32_t, float, uint32_t))(uintptr_t)rd32((const void*)(uintptr_t)rd32((const void*)(uintptr_t)aux, 0), 0x44))(aux, 2.0f, 0);
    }
    if (!(rd8(obj, 0x34) & 2))
        ((void (*)(uint32_t, uint32_t))(uintptr_t)rd32((const void*)(uintptr_t)vt, 0x40))(iface, 0);
    else
        env->stopStream(obj, 0, 0, 0);
    env->setPan(obj, 0.0f);
    env->setVolume(obj, 0xFF);
    LeaveCriticalSection(&g_lock);
}

// 1.13c D2Sound+0x7FB0
// Restart the primary stream for 200 ms if it is active; always drops the stop flag.
void D2Sound_7FB0(void) {
    void* p = g_primary;
    if (!p) return;
    uint32_t active = rd32(p, 0x48);
    g_stopFlag = 0;
    if (active) D2Sound_6B80(p, 0, 200);
}

// 1.13c D2Sound+0x7FE0
// Tear the primary stream down; always drops the stop flag.
void D2Sound_7FE0(const D2SoundEnv* env) {
    void* p = g_primary;
    if (!p) return;
    uint32_t active = rd32(p, 0x48);
    g_stopFlag = 0;
    if (active) D2Sound_6980(p, env);
}

// 1.13c D2Sound+0x8100
// Restart every global stream that is currently playing (+0x1C), 200 ms each; a null/empty slot is fatal.
void D2Sound_8100(void) {
    if (!g_initialized) return;
    for (int i = 0; i < 8; ++i) {
        void* p = g_streams[i];
        if (!p || !rd32(p, 0)) d2sound_fatal(0x2A5);
        if (rd32(p, 0x1C)) D2Sound_6B80(p, 0, 200);
    }
}

// 1.13c D2Sound+0x83C0
// Bring the device up: pin the master volume to 0xFF (re-applying every list entry's own +0x38 volume),
// then create the eight global streams (indices 0..3 with the first argument set, 4..7 without).
void D2Sound_83C0(const D2SoundEnv* env) {
    if (!g_soundOn || g_initialized) return;
    InitializeCriticalSection(&g_initLock);
    EnterCriticalSection(&g_initLock);
    if (g_soundOn && g_masterVolume != 0xFF) {
        EnterCriticalSection(&g_lock);
        g_masterVolume = 0xFF;
        for (D2SoundNode* n = (D2SoundNode*)g_list; n; n = n->next) {
            void* p = (void*)(uintptr_t)n->value;
            if (!p) d2sound_fatal(0x2E4);
            env->setVolume(p, rd32(p, 0x38));
        }
        LeaveCriticalSection(&g_lock);
    }
    for (int i = 0; i < 8; ++i)
        g_streams[i] = env->createStream((uint32_t)(i < 4), 0);
    g_initialized = 1;
    LeaveCriticalSection(&g_initLock);
}

// 1.13c D2Sound+0x8640
// Append a {value, next} node to the list at *list (retail allocates 8 bytes through Fog #10042).
void D2Sound_8640(D2SoundNode** list, uint32_t value) {
    D2SoundNode* node = (D2SoundNode*)std::malloc(sizeof(D2SoundNode));
    node->value = value;
    node->next = nullptr;
    D2SoundNode* cur = *list;
    if (!cur) { *list = node; return; }
    while (cur->next) cur = cur->next;
    cur->next = node;
}

// 1.13c D2Sound+0x87D0
// The distance/pan curve: 1.0f above the 0.0f bound, 0.0f below -10000.0f, and pow(10.0, x * 0.0005f)
// (0x9CF0's x87 fyl2x path) in between. Both bounds are fcomps compares, so NaN takes the pow branch.
double D2Sound_87D0(float x) {
    if (g_c0978 < x + g_c097C) return (double)g_c0974;
    if (x - g_c097C < g_c0970) return (double)g_c0978;
    return std::pow(g_c0968, (double)(x * g_c0964));  // 10^(x*0.0005): -10000..0 hundredths-dB -> 1e-5..1
}

// D2Sound 0x8980 (1.13c)
// Parse a RIFF/WAVE header in place: 'RIFF' + 'WAVE', the 'fmt ' chunk (>= 16 bytes) is copied into the
// 18-byte WAVEFORMATEX-ish out record, then the 'data' chunk is located. *size is consumed as the
// chunks are walked; on success it ends as the data chunk's length and the data payload is returned.
int32_t* D2Sound_8980(int32_t* riff, uint32_t* size, uint16_t* out, const D2SoundEnv* env) {
    if (*size < 0x20) return nullptr;
    *size -= 0xC;
    if (riff[0] != 0x46464952 || riff[2] != 0x45564157) return nullptr;   // 'RIFF', 'WAVE'
    uint32_t fmtLen = 0;
    int32_t* fmt = env->findChunk(size, riff + 3, 0x20746D66u, &fmtLen);  // 'fmt '
    if (!fmt) return nullptr;
    if (!(0xF < fmtLen && fmtLen <= *size)) return nullptr;
    *size -= fmtLen;
    ((uint16_t*)out)[0] = ((const uint16_t*)fmt)[0];
    ((uint16_t*)out)[1] = ((const uint16_t*)fmt)[1];
    *(int32_t*)(out + 2) = fmt[1];
    *(int32_t*)(out + 4) = fmt[2];
    ((uint16_t*)out)[6] = (uint16_t)fmt[3];
    ((uint16_t*)out)[7] = ((const uint16_t*)fmt)[7];
    ((uint16_t*)out)[8] = 0;
    uint32_t dataLen = 0;
    int32_t* data = env->findChunk(size, (const int32_t*)((const uint8_t*)fmt + fmtLen),
                                   0x61746164u, &dataLen);               // 'data'
    if (data && dataLen <= *size) { *size = dataLen; return data; }
    return nullptr;
}

// ---- 0x7D30 music-track picker state (retail addresses) -----------------------------------------
struct D2SoundTrack { uint32_t name; uint32_t played; };   // {path ptr, "already played" flag}
static D2SoundTrack g_tracksA[64];     // 0x6F9C5950  intro-first list (used while g_listSel != 0)
static D2SoundTrack g_tracksB[64];     // 0x6F9C5908  common list
static int32_t  g_trackCount = 0;      // 0x6F9C5948
static int32_t  g_trackCount2 = 0;     // 0x6F9C5990  must equal g_trackCount or retail dies (id 0x52)
static uint32_t g_listInit;            // 0x6F9C65E0
static uint32_t g_listSel;             // 0x6F9C65DC  copied from g_listDefault on first use
static uint32_t g_listDefault;         // 0x6F9C65B4

// Host seams for 0x7D30's unmapped callees.
struct D2SoundPickEnv {
    void (*resetPlayed)(void);                       // 0x7BC0 (clears every played flag)
    void (*setVolume)(void* obj, uint32_t v);        // 0x6E20
    uint32_t (*play)(void* obj, uint32_t a, uint32_t b);  // 0x66F0
    const D2SoundEnv* env;                           // for 0x7650
};

// Retail's inlined track lookup: lazy list select, the two fatal checks (0x52, 0x53), the table.
static D2SoundTrack* d2sound_track(int32_t i) {
    if (!g_listInit) { g_listInit = 1; g_listSel = g_listDefault; }
    if (g_trackCount != g_trackCount2) d2sound_fatal(0x52);
    if (i >= g_trackCount || i < 0) d2sound_fatal(0x53);
    return (g_listSel ? g_tracksA : g_tracksB) + i;
}

// 1.13c D2sound+0x7D30
// Pick the next unplayed music track, start it on the primary stream, return 1; 0 if none/not ready.
uint32_t D2Sound_7D30(const D2SoundPickEnv* pe) {
    EnterCriticalSection(&g_lock);
    auto ready = [] { auto* p = (uint8_t*)g_primary; return p && g_soundOn && g_masterVolume && rd32(p, 0x48) == 0; };
    if (!ready()) { LeaveCriticalSection(&g_lock); return 0; }
    bool reset = false;
    for (;;) {
        D2SoundTrack* t = nullptr;
        for (int32_t i = 0; i < g_trackCount; i++) {
            D2SoundTrack* c = d2sound_track(i);
            if (!c->played) { t = c; break; }
        }
        if (!t) {
            if (reset) { g_stopFlag = 0; LeaveCriticalSection(&g_lock); return 0; }
            pe->resetPlayed();
            reset = true;
        }
        if (g_firstPick == 0) {
            int32_t i = rand() % g_trackCount;
            for (;; ) {
                t = d2sound_track(i);
                if (!t->played) break;
                if (++i >= g_trackCount) i = 0;
            }
        } else {
            t = d2sound_track(0);
            if (g_trackCount < 1) d2sound_fatal(0x53);
            g_firstPick = 0;
        }
        t->played = 1;
        if (ready() && t->name) {
            D2Sound_7650(g_primary, pe->env);
            pe->setVolume(g_primary, 0x6E);
            if (pe->play(g_primary, 0, 0)) { LeaveCriticalSection(&g_lock); return 1; }
        }
    }
}

// 1.13c D2sound+0x06850
// Query the stream handle at +0x48 through Storm #258; returns its first out word >> 2, 0 if no handle/failed.
uint32_t D2Sound_6850(void* obj, const D2SoundEnv* env) {
    if (!obj) d2sound_fatal(0x310);
    uint32_t a = 0; uint8_t b[4];
    EnterCriticalSection(&g_lock);
    uint32_t h = rd32(obj, 0x48);
    if (h && env->storm258(h, &a, b)) { LeaveCriticalSection(&g_lock); return a >> 2; }
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c D2sound+0x06820
// Store v at obj+0x40; null obj is fatal (0x32F).
void D2Sound_6820(void* obj, uint32_t v) {
    if (!obj) d2sound_fatal(0x32F);
    wr32(obj, 0x40, v);
}

// 1.13c D2sound+0x080A0
// 1 if any of the 8 streams has a nonzero +0x1C; a null stream or zero head word is fatal. 0 if uninitialized/none.
uint32_t D2Sound_80A0(void) {
    if (!g_initialized) return 0;
    for (int i = 0; i < 8; i++) {
        void* p = g_streams[i];
        if (!p || rd32(p, 0) == 0) d2sound_fatal(0x2A5);
        if (rd32(p, 0x1C)) return 1;
    }
    return 0;
}

// Guest-layout access for the COM calls of 0x6C00, 0x6CE0 and 0x6F30: the retail record holds 4-byte pointers and a vtable
// slot is a 4-byte code address, so they are read as dwords (a void* element would be 8 bytes on wasm64).
static inline uint32_t g32(uint32_t addr, uint32_t off) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(addr + off), 4); return v; }

// 1.13c D2sound+0x06F30
// Call the stream interface's vtable+0x30 (0,0,1) under the lock; on success set +0x1C=1, return 1; failure returns 0.
uint32_t D2Sound_6F30(void* obj) {
    if (!obj) d2sound_fatal(0x279);
    EnterCriticalSection(&g_lock);
    const uint32_t iface = rd32(obj, 0);
    typedef int32_t (*Slot4)(uint32_t, uint32_t, uint32_t, uint32_t);
    if (((Slot4)(uintptr_t)g32(g32(iface, 0), 0x30))(iface, 0, 0, 1) < 0) {
        LeaveCriticalSection(&g_lock);
        return 0;
    }
    wr32(obj, 0x1C, 1);
    LeaveCriticalSection(&g_lock);
    return 1;
}

// Host seams for 0x66F0's imports.
struct D2SoundPlayEnv {
    uint32_t (*fogReady)(void);                                                         // Fog#10102 (0x5F6E)
    void (*fogFail)(void);                                                              // Fog#10103 (0x5F74)
    int32_t (*storm255)(void*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t); // Storm#255 (0x5F3E)
};

// 1.13c D2sound+0x066F0
// Start the stream under the lock via Storm#255(obj,+4,flags,volume*4,max,max,*obj); on success +0x48=+4, +0x1C=1, +0x30=-1, return 1.
uint32_t D2Sound_66F0(void* obj, int32_t volume, int32_t loop, const D2SoundPlayEnv* pe) {
    if (!obj) d2sound_fatal(0x3DB);
    if (!pe->fogReady()) return 0;
    EnterCriticalSection(&g_lock);
    uint32_t f4 = rd32(obj, 4);
    if (pe->storm255(obj, f4, loop ? 0x40000u : 0u, (uint32_t)volume * 4, 0x7FFFFFFFu, 0x7FFFFFFFu, rd32(obj, 0))) {
        wr32(obj, 0x48, f4);
        wr32(obj, 0x1C, 1);
        wr32(obj, 0x30, 0xFFFFFFFFu);
        LeaveCriticalSection(&g_lock);
        return 1;
    }
    pe->fogFail();
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c D2sound+0x06680
// Release the stream's backing object: under the lock, if +0x48 is set, hand it to Fog ordinal 257
// (free), call interface method +0x48, run Fog ordinal 10103, then clear +0x48 and +0x1C.
// The two ordinals are host seams (freeBacking takes +0x48; afterRelease takes nothing).
void D2Sound_6680(void* obj, void (*freeBacking)(uint32_t), void (*afterRelease)(void)) {
    if (!obj || !rd32(obj, 0)) d2sound_fatal(0x400);
    EnterCriticalSection(&g_lock);
    if (rd32(obj, 0x48)) {
        freeBacking(rd32(obj, 0x48));
        void* iface = rdptr(obj, 0);
        void** vt = *(void***)iface;
        ((void (*)(void*))vt[0x48 / 4])(iface);
        afterRelease();
        wr32(obj, 0x48, 0);
        wr32(obj, 0x1C, 0);
    }
    LeaveCriticalSection(&g_lock);
}

// 1.13c D2sound+0x06950  (Ordinal 10044; ECX = stream record) -> record+0x1C, fatal if null or +0 == 0.
uint32_t d2sound_6950(const void* s) {
    if (s && rd32(s, 0)) return rd32(s, 0x1C);
    d2sound_fatal(0x2a5);
    return 0;
}

// 1.13c D2sound+0x067F0  (ECX = record, EDX = value) -> record+0x44 = value; fatal if record null.
void D2Sound_67F0(void* rec, uint32_t v) {
    if (!rec) d2sound_fatal(0x33a);
    wr32(rec, 0x44, v);
}

// 1.13c D2sound+0x07D00
// Set the music-stop flag; on a 0 -> nonzero edge clear every played flag (0x7BC0) and arm the first pick.
void D2Sound_7D00(uint32_t on, void (*resetPlayed)(void)) {
    if (on && !g_stopFlag) { resetPlayed(); g_firstPick = 1; }
    g_stopFlag = on;
}

// ---- batch b47: list/query helpers and the stream create/queue paths -----------------------------
// Extra seams: 0x7090 (stream setup, nonzero = ok), 0x72D0 (buffer fill), 0x6F30's caller env.
struct D2SoundEnvB {
    const D2SoundEnv* base;
    const D2SoundPlayEnv* play;
    uint32_t (*setup)(void* obj, uint32_t flags);                  // 0x7090
    void (*fill)(void* a, uint32_t b, void* c);                    // 0x72D0 (called (0,0,stream+4))
};
static uint32_t g_dsAvail;             // 0x6F9C6590  tested by 0x7740
static uint32_t g_tag6584;             // 0x6F9C6584  last 0x7740 argument
static uint8_t* g_pending;             // 0x6F9C65BC  queue of {name, ..., +0x50 data, +0x54 size, +0x58 id, +0x5C next}

// 1.13c D2sound+0x06EC0 (Ordinal 10054)
// Call interface method +0x24 of the stream's +0x00 object under the lock. The slot is stdcall with two words:
// the iface and &local, where local starts as the ECX (the stream pointer) and the slot writes it. Success
// (eax >= 0) returns the low bit of local (`movzx eax, byte [esp+4]; and eax, 1`); failure returns 0.
uint32_t D2Sound_6EC0(void* obj) {
    if (!obj || !rd32(obj, 0)) d2sound_fatal(0x2B0);
    EnterCriticalSection(&g_lock);
    void* iface = rdptr(obj, 0);
    void** vt = *(void***)iface;
    uint32_t local = (uint32_t)(uintptr_t)obj;
    int32_t hr = ((int32_t (*)(void*, uint32_t*))vt[0x24 / 4])(iface, &local);
    LeaveCriticalSection(&g_lock);
    return hr < 0 ? 0u : (local & 1u);
}

// 1.13c D2sound+0x07B10 (Ordinal 10057)
uint32_t D2Sound_7B10(void) { return g_primary && rd32(g_primary, 0x48) ? 1u : 0u; }

// D2sound 0x06B60 (1.13c) (Ordinal 10064)
uint32_t D2Sound_6B60(void) { return g_soundOn ? g_masterVolume : 0u; }

// D2sound 0x086C0 (1.13c) (Ordinal 10062)
// Unlink the first node holding `value` and free it.
void D2Sound_86C0(D2SoundNode** list, uint32_t value) {
    D2SoundNode *prev = nullptr, *n = *list;
    for (; n; prev = n, n = n->next) {
        if (n->value != value) continue;
        if (n == *list) *list = n->next;
        if (prev) prev->next = n->next;
        std::free(n);
        return;
    }
}

// D2sound 0x08920 (1.13c) (Ordinal 10063)
// Free every node (retail re-walks from the head for the head's value each pass).
void D2Sound_8920(D2SoundNode** list) {
    while (*list) { D2SoundNode* h = *list; D2Sound_86C0(list, h->value); }
}

// 1.13c D2sound+0x07480 (Ordinal 10061)
// Set the master volume (0..0xFF, fatal outside) and re-apply it to every listed stream's +0x38 level.
void D2Sound_7480(uint32_t v, const D2SoundEnv* env) {
    if (!g_soundOn || (int32_t)v == (int32_t)g_masterVolume) return;
    if ((int32_t)v < 0 || (int32_t)v > 0xFF) d2sound_fatal(0x3B9);
    EnterCriticalSection(&g_lock);
    g_masterVolume = v;
    for (D2SoundNode* n = (D2SoundNode*)g_list; n; n = n->next) {
        void* s = (void*)(uintptr_t)n->value;
        if (!s) d2sound_fatal(0x2E4);
        env->setVolume(s, rd32(s, 0x38));
    }
    LeaveCriticalSection(&g_lock);
}

// D2sound 0x07740 (1.13c) (Ordinal 10065)
// Allocate a zeroed 0x60-byte stream, run 0x7090 on it, and push it on the global list.
void* D2Sound_7740(uint32_t flags, uint32_t tag, const D2SoundEnvB* e) {
    g_tag6584 = tag;
    if (((flags & 2) && !g_dsAvail) || ((flags & 2) && (flags & 1))) return nullptr;
    void* s = std::calloc(1, 0x60);
    if (!e->setup(s, flags)) { std::free(s); return nullptr; }
    EnterCriticalSection(&g_lock);
    D2SoundNode* n = (D2SoundNode*)std::malloc(sizeof(D2SoundNode));
    n->value = (uint32_t)(uintptr_t)s;
    n->next = (D2SoundNode*)g_list;
    g_list = n;
    LeaveCriticalSection(&g_lock);
    return s;
}

// D2sound 0x07C60 (1.13c) (Ordinal 10068)
// Restart the primary stream from 0 at volume 0x6E when it is idle; 1 on success.
uint32_t D2Sound_7C60(int32_t enable, int32_t loop, const D2SoundEnvB* e) {
    if (g_primary && g_soundOn && g_masterVolume && rd32(g_primary, 0x48) == 0 && enable) {
        D2Sound_7650(g_primary, e->base);
        e->base->setVolume(g_primary, 0x6E);
        if (D2Sound_66F0(g_primary, 0, loop, e->play)) return 1;
    }
    return 0;
}

// 1.13c D2sound+0x07580
// Bind a WAVE image to the stream: parse the header, and if the payload is longer than +0x44 store
// (len, fmt) at +0x18/+0x14, refill, mark +0x28, and set the +0x30 cursor. Fatal on any failure.
// `sd`/`wavd` are retail 32-bit pointers (guest dwords).
extern "C" int32_t* D2sound_8980(int32_t* riff, uint32_t* size, uint16_t* wfx);
uint32_t D2Sound_72D0(uint32_t sd, uint32_t pos, uint32_t lockOff, uint32_t bytes);
uint32_t D2Sound_7580(uint32_t sd, uint32_t wavd, uint32_t size) {
    void* s = (void*)(uintptr_t)sd;
    if (!s || !rd32(s, 0) || !wavd || !size) d2sound_fatal(599);
    EnterCriticalSection(&g_lock);
    uint16_t fmt[10];
    uint32_t len = size;
    int32_t* data = D2sound_8980((int32_t*)(uintptr_t)wavd, &len, fmt);
    if (rd32(s, 0x44) >= len) d2sound_fatal(0x25F);
    wr32(s, 0x18, len);
    wr32(s, 0x14, (uint32_t)(uintptr_t)data);
    D2Sound_72D0(sd, 0, 0, rd32(s, 4));
    wr32(s, 0x28, 1);
    wr32(s, 0x30, (!rd32(s, 0x40) && rd32(s, 0x18) < rd32(s, 4)) ? rd32(s, 0x18) : 0xFFFFFFFFu);
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c D2sound+0x08200
// Find the queued entry named `name`, then hand its sound to the first of the 8 streams whose id
// matches the entry's and which is idle (+0x1C == 0): reset, bind (0x7580), volume 0xFF, play-state 0x6F30.
void D2Sound_8200(const uint8_t* name, const D2SoundEnvB* e, uint32_t (*after6F30)(void*)) {
    if (!g_initialized) return;
    EnterCriticalSection(&g_initLock);
    uint8_t* ent = g_pending;
    while (ent && std::strcmp((const char*)ent, (const char*)name) != 0) ent = *(uint8_t**)(ent + 0x5C);
    if (ent) {
        for (int i = 0; i < 8; ++i) {
            void* s = g_streams[i];
            if (rd32(s, 0x34) != rd32(ent, 0x58)) continue;   // +0x34 is retail's piVar2[0xd]
            if (!s || !rd32(s, 0)) d2sound_fatal(0x2A5);
            if (rd32(s, 0x1C) == 0) {
                D2Sound_7650(s, e->base);
                D2Sound_7580((uint32_t)(uintptr_t)s, rd32(ent, 0x50), rd32(ent, 0x54));
                e->base->setVolume(s, 0xFF);
                after6F30(s);
                break;
            }
        }
    }
    LeaveCriticalSection(&g_initLock);
}

// 1.13c D2sound+0x072D0
// Stream refill. Retail takes the stream in EBX and (pos, lockOffset, bytes) on the stack. Locks
// `bytes` of the buffer object (+0x00, method +0x2C), copies from data(+0x14) at `pos` wrapping at
// +0x18 back to +0x44, zero-pads a non-looping tail up to the +0x30 limit (compared against
// lockOffset + bytes copied, retail keeps that running sum in the lockOffset slot), unlocks (+0x4C).
// `s` is the retail 32-bit stream record: its words are guest dwords, never host pointers.
uint32_t D2Sound_72D0(uint32_t sd, uint32_t pos, uint32_t lockOff, uint32_t bytes) {
    void* s = (void*)(uintptr_t)sd;
    if (!s || !rd32(s, 0) || !rd32(s, 0x14) || !bytes) d2sound_fatal(0xDD);
    EnterCriticalSection(&g_lock);
    const uint32_t iface = rd32(s, 0);
    const uint32_t vt = rd32((const void*)(uintptr_t)iface, 0);
    uint32_t p1 = 0, len1 = bytes;
    typedef int32_t (*LockFn)(uint32_t, uint32_t, uint32_t, uint32_t*, uint32_t*, uint32_t, uint32_t, uint32_t);
    typedef int32_t (*UnlockFn)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    if (((LockFn)(uintptr_t)rd32((const void*)(uintptr_t)vt, 0x2C))(iface, lockOff, bytes, &p1, &len1, 0, 0, 0) < 0) {
        LeaveCriticalSection(&g_lock);
        return 0;
    }
    uint8_t* d = (uint8_t*)(uintptr_t)p1;
    uint32_t n = bytes;
    for (;;) {
        uint32_t end = rd32(s, 0x18);
        if (pos >= end) { uint32_t b = rd32(s, 0x44); pos = (pos - b) % (end - b) + b; }
        uint32_t room = end - pos;
        uint32_t c = (int32_t)n < (int32_t)room ? n : room;
        std::memcpy(d, (const uint8_t*)(uintptr_t)rd32(s, 0x14) + pos, c);
        d += c; wr32(s, 0x24, rd32(s, 0x24) + c); n -= c; lockOff += c; pos += c;
        if (rd32(s, 0x40) == 0) break;
        if (n == 0) goto done;
    }
    if (n) {
        uint32_t lim = rd32(s, 0x30);
        if (lim != 0xFFFFFFFFu) { if (lockOff >= lim) goto done; n = lim - lockOff; }
        if (n) std::memset(d, 0, n);
    }
done:
    ((UnlockFn)(uintptr_t)rd32((const void*)(uintptr_t)vt, 0x4C))(iface, p1, len1, 0, 0);
    if (rd32(s, 0x40) == 0 && rd32(s, 0x24) >= rd32(s, 0x18)) wr32(s, 0x2C, 1);
    LeaveCriticalSection(&g_lock);
    return 1;
}

// ---- the 12 formerly STALLED rows: closest faithful bodies; DirectSound/COM objects are the retail
// vtable-called interfaces held in the stream records, the worker thread/Ordinal_256 teardown is a host seam.
static float   g_mixF;                 // scratch for the x87 stack hand-off
static void*   g_com[5];               // 0x6F9C64B8, 0x6F9C6524, 0x6F9C6528, 0x6F9C652C, 0x6F9C658C (IDirectSound)
static uint32_t g_flag6588, g_cs65a0;  // 0x6F9C6588, 0x6F9C65A0 (g_lock initialised)
struct D2SoundWav { char name[0x50]; void* data; uint32_t size; uint32_t stereo; D2SoundWav* next; };   // Fog #10042, 0x8A bytes
static D2SoundWav* g_wavList;          // 0x6F9C65BC

static void com_release(void*& o) {
    if (!o) return;
    void** vt = *(void***)o;
    ((void (*)(void*))vt[2])(o);
    o = nullptr;
}

// 1.13c D2Sound+0x6CE0 (thiscall, float on the stack, ret 4)
// Store the float at +0x3C. With an interface at +0x10, mode 2 and (flags bit 2 set, or value == 0.0f): property 7 of GUID 0x6F9C0208 gets
// trunc(0x8830(0, 1, 1.0f - value)).
void D2Sound_6CE0(void* obj, float v, const D2SoundEnv* env) {
    if (!obj) d2sound_fatal(0x345);
    const uint32_t iface = rd32(obj, 0x10);
    std::memcpy((uint8_t*)obj + 0x3C, &v, 4);
    if (iface && g_ds3d == 2 && ((rd8(obj, 0x34) & 4) || v == g_c0978)) {
        const float c = g_c0974 - v;
        env->attenuate(0.0f, 1.0f, c);
        uint32_t mixed = (uint32_t)env->mixValue();
        d2sound_prop32(iface, kGuid0208, 7, &mixed, 4);
    }
}

// 1.13c D2Sound+0x6FA0
// Destroy a stream record: stop it (0x6980), release its three COM objects (+0x10, +0xC, +0), zero the
// 0x60-byte record, and unlink + free its node from the global list.
void D2Sound_6FA0(void* obj, const D2SoundEnv* env) {
    if (!obj) d2sound_fatal(0x211);
    EnterCriticalSection(&g_lock);
    D2Sound_6980(obj, env);
    for (uint32_t off : {0x10u, 0xCu, 0u}) {
        void* c = rdptr(obj, off);
        if (c) { void** vt = *(void***)c; ((void (*)(void*))vt[2])(c); std::memset((uint8_t*)obj + off, 0, sizeof(void*)); }
    }
    std::memset(obj, 0, 0x60);
    D2SoundNode* prev = nullptr;
    for (D2SoundNode* n = (D2SoundNode*)g_list; n; prev = n, n = n->next) {
        if ((uintptr_t)n->value != (uintptr_t)obj) continue;
        if (g_list == n) g_list = n->next;
        if (prev) prev->next = n->next;
        std::free(n);
        break;
    }
    LeaveCriticalSection(&g_lock);
}

// 1.13c D2Sound+0x8020
// Stop every one of the 8 global streams (release the backing handle, interface method +0x48, clear +0x1C).
void D2Sound_8020(const D2SoundEnv* env) {
    if (!g_initialized) return;
    for (int i = 0; i < 8; ++i) {
        void* p = g_streams[i];
        if (!p || !rd32(p, 0)) d2sound_fatal(0x292);
        EnterCriticalSection(&g_lock);
        if (rd32(p, 0x48)) env->releaseStream(p);
        void* iface = rdptr(p, 0);
        void** vt = *(void***)iface;
        ((void (*)(void*))vt[0x48 / 4])(iface);
        wr32(p, 0x1C, 0);
        LeaveCriticalSection(&g_lock);
    }
}

// 1.13c D2Sound+0x8320
// Register a 22050 Hz WAV (loaded by 0x6360, parsed by 0x8980) under `name` at the head of the wave list.
// `esiFlags` is the caller's ESI (high word == 2 marks stereo); the decompile drops it.
void D2Sound_8320(const char* name, uint32_t esiFlags, const D2SoundEnv* env) {
    if (!g_initialized) return;
    int32_t size = 0;
    int32_t* wav = (int32_t*)env->loadFile(nullptr, &size);
    if (!wav) return;
    uint32_t len = (uint32_t)size;
    uint16_t fmt[9];
    D2Sound_8980(wav, &len, fmt, env);
    int32_t rate; std::memcpy(&rate, (uint8_t*)fmt + 4, 4);
    if (rate != 0x5622) return;
    D2SoundWav* w = (D2SoundWav*)std::calloc(1, sizeof(D2SoundWav));
    w->next = g_wavList; g_wavList = w;
    w->data = wav; w->size = (uint32_t)size;
    w->stereo = (uint16_t)(esiFlags >> 16) == 2;
    std::strncpy(w->name, name, sizeof w->name - 1);
}

// 1.13c D2Sound+0x8B50
// Distance + pan to (left, right) 0..255 gains; the x87 runs at the CRT's 53-bit precision.
// d = sqrt((x*x + y*y) + z*z), clamped to 100; v = (float)(ln(d/2) * 1.4426950f * 6) when d >= 2, else 0;
// left = _ftol(255 / pow(10, v * 0.05f)), right = _ftol(x * 127 * 0.8f + 128), each clamped to 0..255.
// _ftol (0xACCC) truncates toward zero: fistp m64, the low dword.
static int32_t d2sound_ftol(double v) {
    if (!(v > -9.2e18 && v < 9.2e18)) return 0;   // the integer indefinite 0x8000000000000000: low dword 0
    return (int32_t)(uint32_t)(uint64_t)(int64_t)v;
}
void D2Sound_8B50(int32_t* left, int32_t* right, float x, float y, float z) {
    double d = std::sqrt(((double)x * x + (double)y * y) + (double)z * z);
    float v = 0.0f;
    if (d > 100.0) d = 100.0;
    if (!(d < 2.0)) v = (float)(std::log(d * 0.5) * (double)1.44269502f * 6.0);
    double g = 255.0 / std::pow(10.0, (double)v * (double)0.05f);
    int32_t l = d2sound_ftol(g);
    int32_t L = l <= 0 ? 0 : l < 0xFF ? l : 0xFF;
    int32_t r = d2sound_ftol((double)x * 127.0 * (double)0.8f + 128.0);
    *left = L;
    *right = r > 0 ? (r < 0xFF ? r : 0xFF) : 0;
}

// 1.13c D2Sound+0x9230
// Shutdown (ordinal 10031): drop master volume and re-apply every list entry's volume, destroy the primary
// stream, join the worker thread, release the COM objects, delete the lock.
void D2Sound_9230(const D2SoundEnv* env) {
    g_flag6588 = 0;
    if (g_com[4] && g_masterVolume) {
        EnterCriticalSection(&g_lock);
        g_masterVolume = 0;
        for (D2SoundNode* n = (D2SoundNode*)g_list; n; n = n->next) {
            void* p = (void*)(uintptr_t)n->value;
            if (!p) d2sound_fatal(0x2E4);
            env->setVolume(p, rd32(p, 0x38));
        }
        LeaveCriticalSection(&g_lock);
    }
    if (g_primary) { D2Sound_6FA0(g_primary, env); g_primary = nullptr; }
    env->shutdownWorker();
    for (int i = 0; i < 5; ++i) com_release(g_com[i]);
    g_soundOn = 0;
    if (g_cs65a0) { DeleteCriticalSection(&g_lock); g_cs65a0 = 0; }
}

// ---- FN-probe wrappers: they build the host seams a vectored path needs (every other seam stays null, so a
// descriptor that reached an unmapped callee would crash the replay instead of silently passing) ----------
void D2Sound_fnRun7510(uint32_t percent) {
    D2SoundEnv e{};
    e.setMasterVolume = [](uint32_t v) { D2Sound_7480(v, nullptr); };   // empty stream list: the env is never read
    D2Sound_7510(percent, &e);
}
void D2Sound_fnRun7D00(uint32_t on) { D2Sound_7D00(on, [] {}); }        // pinned track count 0: retail's 0x7BC0 changes nothing
void D2Sound_fnRun7FE0(void) { D2SoundEnv e{}; D2Sound_7FE0(&e); }
void D2Sound_fnRun8020(void) { D2SoundEnv e{}; D2Sound_8020(&e); }
uint32_t D2Sound_fnRun7D30(void) { D2SoundPickEnv pe{}; return D2Sound_7D30(&pe); }
void D2Sound_fnSetPending(void* p) { g_pending = (uint8_t*)p; }
static uint32_t d2sound_fnAfter6F30(void*) { return 0; }
void D2Sound_fnRun8200(const uint8_t* name) { D2SoundEnvB e{}; D2Sound_8200(name, &e, d2sound_fnAfter6F30); }
uint32_t D2Sound_fnRun7C60(int32_t enable, int32_t loop) { D2SoundEnvB e{}; return D2Sound_7C60(enable, loop, &e); }

// FN probe seam for 0x8320: the 0x6360 file load answers "no file", so only the init gate and the null-load return run.
static void* d2sound_fnNoFile(void*, int32_t*) { return nullptr; }
void D2Sound_fnRun8320(const char* name, uint32_t esiFlags) { D2SoundEnv e{}; e.loadFile = d2sound_fnNoFile; D2Sound_8320(name, esiFlags, &e); }
// FN probe seam for 0x83C0: the gate (sound off, or already initialised) never reaches the env.
void D2Sound_fnRun83C0() { D2SoundEnv e{}; D2Sound_83C0(&e); }
