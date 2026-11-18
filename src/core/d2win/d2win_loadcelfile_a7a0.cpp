// src/core/d2win/d2win_loadcelfile_a7a0.cpp -- 1.13c D2Win.dll LoadCelFile, from the cached Ghidra decompile
// (build-wasm/ghidra-d2win/decomp/0xA7A0.c).
#include <cstdint>
#include <cstring>

// Host seams: Ordinal_10070 (suffix string for param_2), 0x7C60 (EAX = path, stack 0, 0: the file block), Ordinal_10006 (load).
struct D2WinLoadCelEnv {
    const char* (*suffix)(uint32_t id);
    int32_t (*archive)(const char* path, int32_t n0, uint32_t* n1);
    void (*load)(int32_t archive, uint32_t* cell, const char* srcFile, int32_t line, int32_t a, int32_t b);
};

// 1.13c D2Win+0x0A7A0 (ordinal 10111)
// fastcall(path, id), plain ret: builds path + suffix(id) in a 260-byte stack buffer (retail: unchecked strcpy/strcat), reads the
// file through 0x7C60 (EAX = buffer, 0, 0) and hands the block to D2CMP #10006 with the local cell out-pointer, source tag
// "..\Source\D2Win\Src\D2WinArchive.cpp" line 0x42, (-1, 0). Returns that local (retail's EAX = [esp+8] after the call).
// ponytail: buffer truncated at 259 bytes instead of overflowing.
extern "C" uint32_t d2win_loadcelfile_a7a0(const char* path, uint32_t id, const D2WinLoadCelEnv* env) {
    char buf[260];
    uint32_t cell = 0;
    strncpy(buf, path, sizeof buf - 1);
    buf[sizeof buf - 1] = 0;
    strncat(buf, env->suffix(id), sizeof buf - 1 - strlen(buf));
    env->load(env->archive(buf, 0, nullptr), &cell, "..\\Source\\D2Win\\Src\\D2WinArchive.cpp", 0x42, -1, 0);
    return cell;
}
