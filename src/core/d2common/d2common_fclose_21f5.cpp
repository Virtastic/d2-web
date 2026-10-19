#include <cstdint>
#include <cstdio>

// 1.13c D2Common+0x021F5
// MSVC 2003 _fclose wrapper. D2Common's hosted file paths use host stdio streams (the same model as
// D2Game_CrtFclose), whose close operation includes the stream lock and returns the flush/close status.
extern "C" int32_t D2Common_21F5_FClose(FILE* stream)
{
    return static_cast<int32_t>(std::fclose(stream));
}
