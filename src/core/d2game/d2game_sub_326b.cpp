#include <cstdio>

// __fclose_lk(stream): flag&0x83 set -> flush, free buffer, close fd, free _tmpfname; _flag cleared;
// a close failure forces -1. Host stdio holds that state, so fclose is the whole effect.
// 1.13c D2Game+0x0326B
int D2Game_CrtFcloseLk(FILE* stream)
{
    return std::fclose(stream);
}
