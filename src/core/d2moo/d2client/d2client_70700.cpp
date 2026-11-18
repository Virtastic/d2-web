// 1.13c D2Client+0x70700: chat verb handler, one-argument form (called from 0x707F0). EAX = line (writable), EBX = verb.
// Verb match through 0x6EFB0 (EAX = verb, stack = line; 0 = match), then the verb must be followed by a space or the end of the line.
// The line is tokenised on ' ': exactly one token (the verb alone) goes to the Battle.net handler vtable slot +0x14 (ECX = line),
// anything else shows "<BattleNet>: Correct usage is: <verb>" through 0x7F780 (system message record: +1 = 4, +8 = 3, text at +0x1a, 256 bytes).
// Returns 0 when the verb does not match, else 1. vtFnRva is the RVA of the vtable slot's callee, 0 when [gs+0x221] is null.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "d2client_callee.h"
// MSVCRT strtok with the single delimiter ' ' (in place; the caller owns the save pointer).
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
extern "C" uint32_t d2c_70700_svc(const D2cSvc& v, char* line, const char* cmd, uint32_t vtFnRva) {
    const uint32_t len = (uint32_t)std::strlen(cmd);
    if (d2c_call(v, 0x6efb0, (uint32_t)(uintptr_t)cmd, 0, 0, 0, 0, (uint32_t)(uintptr_t)line) != 0) return 0;
    if (line[len] != ' ' && line[len] != 0) return 0;
    char* save = nullptr;
    int n = 0;
    char* tk = d2c_strtok_sp(line, &save);
    if (tk) {
        while (tk) {
            if (n > 1) break;
            ++n;
            tk = d2c_strtok_sp(nullptr, &save);
        }
        if (n == 1) {
            if (vtFnRva) d2c_call(v, vtFnRva, 0, (uint32_t)(uintptr_t)line, 0, 0, 0, (uint32_t)(uintptr_t)line);
            return 1;
        }
    }
    char buf[256 + 64] = {};
    std::snprintf(buf, sizeof buf, "<BattleNet>: Correct usage is: %s", cmd);
    uint8_t pkt[0x140] = {};
    pkt[1] = 4; pkt[8] = 3;
    std::memcpy(pkt + 0x1a, buf, 256);
    d2c_call(v, 0x7f780, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)pkt);
    return 1;
}
