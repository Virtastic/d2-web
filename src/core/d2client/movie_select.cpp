#include "movie_select.h"
#include <emscripten.h>

// 1.13c D2Client+0x899A0 / +0xAF940 / +0xE1314. The browser reads these so the engine's 0x61
// requests and the film rows are D2Client's, not a second copy in JS.

// 1.13c D2Client+0xAF940: the D2GS opcode-0x61 receiver.
// The index an opcode-0x61 datagram selects (D2Client+0xAF940), or -1 when this is not one.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_movie_index(const uint8_t* packet, uint32_t size) {
    return d2ClientMovieIndexFromPacket(packet, size);
}

// 1.13c D2Client+0x899A0: the film player.
// The film file name for a row (D2Client+0x899A0), resolution applied; 0 when the index is not a row.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_movie_file_name(uint32_t index, int32_t wide,
                                                                   char* out, uint32_t cap) {
    return d2ClientMovieFileName(index, wide ? "640x292" : "640x146", out, cap);
}

// 1.13c D2Client+0x89770: the local-video directory resolver (DATA\LOCAL\video\<language>).
// The DAT\GLOBAL branch is D2Client+0x899A0's own join, "<dir>\\video\\<name>".
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_movie_path(const char* dir, const char* name,
                                                             char* out, uint32_t cap) {
    return d2ClientMoviePath(dir, name, out, cap);
}

// The row's directory flag: 1 = DATA\LOCAL\<language>, 0 = DATA\GLOBAL.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_movie_local_dir(uint32_t index) {
    return index < kD2ClientMovieCount ? kD2ClientMovieRows[index].localVideoDir : -1;
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_movie_count() { return kD2ClientMovieCount; }
