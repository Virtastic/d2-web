#include <cstdint>

// 1.13c D2Gfx+0x423B -- MSVC 2003 Release CRT _wctomb, the module's wide-to-multibyte leaf.
// Retail (39 bytes): __getptd 0x28FE, then if the thread's locale pointer (+0x14 of the ptd? no:
// ptd->_tfpecode) differs from the module locale at 0x6FA9074C, ___updatetlocinfo 0x535E; then
// tail-call ___wctomb_mt 0x41DB with (ptlocinfo, mbch, wch). Nothing else.
// ___wctomb_mt (96 bytes): mbch == NULL -> 0 (the "is the encoding stateful" answer, C locale: 0);
// else if the locale's mbctype word is 0 (the C locale) and (USHORT)wch < 0x100, store its low byte and
// return 1; else (DBCS locale, or a wide value >= 0x100) WideCharToMultiByte(codepage, 0, &wch, 1,
// mbch, mbchsize, NULL, &used_default) and return its count when nonzero and used_default is 0 --
// otherwise set errno = EILSEQ (0x2A) and return -1.
//
// Our engine has no per-thread locale and no WideCharToMultiByte, and nothing in the ported engine
// calls this: retail's only caller is _output 0x1E28 (%C), which ours handles without it. So this is
// the C-locale path of ___wctomb_mt only -- the state the DLL is in at load (locale word zero). The
// DBCS branch and the errno store are not modelled: there is no code page and no CRT errno in wasm.
extern "C"
int d2gfx_wctomb(char* mbch, uint16_t wch) {
    if (mbch == nullptr) return 0;              // stateful-encoding query; C locale is stateless
    if (wch < 0x100) { *mbch = (char)wch; return 1; }  // (USHORT)wch < 0x100, low byte only
    return -1;                                  // retail: errno = EILSEQ (0x2A)
}
