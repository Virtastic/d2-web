// The D2Client host runtime, key-binding part: the 114-record bind table and the 57-record command table the
// 1.13c code (D2Client+0x38AD0 scan, +0x38CD0 bindable test, +0x3A270 assign) reads, held here and filled by the shell from
// the player's Configure Controls table. The shell resolves a key press and assigns a key through these exports.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "d2client_batch_38410.h"
using namespace d2client_batch_38410;
namespace {
struct Host final : Services {
    int32_t lastCmd = -1;
    State st{*this};
    bool badCodePtr(uint32_t) override { return false; }
    void callHandler(uint32_t c) override { lastCmd = int32_t(c); }
    void ord511(void*) override {}
    [[noreturn]] void fatal(int32_t) override { __builtin_trap(); }
    bool f2060() override { return true; } bool f2030() override { return true; }
    bool f20e0() override { return true; } bool f2090() override { return true; }
    void ord10003(uint32_t) override {} uint8_t jumpByte(uint32_t) override { return 0; }
    void f391e0(uint32_t, int32_t) override {}
    void ord(int32_t, uint32_t, uint32_t, uint32_t) override {}
    void sendClose(void*) override {} void destroyObj(void*) override {}
    void ord10043(int32_t, int32_t) override {} void ord10065(int32_t) override {}
    int32_t listFirst() override { return 0; } int16_t* listData(int32_t) override { return nullptr; }
    int32_t listCount(int32_t) override { return 0; } int32_t listNext(int32_t) override { return 0; }
};
// Retail's own tables (D2Client .data, read from the 1.13c image). Row -> command of the expansion list at +0xF8928 (62 rows,
// 0x39 a blank sentinel), the table the 62-slot Configure Controls screen walks; the flags of the 56 commands at +0xF4F10
// ({code @+0, handler @+4, flag @+8}): the flag bit set below, and whether +0 is null (only command 0x2A: Print Screen binds nowhere else).
const uint8_t kRowCmd[62] = {0,1,2,0x36,3,4,6,0x39,0xc,0xd,0xe,0xf,0x10,0x11,0x12,0x13,0x14,0x15,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,
    0x27,0x28,0x39,0x16,0x17,0x18,0x19,0x1a,0x2c,0x39,5,0x22,0x23,0x24,0x25,0x2b,0x39,7,8,9,0xa,0xb,0x2d,0x39,
    0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x37,0x39,0x2a,0x26,0x29};
constexpr uint64_t kFlag = (1ull<<5)|(1ull<<0x16)|(0x7full<<0x1b)|(1ull<<0x29)|(1ull<<0x2a)|(1ull<<0x2c)|(1ull<<0x2d)|(1ull<<0x37);
Host& host() { static Host h; return h; }
uint16_t g_err = 0;
}
extern "C" {
// The retail command a Configure Controls row (0..61, expansion list) binds, -1 for a blank sentinel row.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_bind_row_cmd(int32_t row) { return unsigned(row) < 62 && kRowCmd[row] != 0x39 ? kRowCmd[row] : -1; }
// 1.13c D2Client+0x38CD0
EMSCRIPTEN_KEEPALIVE int32_t d2_client_key_bindable(int32_t vk) { return sub_38CD0(uint32_t(vk)) ? 1 : 0; }
// Clear both tables; every command gets a live handler slot (the shell dispatches by name, so the slot only has to be nonzero).
EMSCRIPTEN_KEEPALIVE void d2_client_bind_reset() {
    State& s = host().st;
    for (auto& b : s.binds) { b.cmd = 0xffff; b.key = 0xffff; b.mod = 0; }
    for (unsigned i = 0; i < 0x38; i++) s.cmds[i] = Cmd{i == 0x2a ? 0u : 1u, 1, uint32_t((kFlag >> i) & 1)};
}
EMSCRIPTEN_KEEPALIVE void d2_client_bind_put(int32_t idx, int32_t cmd, int32_t vk, int32_t mod) {
    if (unsigned(idx) >= kBinds) return;
    Bind& b = host().st.binds[idx]; b.cmd = cmd; b.key = uint16_t(vk); b.mod = mod;
}
EMSCRIPTEN_KEEPALIVE int32_t d2_client_bind_get(int32_t idx, int32_t field) {
    if (unsigned(idx) >= kBinds) return -1;
    const Bind& b = host().st.binds[idx];
    return field == 0 ? b.cmd : field == 1 ? int32_t(b.key) : b.mod;
}
// 1.13c D2Client+0x38AD0: the command the key press runs, -1 when no bind answers.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_bind_scan(int32_t vk) {
    Host& h = host(); alignas(8) uint8_t msg[0x20] = {}; uint32_t k = uint32_t(vk);
    std::memcpy(msg + 8, &k, 4); h.lastCmd = -1; sub_38AD0(h.st, msg); return h.lastCmd;
}
// 1.13c D2Client+0x3A270: bind vk to (cmd, mod); 1 on success, else 0 with the refusal string id in d2_client_bind_error.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_bind_assign(int32_t cmd, int32_t mod, int32_t vk) {
    g_err = 0; return sub_3A270(host().st, cmd, mod, int16_t(vk), &g_err) ? 1 : 0;
}
EMSCRIPTEN_KEEPALIVE int32_t d2_client_bind_error() { return g_err; }
}
