// 1.13c D2Net+0x136d sub_136d: n82_lock8, __lock(8) through the N82Crt seam. r[0] = hooklog blob (@lg).
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
struct N82Crt {
    // globals, named at their retail address (base 0x6FBF0000)
    int exiting;                 // 0xADE4
    int exitFlag;                // 0xADE0
    uint8_t quick;               // 0xADDC
    int initCount;               // 0xADE8
    int (*userEntry)(void*, int, void*);  // 0xB7A4 (null = none)
    void (*preInit)(int);        // 0xB7B8
    void (**atexitBegin)();      // 0xB7B0
    void (**atexitEnd)();        // 0xB7AC (decremented by the walk)
    // seams
    void (*lock)(int);           // 0x18E0 __lock
    void (*unlock)(int);         // 0x182B
    void (*terminateSelf)(unsigned);   // GetCurrentProcess + TerminateProcess
    void* (*getModuleHandle)(const char*);       // GetModuleHandleA
    void* (*getProcAddress)(void*, const char*); // GetProcAddress
    void (*exitProcess)(unsigned);               // ExitProcess
    void (**termA)(); void (**termAEnd)();       // initterm(0xA020..)
    void (**termB)(); void (**termBEnd)();       // initterm(0xA028..)
    int (**initA)(); int (**initAEnd)();         // 0xA00C..0xA018
    void (**initB)(); void (**initBEnd)();       // 0xA000..0xA008
    int (*atexit)(void (*)());                   // 0x5596; the 0x1A47 handler is passed as atexitHandler
    void (*atexitHandler)();
    int (*crtInit1)();           // GetVersionEx..heap_init..mtinit..ioinit..setargv..setenvp, returns >=0 ok
    int (*mtinit)(); void (*ioterm)(); void (*mtterm)(); void (*heapTerm)();
    void (*freeptd)(void*);
    int (*initThread)();         // 0x1F61 calloc+TlsSetValue+__initptd, 1 ok
    int (*dllMain)(void*, int);  // 0x6AA0
};
void n82_lock8(N82Crt* c);
static uint32_t* g_lg;
static void rec(uint32_t rva, uint32_t s0) { uint32_t* w = g_lg + 1 + 10 * g_lg[0]++; for (int i = 0; i < 10; i++) w[i] = 0; w[0] = rva; w[6] = s0; }
static void seam(int n) { rec(0x18e0, (uint32_t)n); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2net_136d(uint32_t* r) {
  g_lg = (uint32_t*)(uintptr_t)r[0];
  for (int i = 0; i < 1 + 10 * 8; i++) g_lg[i] = 0;
  N82Crt c{};
  c.lock = seam;
  n82_lock8(&c);
  r[4] = 0;
}
