// D2Lang batch b144. Skipped (CRT/unaff locals/32-bit globals): 7EF0 4BB1 4FD3.
#include <cstdint>

// 1.13c D2Lang+0x1e68: TlsAlloc thunk (result discarded). No TLS on single-threaded wasm.
void d2lang_tlsAllocThunk() {}

// 1.13c D2Lang+0x2044: leave CRT lock 13 (sub_2215(0xd)). No-op on single-threaded wasm.
void d2lang_leaveLock13_2044() {}

// 1.13c D2Lang+0x2050: leave CRT lock 12 (sub_2215(0xc)). No-op.
void d2lang_leaveLock12_2050() {}

// 1.13c D2Lang+0x49be: EAX=codepage (hidden register) -> explicit param; ANSI cp -> LANGID.
uint32_t d2lang_cpToLangId(int cp)
{
    switch (cp) {
    case 0x3a4: return 0x411;
    case 0x3a8: return 0x804;
    case 0x3b5: return 0x412;
    case 0x3b6: return 0x404;
    }
    return 0;
}

// 1.13c D2Lang+0x4e8c: leave CRT lock 13 (sub_2215(0xd)). No-op.
void d2lang_leaveLock13_4e8c() {}

// 1.13c D2Lang+0x513b: leave CRT lock 4 (sub_2215(4)). No-op.
void d2lang_leaveLock4_513b() {}

// 1.13c D2Lang+0x51f2: leave CRT lock 4 (sub_2215(4)). No-op.
void d2lang_leaveLock4_51f2() {}
