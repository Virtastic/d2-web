// src/core/d2win/d2win_loadcelfile_a7a0.cpp -- 1.13c D2Win.dll LoadCelFile, from the cached Ghidra decompile
// (build-wasm/ghidra-d2win/decomp/0xA7A0.c).
#include <cstdint>
#include <cstring>

// Host seams: Ordinal_10070 (suffix string for param_2), sub_7C60(0,0) (archive handle), Ordinal_10006 (load).
struct D2WinLoadCelEnv {
    const char* (*suffix)(uint32_t id);
    int32_t (*archive)();
    void* (*load)(int32_t archive, const char* path, const char* srcFile, int32_t line, int32_t a, int32_t b);
};

// 1.13c D2Win+0x0A7A0
// fastcall(path, id): builds path + suffix(id) in a 260-byte stack buffer (retail: unchecked strcpy/strcat),
// then loads it from the archive with source tag "..\Source\D2Win\Src\D2WinArchive.cpp" line 0x42, (-1, 0).
// Retail's return value is the source-tag string pointer (a decompiler artifact of the unused eax); we return
// the loader result. ponytail: buffer truncated at 259 bytes instead of overflowing.
extern "C" void* d2win_loadcelfile_a7a0(const char* path, uint32_t id, const D2WinLoadCelEnv* env) {
    char buf[260];
    strncpy(buf, path, sizeof buf - 1);
    buf[sizeof buf - 1] = 0;
    strncat(buf, env->suffix(id), sizeof buf - 1 - strlen(buf));
    return env->load(env->archive(), buf, "..\\Source\\D2Win\\Src\\D2WinArchive.cpp", 0x42, -1, 0);
}
