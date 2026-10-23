// 1.13c D2Win+0x06CF4 -- MSVC CRT __report_gsfailure(code): builds "<msg>\n\nProgram: <exe>\n\n<hint>"
// for a message box, then __exit(3). Never returns; no game state is touched.
#include <cstdint>
#include <cstdio>
#include <cstdlib>

extern "C" void d2win_sub_6cf4(int32_t code) {   // 1.13c D2Win+0x06CF4
    const char* msg = code == 1 ? "Buffer overrun detected!" : "Unknown security failure detected!";
    fprintf(stderr, "Microsoft Visual C++ Runtime Library: %s\n", msg);   // ___crtMessageBoxA stand-in
    exit(3);
}
