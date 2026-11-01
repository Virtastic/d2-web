// D2Common 1.13c 0x1B930: next token of a delimited list. The cursor (char**) is advanced past the delimiter
// or set to NULL at the end; the token is returned as atol(token). Retail: EAX = delimiter set, ESI = cursor.
#include <cstdint>
#include <cstdlib>
#include <cstring>

// 1.13c D2Common+0x1B930
int32_t D2Common_1B930_NextToken(char** cur, const char* delims) {
    char* s = *cur;
    char* p = s + strcspn(s, delims);
    *cur = p;
    if (*p != 0) {
        *p = 0;
        *cur = p + 1;
    } else {
        *cur = nullptr;
    }
    return static_cast<int32_t>(atol(s));
}
