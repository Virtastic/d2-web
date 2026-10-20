#include <cstdint>
#include <cstring>
#include <emscripten.h>

// MCP request composers sharing sub_6EC0 (append strz to running length; _exit(-1) once length >= 0x400).
// Each returns the byte count sent, or 0 where retail would _exit. 
// 1.13c D2MCPClient+0x6EC0 (ecx = src, edx = &len, edi = dst, [esp+4] = limit, ret 4): append the strz at src to
// dst, *len += strlen+1. When the new length reaches the limit retail logs (0x5DEC/0x5DE0) and _exit(-1)s (0x1187);
// here *len is already updated and nullptr is returned (the caller sends nothing, our stand-in for the fatal path).
// Otherwise the strz is copied and dst + strlen+1 -- the next write position -- comes back.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t* d2_mcp_append_strz(const char* s, uint32_t* len, uint8_t* dst, uint32_t limit) {
    uint32_t n = (uint32_t)strlen(s) + 1;
    *len += n;
    if (*len >= limit) return nullptr;
    memcpy(dst, s, n);
    return dst + n;
}
// Append strz at out+off with running length *len (the composers' limit is always 0x400); false = retail's overflow exit.
static bool app(const char* s, uint32_t* len, uint8_t* at) { return d2_mcp_append_strz(s, len, at, 0x400) != nullptr; }

// 1.13c D2MCPClient+0x6F20: { 0x16, u32 a, u32 b, strz }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6f20(const char* s, uint32_t a, uint32_t b, uint8_t* out) {
    uint32_t len = 0;
    out[0] = 0x16; memcpy(out + 1, &a, 4); memcpy(out + 5, &b, 4);
    return app(s, &len, out + 9) ? len + 9 : 0;
}
// 1.13c D2MCPClient+0x7020: { 0x0B, strz, strz }
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_7020(const char* s1, const char* s2, uint8_t* out) {
    uint32_t len = 0;
    out[0] = 0x0B;
    if (!app(s1, &len, out + 1)) return 0;
    uint32_t at = len + 1;   // (an argument expression reading len would race app's update of it)
    return app(s2, &len, out + at) ? len + 1 : 0;
}
// { op, u16 word, strz }; both store word to [0xaeb4].
static uint32_t op_word_str(uint8_t op, const char* s, uint16_t w, uint8_t* out) {
    uint32_t len = 0;
    out[0] = op; out[1] = w & 0xFF; out[2] = w >> 8;
    return app(s, &len, out + 3) ? len + 3 : 0;
}
// 1.13c D2MCPClient+0x70B0 (op 9) and +0x71D0 (op 6)
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_70b0(const char* s, uint16_t w, uint8_t* out) { return op_word_str(9, s, w, out); }
// ponytail: 71D0 has no 0xFFFF check; unreachable after the 0x400 bound, so identical.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_71d0(const char* s, uint16_t w, uint8_t* out) { return op_word_str(6, s, w, out); }
// 1.13c D2MCPClient+0x6E10: { 0x18, strz }; SStrCopy(buf+1, s, 0x400) truncates to 1023 chars, send(len = SStrLen + 2); zeroes [0xb1c8]. No word.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6e10(const char* s, uint8_t* out) {
    uint32_t n = (uint32_t)strnlen(s, 1023);
    out[0] = 0x18; memcpy(out + 1, s, n); out[1 + n] = 0;
    return n + 2;
}
// 1.13c D2MCPClient+0x6E60: { 0x0A, u16 word, strz }; the string goes through Storm SStrCopy(buf+3, s, 0x400) (truncates to 1023 chars),
// then send(len = SStrLen + 4); stores word to [0xaeb4]. No 0x400 exit.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6e60(const char* s, uint16_t w, uint8_t* out) {
    uint32_t n = (uint32_t)strnlen(s, 1023);
    out[0] = 10; out[1] = w & 0xFF; out[2] = w >> 8;
    memcpy(out + 3, s, n); out[3 + n] = 0;
    return n + 4;
}
// 1.13c D2MCPClient+0x6A30: teardown. g = { [0xb028] socket, [0xb1d0] thread handle, [0xb218] handle }. A nonzero socket is closed through the
// Fog ordinal-10069 thunk (returned here for the caller to close); a nonzero thread handle is waited on (INFINITE) and closed, then the second
// handle closed; all three globals end 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_teardown_6a30(uint32_t* g) {
    uint32_t sock = g[0];
    g[0] = g[1] = g[2] = 0;
    return sock;
}

// 1.13c D2MCPClient+0x6CF0: one-byte request { 0x13 }; no argument is read (ecx is saved and dropped); zeroes [0xb1c8], sends len 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6cf0(uint8_t* out) { out[0] = 0x13; return 1; }

// 1.13c D2MCPClient+0x65D0: keep a copy of a received MCP packet. The previous copy ([0xb220], Fog #10043 free) is dropped; len 0 stops there.
// Otherwise len bytes go to dst (Fog #10042 alloc, [0xb220] := dst) and the header is checked: opcode 0x17 needs u16 at +1,+3,+5,+7 each < 0x100;
// opcode 0x19 the same, then u16 at +7 many pairs of (4 bytes + strz + strz) are walked; any other opcode or a failing u16 logs and _exit(-1).
// Returns where retail's EAX ends as an offset from dst (the scan pointer; 0 for 0x17, the bare 9 for 0x19 with no records), or ~0 where it exits.
// Len 0: EAX is the free's result, the caller handles it. The walk reads dst past len like retail (the blob is zero there).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_store_65d0(const uint8_t* src, uint32_t len, uint8_t* dst) {
    if (!len) return 0;
    memcpy(dst, src, len);
    // retail reads past the copy (heap garbage); here bytes beyond len read as 0 so no read leaves the copy and every loop ends
    auto b = [&](uint32_t o) { return o < len ? dst[o] : 0; };
    auto w = [&](uint32_t o) { return (uint32_t)(b(o) | b(o + 1) << 8); };
    if (dst[0] != 0x17 && dst[0] != 0x19) return ~0u;
    for (int o = 1; o <= 7; o += 2) if (w(o) >= 0x100) return ~0u;
    if (dst[0] == 0x17) return 0;
    uint32_t o = 9;
    for (uint32_t n = w(7); n; n--) { o += 4; while (b(o++)); while (b(o++)); }
    return o;
}
// 1.13c D2MCPClient+0x6CB0: { 0x19, u32 8 }, send(len 5); zeroes [0xb1c8]. No arguments.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6cb0(uint8_t* out) {
    out[0] = 0x19; uint32_t v = 8; memcpy(out + 1, &v, 4);
    return 5;
}

// 1.13c D2MCPClient+0x6DE0: { 0x0D } one byte, send(sock [0xb028], buf, 1); zeroes [0xb1c8]. Argument unused.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6de0(uint8_t* out) { out[0] = 0x0D; return 1; }
// 1.13c D2MCPClient+0x6D50 (ordinal 10042, fastcall): { 0x11, byte, word LE } four bytes, send(sock [0xb028], buf, 4); zeroes [0xb1c8]; [0xaeb4] untouched.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6d50(uint8_t b, uint16_t w, uint8_t* out) { out[0] = 0x11; out[1] = b; out[2] = (uint8_t)w; out[3] = (uint8_t)(w >> 8); return 4; }
// 1.13c D2MCPClient+0x6C70 (ordinal 10035): start the MCP receive thread once. `handle` is [0xb1d0]; when it is 0 the
// 0x148-byte config (0x52 dwords) is copied to [0xaee0] and CreateThread(sub_6BA0) runs. Returns 1 when that happens, 0 when
// a thread already exists (config untouched). The thread itself is not ours to compose.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_start_6c70(uint32_t handle, const uint8_t* cfg, uint8_t* cfgDst) {
    if (handle) return 0;
    memcpy(cfgDst, cfg, 0x148);
    return 1;
}
// 1.13c D2MCPClient+0x7290: { 0x04, u16 word, strz, strz }; both strings via sub_6EC0 (shared running length, exit at >= 0x400); stores word to [0xaeb4].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_7290(const char* s1, const char* s2, uint16_t w, uint8_t* out) {
    uint32_t len = 0;
    out[0] = 4; out[1] = w & 0xFF; out[2] = w >> 8;
    if (!app(s1, &len, out + 3)) return 0;
    uint32_t at = len + 3;
    return app(s2, &len, out + at) ? len + 3 : 0;
}
// 1.13c D2MCPClient+0x7330: CREATEGAME, { 0x03, u16 word, u32 s+0x40, s+0x44, s+0x45, s+0x46, strz s, strz s+0x10, strz s+0x20 };
// s is the create-game form (name/password/description in 16-byte char slots at +0,+0x10,+0x20, then difficulty u32,
// the two constant request bytes and maxPlayers). All three strings via sub_6EC0 (one running length, exit at >= 0x400);
// stores word to [0xaeb4] and zeroes [0xaed8] before the send.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_7330(const uint8_t* s, uint16_t w, uint8_t* out) {
    uint32_t len = 0, v; memcpy(&v, s + 0x40, 4);
    out[0] = 3; out[1] = (uint8_t)w; out[2] = (uint8_t)(w >> 8); memcpy(out + 3, &v, 4);
    out[7] = s[0x44]; out[8] = s[0x45]; out[9] = s[0x46];
    if (!app((const char*)s, &len, out + 10)) return 0;
    uint32_t at = len + 10;
    if (!app((const char*)s + 0x10, &len, out + at)) return 0;
    at = len + 10;
    return app((const char*)s + 0x20, &len, out + at) ? len + 10 : 0;
}
// 1.13c D2MCPClient+0x7230: { 0x05, u16 word, u32 u+0x14, strz u }; string via sub_6EC0 (exit at >= 0x400); stores word to [0xaeb4]. No 0xFFFF check.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_7230(const uint8_t* u, uint16_t w, uint8_t* out) {
    uint32_t len = 0, v; memcpy(&v, u + 0x14, 4);
    out[0] = 5; out[1] = w & 0xFF; out[2] = w >> 8; memcpy(out + 3, &v, 4);
    return app((const char*)u, &len, out + 7) ? len + 7 : 0;
}
// 1.13c D2MCPClient+0x6D80 (ordinal 10044, fastcall, argument unused): { 0x10 } one byte, send(sock [0xb028], buf, 1); zeroes [0xb1c8].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6d80(uint8_t* out) { out[0] = 0x10; return 1; }
// 1.13c D2MCPClient+0x6D20 (ordinal 10052, fastcall, argument unused): { 0x12 } one byte, send(sock [0xb028], buf, 1); zeroes [0xb1c8].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_6d20(uint8_t* out) { out[0] = 0x12; return 1; }
// 1.13c D2MCPClient+0x7410: CHARLOGON, { 0x07, strz }; string via sub_6EC0 (exit at >= 0x400); no word, [0xaeb4] untouched.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_7410(const char* s, uint8_t* out) {
    uint32_t len = 0;
    out[0] = 7;
    return app(s, &len, out + 1) ? len + 1 : 0;
}
// 1.13c D2MCPClient+0x6840 (ordinal 10043): copy realm string [0xb1c0] and character name [0xae88] out, return dword [0xae80].
// gz = the globals in retail layout: u32 [0xae80], char[8] [0xb1c0], char[44] [0xae88].
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_pair_6840(const uint8_t* gz, char* o1, char* o2) {
    strcpy(o1, (const char*)gz + 4); strcpy(o2, (const char*)gz + 12);
    uint32_t v; memcpy(&v, gz, 4); return v;
}
// 1.13c D2MCPClient+0x7480: CHARCREATE, { 0x02, u32 class, u16 flags, strz name }; string via sub_6EC0 (exit at >= 0x400).
// Zeroes [0xb1c8] and [0xb214] (probe glue d2mcpclient_probe.h); [0xaeb4] untouched. The dead 0xFFFE check is the
// caller's: 6EC0 already refuses len >= 0x400, so len+7 <= 0x406. Argument order is retail's: name and class on the
// stack, the flags word in ECX (the descriptor's `regs ecx`).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_7480(const char* s, uint32_t cls, uint16_t flags, uint8_t* out) {
    uint32_t len = 0;
    out[0] = 2; memcpy(out + 1, &cls, 4); out[5] = flags & 0xFF; out[6] = (uint8_t)(flags >> 8);
    return app(s, &len, out + 7) ? len + 7 : 0;
}

// 1.13c D2MCPClient+0x07510: startup request { 0x01, reply[0..15], reply[24..71], strz account at reply+0x48 };
// the account via sub_6EC0 (exit at >= 0x400). Zeroes [0xb1c8] (probe glue). Returns the frame length, 0 where retail dies.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_req_7510(const uint8_t* reply, uint8_t* out) {
    uint32_t len = 0;
    out[0] = 1; memcpy(out + 1, reply, 16); memcpy(out + 17, reply + 24, 48);
    return app((const char*)reply + 0x48, &len, out + 0x41) ? len + 0x41 : 0;
}
