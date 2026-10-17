// 1.13c D2Client+0xAF940 (D2GS opcode 0x61 receiver) and D2Client+0x899A0 (the film player it calls).
// Decompile-derived (tools/re, project d2client113c); no clean-room reimplementation.
//
// The D2GS client receive table sits at VA 0x6FB8DE60 (RVA 0xDE60), 12 bytes per opcode:
// { fn, size, flags }. Its opcode 0x61 record is at VA 0x6FB8E2EC (RVA 0xDE2EC) and reads
// { fn=0x6FB5F940, size=2, flags=0 } -- i.e. D2Client receives a 2-byte packet whose byte 1 names
// one of its own film rows. The handler forwards that byte straight to the player:
//     FUN_6FB5F940(packet) { FUN_6FB399A0(packet[1]); ...Ordinal_423("Diablo II","Resolution")... }
// The player rejects only index >= row count (byte 1 is otherwise unexamined, so 0 = the promo film).
//
// The film rows are at VA 0x6FB91314 (RVA 0xE1314), 12 bytes each, count at VA 0x6FB91370
// (RVA 0xE1370) = 8:
//     { const char* name; uint8_t local_video_dir; uint8_t resolution_suffix; uint8_t pad[2]; }
// `resolution_suffix` != 0 means the name is a printf template taking the display's video resolution
// ("640x292" when the client's widescreen selector 0x6FBBF2F2 is set, else "640x146"); 0 means the
// name is the literal file name. `local_video_dir` != 0 resolves the directory through
// D2Client+0x89770 (its format string and language come in registers, so only the shape is
// recovered): "DATA\LOCAL\video\<language>\<name>", which is the spelling the disc listfile uses
// (data\local\video\Eng\d2intro640x292.bik). 0 is the literal "DATA\GLOBAL\video\<name>".
//
// Only the 0x61 packet starts an act's film. D2Client's own frame loop (D2Client+0x45100) plays the
// other two: index 7 when the expansion-outro global 0x6FBCC3AC is set, index 5 when 0x6FBCC388 is
// set. There is no D2Client or D2Game producer of index 1 (d2intro) on the character-creation path;
// the only automatic d2intro is D2Launch's one-time `Diablo II\Aux Battle.net` gate at startup.
#pragma once
#include <cstdint>

// 1.13c D2Client+0x899A0 / +0xE1314. In the DLL's own order (the order the 0x61 payload indexes).
struct D2ClientMovieRow {
    const char* name;              // printf template when resolutionSuffix, else the literal name
    uint8_t localVideoDir;         // 1 = DATA\LOCAL\<language>, 0 = DATA\GLOBAL
    uint8_t resolutionSuffix;      // 1 = name takes the resolution
};

inline constexpr D2ClientMovieRow kD2ClientMovieRows[] = {
    { "d2_promo.bik", 0, 0 },
    { "d2intro%s.bik", 1, 1 },
    { "ACT02START%s.BIK", 1, 1 },
    { "ACT03START%s.BIK", 1, 1 },
    { "ACT04START%s.BIK", 1, 1 },
    { "ACT04END%s.BIK", 1, 1 },
    { "D2X_INTRO_%s.BIK", 1, 1 },
    { "D2X_OUT_%s.BIK", 1, 1 },
};
inline constexpr uint32_t kD2ClientMovieCount = 8;

// D2Client+0xAF940: the index the 0x61 payload selects, or -1 when this is not a 0x61 packet.
inline int32_t d2ClientMovieIndexFromPacket(const uint8_t* packet, uint32_t size) {
    if (!packet || size != 2 || packet[0] != 0x61) return -1;
    return packet[1];
}

// D2Client+0x899A0: the film file name for a row, resolution applied. 0 when the index is not a row.
// `suffix` is the resolution text the client would substitute ("640x292" in retail's widescreen mode).
inline uint32_t d2ClientMovieFileName(uint32_t index, const char* suffix, char* out, uint32_t cap) {
    if (index >= kD2ClientMovieCount || !out || !cap) return 0;
    const D2ClientMovieRow& row = kD2ClientMovieRows[index];
    uint32_t n = 0;
    const char* p = row.name;
    uint32_t written = 0;
    while (*p) {
        if (row.resolutionSuffix && p[0] == '%' && p[1] == 's') {
            for (const char* s = suffix ? suffix : ""; *s; ++s) {
                if (written + 1 >= cap) return 0;
                out[written++] = *s;
            }
            p += 2;
            continue;
        }
        if (written + 1 >= cap) return 0;
        out[written++] = *p++;
    }
    out[written] = '\0';
    n = written;
    return n;
}

// D2Client+0x89770: the "<dir>\\video\\<name>" image, built as the client builds it. 0 on overflow.
inline uint32_t d2ClientMoviePath(const char* dir, const char* name, char* out, uint32_t cap) {
    if (!dir || !name || !out || !cap) return 0;
    uint32_t n = 0;
    for (const char* p = dir; *p; ++p) { if (n + 1 >= cap) return 0; out[n++] = *p; }
    for (const char* p = "\\video\\"; *p; ++p) { if (n + 1 >= cap) return 0; out[n++] = *p; }
    for (const char* p = name; *p; ++p) { if (n + 1 >= cap) return 0; out[n++] = *p; }
    out[n] = '\0';
    return n;
}
