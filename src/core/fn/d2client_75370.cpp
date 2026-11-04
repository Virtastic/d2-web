// 1.13c D2Client+0x75370 probe. r[0] = string id, r[1] = x. Blob 0 = hook log, blob 1 = the pinned width (sc pairs).
// The lookup answers the pinned u"{id}"; the measure import logs the copied buffer as a wide string and returns the pinned width.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "hookenv.h"

typedef uint16_t wch;
struct Uni { void (*strcpy)(wch*, const wch*); void (*strcat)(wch*, const wch*); void (*strncpy)(wch*, const wch*, int); int (*strlen)(const wch*);
             void (*toUnicode)(wch*, const char*, int); void (*win2Unicode)(wch*, const char*, int); };
extern "C" int32_t d2_client_75370(uint32_t id, int32_t param_2, const Uni* U, const wch* (*ord10003)(uint32_t), int (*ord10028)(const wch*));

namespace {
HookEnv* he;
void cp(wch* d, const wch* s) { while ((*d++ = *s++)) {} }
const wch* lang(uint32_t id) { he->h(0xCEACC, 0, id); return (const wch*)he->wid(id); }
int width(const wch* s) { const uint32_t v = he->h(0xCEBAC, 0, he->widTag(s)); he->ws(0xCEBAC, 4, (const char16_t*)s); return (int)v; }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_75370(uint32_t* r) {
    HookEnv h((uint32_t*)(uintptr_t)r[14], (const uint32_t*)(uintptr_t)r[16]); he = &h; h.widReset();
    const Uni U = { cp, nullptr, nullptr, nullptr, nullptr, nullptr };
    r[4] = (uint32_t)d2_client_75370(r[0], (int32_t)r[1], &U, lang, width);
}
