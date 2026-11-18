// Completed members of the held 149A0 Stage A batch. Remaining rows are
// deliberately unmarked: their unresolved ABI is recorded in the commit.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

// Explicit native adapters for the imported error and operating-system services.
// Callers supply concrete implementations; there is no RVA dispatcher or fallback.
struct D2ClientCursorServices {
    // Fog #10265 (RVA FC20): no arguments, returns caller instruction pointer.
    uint32_t (*caller_instruction_pointer)();
    // Native adapter for Fog #10024 (RVA 11100), retail cdecl:
    // three stack arguments: message, instruction pointer, source line.
    void (*assert_error)(uint32_t message, uint32_t instruction_pointer, uint32_t line);
    int32_t (*display_mode)();
    uintptr_t (*window)();
    void (*window_rect)(uintptr_t, int32_t*);
    int32_t (*system_metric)(int32_t);
    void (*set_cursor)(int32_t, int32_t);
};
[[noreturn]] static void cursor_error(const D2ClientCursorServices& api, uint32_t line) {
    const uint32_t instruction_pointer = api.caller_instruction_pointer();
    api.assert_error(0x6fb8359fu, instruction_pointer, line);
    std::_Exit(-1);
}
struct D2ClientCursorEntry {
    uint32_t value;
    int32_t delay;
    uint8_t remainder[20];
};
static_assert(sizeof(D2ClientCursorEntry) == 0x1c);
static_assert(offsetof(D2ClientCursorEntry, delay) == 4);
struct D2ClientCursorState {
    int32_t mode;
    int32_t selection;
    int32_t countdown;
    uint32_t saved_argument;
    uint32_t active;
    uint32_t last_tick;
    int32_t x, y;
};
// 1.13c D2Client+0x160E0
// Table points at global RVA F855C. Signed negative indices are not rejected
// by retail; the supplied table image must include that addressable prefix.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_160e0(int32_t eax, const D2ClientCursorEntry* table,
    const D2ClientCursorState* state, const D2ClientCursorServices* api) {
    if (state->mode != 7) cursor_error(*api, 0x9a);
    if (eax >= 7) cursor_error(*api, 0x9b);
    return static_cast<int32_t>(static_cast<uint32_t>(table[eax].delay) << 8);
}
// 1.13c D2Client+0x16130
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_16130(int32_t eax, const D2ClientCursorEntry* table,
    const D2ClientCursorState* state, const D2ClientCursorServices* api) {
    if (state->mode != 7) cursor_error(*api, 0x8f);
    if (eax >= 7) cursor_error(*api, 0x90);
    return table[eax].value;
}
// 1.13c D2Client+0x161B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_161b0(int32_t ebx, int32_t edi,
    D2ClientCursorState* state, const D2ClientCursorServices* api) {
    if (ebx < 0) cursor_error(*api, 0x307);
    if (edi < 0) cursor_error(*api, 0x308);
    if (api->display_mode() == 0) {
        int32_t rect[4];
        api->window_rect(api->window(), rect);
        const int32_t title = api->system_metric(4);
        const int32_t border = api->system_metric(7);
        const auto x = static_cast<int32_t>(uint32_t(border) + uint32_t(rect[0]) + uint32_t(ebx));
        const auto y = static_cast<int32_t>(uint32_t(title) + uint32_t(rect[1]) + uint32_t(edi));
        api->set_cursor(x, y);
    } else {
        api->set_cursor(ebx, edi);
    }
    state->x = ebx;
    state->y = edi;
}
// 1.13c D2Client+0x162D0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_162d0(uint32_t eax, D2ClientCursorState* state) {
    state->saved_argument = eax;
    state->countdown = 0;
    state->selection = 5;
    state->active = eax ? 4 : 1;
}
struct D2ClientMouseMessage {
    uint8_t prefix[12];
    uint16_t x, y;
    uint8_t middle[8];
    uint32_t result;
};
static_assert(offsetof(D2ClientMouseMessage, x) == 0xc);
static_assert(offsetof(D2ClientMouseMessage, y) == 0xe);
static_assert(offsetof(D2ClientMouseMessage, result) == 0x18);
// 1.13c D2Client+0x16300
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_16300(D2ClientMouseMessage* message,
    D2ClientCursorState* state, uint32_t (*get_tick)()) {
    state->x = message->x;
    state->y = message->y;
    state->last_tick = get_tick();
    if (state->active == 5) {
        state->selection = 5;
        message->result = 0;
        state->active = 1;
        state->countdown = 0;
        return;
    }
    message->result = 0;
}
