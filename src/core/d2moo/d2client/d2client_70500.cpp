// 1.13c D2Client+0x70500: whisper chat verb handler (called four times from 0x707F0). EAX = line (writable), EBX = verb, stack p1 (ret 4).
// 0x6EFB0 verb match + space/end check as 0x70700. The line is tokenised on ' ': token 2 = who, token 3 = the rest of the line from
// its start (taken from an unmodified copy, so inner spaces survive). With three tokens: when p1 != 0 and a node of the player list
// (head DAT 0x11BC14, name at +0x66, next at +0x80) matches who (case-insensitive) a whisper packet {0x15, 2, gameType (DAT 0x113F60),
// message (<= 0xFF bytes, zero padded), 0, who} goes out through 0x147A0 (ESI = packet); otherwise the line is rewritten "<verb> <who> <msg>"
// and handed to the Battle.net handler vtable slot +0x14 (ECX = line). Fewer than three tokens: "<BattleNet>: Correct usage is: <verb> (name) (message)"
// through 0x7F780. Returns 0 when the verb does not match, else 1. vtFnRva: callee RVA of the vtable slot, 0 when [gs+0x221] is null.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "d2client_callee.h"
static char* d2c_strtok_sp(char* s, char** save) {
    if (!s) s = *save;
    if (!s) return nullptr;
    while (*s == ' ') ++s;
    if (!*s) { *save = nullptr; return nullptr; }
    char* t = s;
    while (*s && *s != ' ') ++s;
    if (*s) { *s = 0; *save = s + 1; } else *save = s;
    return t;
}
static int d2c_stricmp_ascii(const char* a, const char* b) {
    for (;; ++a, ++b) {
        int x = (uint8_t)*a, y = (uint8_t)*b;
        if (x >= 'A' && x <= 'Z') x += 32;
        if (y >= 'A' && y <= 'Z') y += 32;
        if (x != y) return x < y ? -1 : 1;
        if (!x) return 0;
    }
}
extern "C" uint32_t d2c_70500_svc(const D2cSvc& v, char* line, const char* cmd, uint32_t p1, uint32_t listHead, uint8_t gameType, uint32_t vtFnRva) {
    const uint32_t len = (uint32_t)std::strlen(cmd);
    if (d2c_call(v, 0x6efb0, (uint32_t)(uintptr_t)cmd, 0, 0, 0, 0, (uint32_t)(uintptr_t)line) != 0) return 0;
    if (line[len] != ' ' && line[len] != 0) return 0;
    char copy[0x430] = {}, who[0x300] = {}, msg[0x300] = {};
    std::strcpy(copy, line);
    char* save = nullptr;
    int n = 0;
    bool three = false;
    for (char* tk = d2c_strtok_sp(line, &save); tk; tk = d2c_strtok_sp(nullptr, &save)) {
        ++n;
        if (n == 2) std::strcpy(who, tk);
        else if (n == 3) { std::strcpy(msg, copy + (tk - line)); three = true; break; }
    }
    if (three) {
        if (p1) {
            for (uint32_t nd = listHead; nd; nd = *(const uint32_t*)(uintptr_t)(nd + 0x80)) {
                if (d2c_stricmp_ascii((const char*)(uintptr_t)(nd + 0x66), who) != 0) continue;
                uint8_t pkt[0x430] = {};
                std::strncpy((char*)pkt + 3, msg, 0xff);
                pkt[0x102] = 0;
                std::strcpy((char*)pkt + 0x103, who);
                pkt[2] = gameType; pkt[1] = 2; pkt[0] = 0x15;
                d2c_call(v, 0x147a0, 0, 0, 0, 0, (uint32_t)(uintptr_t)pkt, (uint32_t)(uintptr_t)pkt);   // s0: the probe's block pointer (ESI is the callee's input)
                return 1;
            }
        }
        std::snprintf(line, 0x300, "%s %s %s", cmd, who, msg);
        if (vtFnRva) d2c_call(v, vtFnRva, 0, (uint32_t)(uintptr_t)line, 0, 0, 0, (uint32_t)(uintptr_t)line);
        return 1;
    }
    char buf[256 + 64] = {};
    std::snprintf(buf, sizeof buf, "<BattleNet>: Correct usage is: %s (name) (message)", cmd);
    uint8_t pkt[0x140] = {};
    pkt[1] = 4; pkt[8] = 3;
    std::memcpy(pkt + 0x1a, buf, 256);
    d2c_call(v, 0x7f780, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)pkt);
    return 1;
}
