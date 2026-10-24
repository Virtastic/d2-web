#include <cstdint>

void D2Common_422D_LockFile2(int32_t nIndex, void* pFile);

// 1.13c D2Common+0x041FE
// __lock_file(FILE*): a FILE inside retail's static 20-slot stdio table (32-byte stride) takes lock
// table entry slot+0x10; any other FILE is locked through its own critical section at +0x20, which is
// exactly __lock_file2 with an index >= 0x14. Host stdio has no static table in wasm, so every stream
// takes the own-critical-section path.
void D2Common_41FE_LockFile(void* pFile)
{
    D2Common_422D_LockFile2(0x14, pFile);
}
