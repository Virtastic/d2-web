// 1.13c Storm+0x3A200: SCODE pseudocode lookup. Copies the name (bounded, 0x100 bytes)
// into a local key, upper-cases a-z, builds a compiler object on the stack (init seam, then the vtable word), compiles
// the key and copies the object's result string out, capped at min(capacity, length + 1). A miss clears the first
// output byte. The object's init, compile and destroy are seams; the object is retail's 8-dword layout, with the
// result string pointer in word 1 and its length in word 2.
#include <cstdint>

typedef uint32_t u32;
typedef uint8_t u8;

struct Storm3A200Env {
    void (*init)(u32* obj);                       // object init (stdcall 1)
    u32 (*compile)(u32* obj, u8* key, u32 zero);  // compile (stdcall 3), nonzero on success
    void (*destroy)(u32* obj);                    // object destroy (stdcall 1)
    u32 vtbl;                                     // the vtable word stored after init
};

static u32 g_err3a200;
extern "C" u32* Storm_3A200_LastError() { return &g_err3a200; }

// The bounded copy: at most n-1 characters, then a NUL; nothing written past the first NUL; n == 0 writes nothing.
static void boundedCopy(u8* d, const u8* s, u32 n) {
    if (n == 0) return;
    for (u32 i = 0; i + 1 < n; i++) if ((d[i] = s[i]) == 0) return;
    d[n - 1] = 0;
}

u32 Storm_3A200(const u8* name, u8* out, u32 cap, const Storm3A200Env& env) {
    if (name == nullptr || name[0] == 0 || out == nullptr || cap == 0) { g_err3a200 = 0x57; return 0; }
    u8 key[0x100] = {};
    boundedCopy(key, name, 0x100);
    for (u32 i = 0; i < 0x100 && key[i]; i++) if (key[i] >= 'a' && key[i] <= 'z') key[i] = (u8)(key[i] - 0x20);
    u32 obj[8];
    env.init(obj);
    obj[0] = env.vtbl;
    if (env.compile(obj, key, 0) == 0) {
        out[0] = 0;
        env.destroy(obj);
        return 0;
    }
    u32 n = obj[2] + 1u;
    if (cap < n) n = cap;
    boundedCopy(out, (const u8*)(uintptr_t)obj[1], n);
    env.destroy(obj);
    return 1;
}
