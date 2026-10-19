// Multibyte CRT snapshot publication; host allocation/NLS/locking stay explicit.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientMbSnapshot {
    int32_t references;
    uint32_t codepage, multibyte, locale;
    uint16_t metadata[5];
    uint8_t reserved[2];
    uint8_t classification[257], casemap[256], tail[3];
};
static_assert(offsetof(D2ClientMbSnapshot, metadata) == 0x10);
static_assert(offsetof(D2ClientMbSnapshot, classification) == 0x1c);
static_assert(offsetof(D2ClientMbSnapshot, casemap) == 0x11d);
static_assert(sizeof(D2ClientMbSnapshot) == 0x220);
struct D2ClientMbPublishState {
    uint32_t pseudo_codepage, locale_codepage;
    D2ClientMbSnapshot* published;
    uint32_t codepage, multibyte, locale;
    uint16_t metadata[5];
    uint8_t classification[257], casemap[256];
};
struct D2ClientMbPublishEnv {
    void (*lock)(int32_t);
    void (*unlock)(int32_t);
    uint32_t (*get_oem_cp)();
    uint32_t (*get_ansi_cp)();
    D2ClientMbSnapshot* (*allocate)(uint32_t);
    void (*release)(D2ClientMbSnapshot*);
    // Executes 0xB343 on these globals, including its case-table tail.
    int32_t (*select)(uint32_t, D2ClientMbPublishState*);
};

// 1.13c D2Client+0x0B4D7
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2client_setmbcp_b4d7(
    int32_t requested, D2ClientMbPublishState* state, const D2ClientMbPublishEnv* env) {
    int32_t result = -1;
    env->lock(13);
    state->pseudo_codepage = 0;
    uint32_t page = static_cast<uint32_t>(requested);
    if (requested == -2 || requested == -3 || requested == -4) {
        state->pseudo_codepage = 1;
        page = requested == -2 ? env->get_oem_cp() :
               requested == -3 ? env->get_ansi_cp() : state->locale_codepage;
    }
    if (page == state->codepage) {
        result = 0;
    } else {
        D2ClientMbSnapshot* snapshot = state->published;
        if (!snapshot || snapshot->references != 0) snapshot = env->allocate(0x220);
        if (snapshot) {
            result = env->select(page, state);
            if (result == 0) {
                snapshot->references = 0;
                snapshot->codepage = state->codepage;
                snapshot->multibyte = state->multibyte;
                snapshot->locale = state->locale;
                std::memcpy(snapshot->metadata, state->metadata, sizeof snapshot->metadata);
                std::memcpy(snapshot->classification, state->classification, sizeof snapshot->classification);
                state->published = snapshot;
                std::memcpy(snapshot->casemap, state->casemap, sizeof snapshot->casemap);
            }
        }
        if (result == -1 && snapshot != state->published) env->release(snapshot);
    }
    env->unlock(13);
    return result;
}
