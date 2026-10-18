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
};

// 1.13c D2Sound+0x67C0
// Is the stream active? Reads +0x48 (the same word 0x6B80 sets at +0x4C's neighbour).
uint32_t D2Sound_67C0(void* obj) {
    if (!obj) d2sound_fatal(0x37A);
    return rd32(obj, 0x48) != 0 ? 1u : 0u;
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

// 1.13c D2Sound+0x6C00
// Push a float (distance ratio / volume) into the stream's +0x10 interface. With the DS3D global at 1
// it is property 1 on kGuid01E8 and the value is passed through unless it equals 0.0f (retail then
// substitutes -1.0f); otherwise the value goes through the 0x8830 curve and 0xACCC mixer first and both
// properties 5 and 4 on kGuid0208 get the mixer's low dword.
void D2Sound_6C00(void* obj, float value, const D2SoundEnv* env) {
    if (!obj) d2sound_fatal(0x359);
    if (g_silent) value = 0.0f;
    void* iface = rdptr(obj, 0x10);
    if (!iface) return;
    if (g_ds3d == 1) {
        // Retail skips the substitution only when the compare is equal or unordered (NaN).
        if (value == value && value != g_c0978) value = -1.0f;
        d2sound_setProp(iface, kGuid01E8, 1, &value, 4);
        return;
    }
    env->attenuate(0.0f, 1.0f, value);
    uint32_t mixed = (uint32_t)env->mixValue();
    d2sound_setProp(iface, kGuid0208, 5, &mixed, 4);
    d2sound_setProp(iface, kGuid0208, 4, &mixed, 4);
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
    void* iface = rdptr(obj, 0);
    void** vt = *(void***)iface;
    ((void (*)(void*, uint32_t))vt[0x34 / 4])(iface, 0);
    void* aux = rdptr(obj, 0x0C);
    if (aux) {
        void** avt = *(void***)aux;
        ((void (*)(void*, float, uint32_t))avt[0x40 / 4])(aux, 100.0f, 0);
        ((void (*)(void*, float, uint32_t))avt[0x44 / 4])(aux, 2.0f, 0);
    }
    if (!(rd8(obj, 0x34) & 2))
        ((void (*)(void*))vt[0x40 / 4])(iface);
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
// The distance/pan curve: 1.0f above the 0.0f bound, 0.0f below -10000.0f, and pow(x * 0.0005f, 10.0)
// (0x9CF0's x87 fyl2x path) in between. Both bounds are fcomps compares, so NaN takes the pow branch.
double D2Sound_87D0(float x) {
    if (g_c0978 < x + g_c097C) return (double)g_c0974;
    if (x - g_c097C < g_c0970) return (double)g_c0978;
    return std::pow((double)(x * g_c0964), g_c0968);
}

// 1.13c D2Sound+0x8980
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
