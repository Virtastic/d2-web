// src/core/d2win/d2win_a710.cpp -- 1.13c D2Win.dll+0x0A710, load a cell file by name plus the type's extension
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering and header layout only).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2WinA710Env {
    // D2CMP #10070: the extension string for `type`.
    const char* (*extension)(uint32_t type);
    // D2Win+0x7C60: read a whole file (path in EAX in retail) into a Fog block; *size gets its length.
    void* (*loadFile)(const char* path, uint32_t n0, uint32_t* size);
    // D2CMP #10006: build a cell file from the block into *cell.
    void (*cellCreate)(void* data, uint32_t* cell, const char* file, int32_t line, int32_t n1, int32_t n0);
};

static const char kArchiveCpp[] = "..\\Source\\D2Win\\Src\\D2WinArchive.cpp";

// 1.13c D2Win+0x0A710 (ordinal 10180). Retail: name in ECX, size out in EDX, type on the stack.
// The path buffer is 260 bytes; strcpy + strcat with no bound, as in retail. The returned cell is
// the stack local D2CMP #10006 writes (zero here if it does not write it; retail's is unset).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_A710(const char* name, uint32_t* size, uint32_t type,
                                                       const D2WinA710Env* env) {
    char path[260];
    uint32_t cell = 0;
    std::strcpy(path, name);
    std::strcat(path, env->extension(type));
    void* data = env->loadFile(path, 0, size);
    env->cellCreate(data, &cell, kArchiveCpp, 0x53, -1, 0);
    return cell;
}
