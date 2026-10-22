// 1.13c BNClient+0x042D7 -- __setenvp: split the double-NUL environment block into a null-terminated
// pointer table of heap copies, skipping entries that begin with '='; frees the block, sets _environ.
#include <cstdint>
#include <cstdlib>
#include <cstring>

static char* g_bn_envblock;   // 0x1E36C the raw environment strings
static char** g_bn_environ;   // 0x1E344
static int g_bn_env_inited;   // 0x40D10

extern "C" void bn_setenvp_input(char* block) { g_bn_envblock = block; }
extern "C" char** bn_environ() { return g_bn_environ; }

extern "C" int bn_setenvp() {
    if (!g_bn_envblock) return -1;
    int n = 0;
    for (const char* p = g_bn_envblock; *p; p += strlen(p) + 1)
        if (*p != '=') n++;
    char** t = (char**)malloc(n * 4 + 4);
    g_bn_environ = t;
    if (!t) return -1;
    for (const char* p = g_bn_envblock; *p; p += strlen(p) + 1) {
        if (*p == '=') continue;
        *t = (char*)malloc(strlen(p) + 1);
        if (!*t) { free(g_bn_environ); g_bn_environ = nullptr; return -1; }
        strcpy(*t++, p);
    }
    free(g_bn_envblock);
    g_bn_envblock = nullptr;
    *t = nullptr;
    g_bn_env_inited = 1;
    return 0;
}
