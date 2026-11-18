// Browser export of the music selector over the native tables: Levels (dwSoundEnv at +0x21C), then SoundEnviron (Song,
// +0 of a 0x58-byte record), then Sounds (Filename +0, Volume +0x3c, Music Vol +0x53 of a 0x8e-byte record), the tables
// 1.13c D2Client+0x5DFD0 loads (d2client_5dfd0). The record index is the table index, as the native lookups use it.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "../include/D2DataTbls.h"
#include "../include/DataTbls/LevelsTbls.h"
#include "d2client_5dfd0.h"

namespace {
struct Sel { int32_t env, song; const uint8_t* snd; };
// env 0 / song 0 = no music (valid); env -1 = unresolved level or table.
bool select(int32_t level, Sel& s) {
    s = { -1, 0, nullptr };
    if (!d2client_sounds::records() && !d2client_sounds::load()) return false;
    const D2LevelsTxt* r = DATATBLS_GetLevelsTxtRecord(level);
    if (!r) return false;
    s.env = int32_t(r->dwSoundEnv);
    if (s.env <= 0) return true;
    if (s.env >= d2client_sounds::envCount()) { s.env = -1; return false; }
    memcpy(&s.song, d2client_sounds::envTable() + size_t(s.env) * 0x58, 4);
    if (s.song <= 0) return true;
    if (s.song >= d2client_sounds::recordCount()) { s.env = -1; return false; }
    s.snd = d2client_sounds::records() + size_t(s.song) * 0x8e;
    return true;
}
}

// field 0 SoundEnv, 1 Song (Sounds row), 2 Volume, 3 Music Vol; -1 when the level or a row does not resolve.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_level_music(int level, int field) {
    Sel s; if (!select(level, s)) return -1;
    switch (field) {
    case 0: return s.env;
    case 1: return s.song;
    case 2: return s.snd ? s.snd[0x3c] : 0;
    case 3: return s.snd ? s.snd[0x53] : 0;
    }
    return -1;
}
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_client_level_music_path(int level) {
    static char path[0x40];
    Sel s; path[0] = 0;
    if (!select(level, s) || !s.snd) return path;
    memcpy(path, s.snd, 0x3b); path[0x3b] = 0;
    return path;
}
