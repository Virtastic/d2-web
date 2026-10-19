// Prior art: D2MOO D2WinMain.cpp D2Win_10019 (1.10f). Cross-checked
// against the cached 1.13c decompile and x86 instructions: the bottom tile
// and dim rectangle use WINDOW_GetDimensions outputs, lost by Ghidra.
#include <cstddef>
#include <cstdint>

namespace {
// Serialized control header; pointer and callback words remain retail handles.
struct Control32 {
    uint32_t type, cell, flags, x, y, width, height;
    uint32_t draw, initialize;
    uint32_t rest[6];
    uint32_t next;
};
static_assert(offsetof(Control32, flags) == 8);
static_assert(offsetof(Control32, draw) == 0x1c);
static_assert(offsetof(Control32, initialize) == 0x20);
static_assert(offsetof(Control32, next) == 0x3c);
struct GfxData32 { uint32_t words[18]; };
static_assert(sizeof(GfxData32) == 0x48);
}

// Explicit state for retail globals 9014A0/9014BC/901494/9A9CEC.
// The owner may update it during callbacks (including changing the list head).
struct D2WinDrawState {
    uint32_t head, customWindowProc, backgroundCell, dimmed;
};
struct D2WinDrawServices {
    uint32_t (*windowState)();                       // D2Gfx #10043
    const void* (*resolveControl)(uint32_t handle);  // retail data -> native data
    void (*invokeControl)(uint32_t callback, uint32_t control);
    void (*dimensions)(int32_t* width, int32_t* height); // #10080
    int32_t (*startDraw)(int32_t, int32_t, int32_t, int32_t); // #10085
    void (*clearScreen)(uint32_t);                  // #10021
    void (*celDraw)(const void*, int32_t, int32_t, int32_t, int32_t, int32_t);
    void (*drawAuxiliary)();                        // D2Win 0x18750
    void (*drawBox)(int32_t, int32_t, int32_t, int32_t, uint32_t, uint8_t);
    void (*drawCursor)();                           // D2Win 0x17F40
    void (*endScene)();                             // D2Gfx #10040
    void (*pump)();                                 // D2Win 0x17970
};

// 1.13c D2Win+0x187E0
// Required services are the real import/callee boundaries, with no default
// no-ops. Every pass reads next AFTER invoking its callback, as retail does.
extern "C" void d2win_sub_187E0(D2WinDrawState* state, const D2WinDrawServices* api) {
    if (api->windowState()) return;
    auto control = [&](uint32_t handle) {
        return static_cast<const Control32*>(api->resolveControl(handle));
    };
    for (uint32_t h = state->head; h;) {
        if (control(h)->initialize) api->invokeControl(control(h)->initialize, h);
        h = control(h)->next;
    }
    if (!state->customWindowProc) {
        int32_t width, height;
        api->dimensions(&width, &height);
        if (api->startDraw(0, 0, 0, 0)) {
            api->clearScreen(0);
            if (state->backgroundCell) {
                GfxData32 data{};
                data.words[13] = state->backgroundCell; // +0x34 pCellFile
                for (int32_t y = 256; y < 768; y += 256) {
                    for (int32_t x = 0; x < 1024; x += 256) {
                        api->celDraw(&data, x, y, -1, 5, 0);
                        ++data.words[0];
                    }
                }
                for (int32_t x = 0; x < 1024; x += 256) {
                    api->celDraw(&data, x, static_cast<int32_t>(uint32_t(height) - 1), -1, 5, 0);
                    ++data.words[0];
                }
            }
            auto drawPass = [&](bool foreground) {
                for (uint32_t h = state->head; h;) {
                    const Control32* c = control(h);
                    if (c->draw && bool(c->flags & 8u) == foreground)
                        api->invokeControl(c->draw, h);
                    h = control(h)->next;
                }
            };
            drawPass(false);
            api->drawAuxiliary();
            if (state->dimmed) api->drawBox(0, 0, width, height, 0, 0x7f);
            drawPass(true);
            api->drawCursor();
            api->endScene();
        }
        api->pump();
    }
}
