// 1.13c D2Win+0x4EE0 -- statically linked CRT strcat (word-at-a-time scan/copy); returns dst.
#include <cstring>
// 1.13c D2Win+0x04EE0
extern "C" char* d2win_sub_4ee0(char* dst, const char* src) { strcpy(dst + strlen(dst), src); return dst; }
