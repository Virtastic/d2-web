#pragma once
#include <cstdint>
// Handles are native pointers. Serialized retail IDs remain separate uint32_t values.
extern "C" {
uint32_t FOG_FOpenFile(const char* name, void** file);
uint32_t FOG_FCloseFile(void* file);
uint32_t FOG_FGetFileSize(void* file, uint32_t* high);
}

namespace d2 { uint32_t nativeFileLastError(); }
