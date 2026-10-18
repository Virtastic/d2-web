// Prior art used: existing D2Client CRT fd-table and __read_nolock implementations.
// D2MOO has no D2Client module; control flow follows the cached 1.13c body.
#include <cstdint>
#include <emscripten.h>

extern uint32_t g_fdcount; // +0x123990 _nfile, owned by d2client_batch_b56.cpp
extern uint32_t g_pioinfo[0x20]; // +0x1239A0, 32 records per block, stride 0x24
extern "C" int32_t* d2_client_579c();
extern "C" int32_t* d2_client_57a5();
struct D2ClientReadNolockEnv;
extern "C" int32_t d2_client_81a6(uint32_t, char*, uint32_t,
                                  const D2ClientReadNolockEnv*);
struct D2ClientReadEnv {
    void (*lock_fhandle)(uint32_t); // +0xA67B
    void (*unlock_fhandle)();       // +0x840A finally fragment
    const D2ClientReadNolockEnv* read;
};

static int32_t bad_descriptor() {
    *d2_client_579c() = 9;
    *d2_client_57a5() = 0;
    return -1;
}

// 1.13c D2Client+0x08383
// __read: reject invalid/closed descriptors before locking, recheck the same
// flags byte under the lock, and delegate to __read_nolock. Both invalid paths
// set errno 9 and clear doserrno. Retail's finally unlocks every protected exit.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_8383(
    int32_t fd, void* destination, uint32_t count, const D2ClientReadEnv* env) {
    const uint32_t descriptor = static_cast<uint32_t>(fd);
    if (descriptor >= g_fdcount) return bad_descriptor();
    auto* flags = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(
        g_pioinfo[descriptor >> 5] + (descriptor & 31u) * 0x24u + 4u));
    if ((*flags & 1u) == 0) return bad_descriptor();
    env->lock_fhandle(descriptor);
    struct Unlock {
        const D2ClientReadEnv* env;
        ~Unlock() { env->unlock_fhandle(); }
    } unlock{env};
    if ((*flags & 1u) == 0) return bad_descriptor();
    return d2_client_81a6(descriptor, static_cast<char*>(destination), count, env->read);
}
