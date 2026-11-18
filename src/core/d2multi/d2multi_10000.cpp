// 1.13c D2Multi+0x10000: EDI = the previous whisper target, EBX = the message text, stdcall (const char* name). Copies EDI into the global DAT_6fa09198 and builds the whisper line:
// "%s whispers: %s" when name is empty, or when name starts with '*' and its text after '*' equals EDI's; otherwise "%s (*%s) whispers: %s". Shows it with 0xFC90(line, 2).
#include "d2multi_env.h"
#include <cstring>

void d2multi_10000(const D2mEnv& e, const char* name, const char* edi, const char* ebx, char* last) {
    strcpy(last, edi);
    char line[500];
    bool plain = *name == 0;
    if (!plain && *name == '*') {
        const char* p = strchr(edi, '*');
        const char* a = (p && p[1]) ? p + 1 : edi;
        const char* q = strchr(name, '*');
        const char* bb = (q && q[1]) ? q + 1 : name;
        plain = strcmp(a, bb) == 0;
    }
    if (plain) d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(line), D2M_MOD(0x17608), d2m_ptr(name), d2m_ptr(ebx));
    else d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(line), D2M_MOD(0x175f0), d2m_ptr(name), d2m_ptr(edi));
    d2m_call(e, 0xfc90, 0, 0, 0, 0, 0, d2m_ptr(line), 2);
}
