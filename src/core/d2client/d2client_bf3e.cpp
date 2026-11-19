// D2Client import thunks that jump through the IAT into Storm: 0xBF3E (Storm #514, SMsgUnregisterKeyUp 0x344F0),
// 0xBF62 (Storm #515, SMsgUnregisterMessage 0x344D0) and 0xBF68 (Storm #475, SCmdProcess 0x25890). Each one forwards its
// stack arguments unchanged and returns the import's result; the callers in D2Client link these instead of the Storm
// senders directly, so the thunk rows are the call path.
#include <cstdint>
#include <emscripten.h>

extern "C" {
void Storm_Sub344F0_Real(uint32_t key, uint32_t a, uint32_t b, uint32_t esi);
void Storm_Sub344D0_Real(uint32_t key, uint32_t a, uint32_t b, uint32_t esi);
int32_t Storm_Sub25890(char** string, uint32_t param_2, void* callback, void* errorHandler);

// 1.13c D2Client+0xBF3E
void d2client_bf3e(uint32_t key, uint32_t a, uint32_t b) { Storm_Sub344F0_Real(key, a, b, 0); }
// 1.13c D2Client+0xBF62
void d2client_bf62(uint32_t key, uint32_t a, uint32_t b) { Storm_Sub344D0_Real(key, a, b, 0); }
// 1.13c D2Client+0xBF68
int32_t d2client_bf68(char** string, uint32_t param_2, void* callback, void* errorHandler) {
    return Storm_Sub25890(string, param_2, callback, errorHandler);
}
}
