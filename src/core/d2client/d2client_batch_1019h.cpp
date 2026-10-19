// src/core/d2client/d2client_batch_1019h.cpp -- five 1.13c D2Client.dll CRT rows reimplemented from
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane, batch 1019h). Four are the 8-byte SEH finally
// fragments of __close/__read/__lseek/FlushFileBuffers; each calls __unlock_fhandle with the descriptor
// in EBX, so the handle is the explicit argument here. The fifth is the 9-byte __unlock(0xd) thunk of
// __setmbcp. __unlock_fhandle itself is row 0x0A71B, not this batch, so it stays a host seam accepted by
// every fragment (the bnclient_batch3_crt_unwind.cpp convention).
#include <cstdint>
#include <emscripten.h>

// _unlock(idx), recovered in d2client_batch_3664.cpp.
extern "C" void d2_client_47ab(uint32_t idx);

typedef void (*D2ClientUnlockFhandleFn)(int32_t file_handle);  // 0x0A71B __unlock_fhandle

// 1.13c D2Client+0x06157: finally fragment of __close; __unlock_fhandle(fh).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6157(int32_t file_handle,
                                                    D2ClientUnlockFhandleFn unlock_fhandle) {
    unlock_fhandle(file_handle);
}

// 1.13c D2Client+0x0840A: finally fragment of __read; __unlock_fhandle(fh).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_840a(int32_t file_handle,
                                                    D2ClientUnlockFhandleFn unlock_fhandle) {
    unlock_fhandle(file_handle);
}

// 1.13c D2Client+0x08773: finally fragment of __lseek; __unlock_fhandle(fh).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_8773(int32_t file_handle,
                                                    D2ClientUnlockFhandleFn unlock_fhandle) {
    unlock_fhandle(file_handle);
}

// 1.13c D2Client+0x0A959: finally fragment of the FlushFileBuffers wrapper; __unlock_fhandle(fh).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a959(int32_t file_handle,
                                                    D2ClientUnlockFhandleFn unlock_fhandle) {
    unlock_fhandle(file_handle);
}

// 1.13c D2Client+0x0B61E: __unlock(0xd) -- call 0x47AB with 0xd (the __setmbcp lock index).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_b61e(void) {
    d2_client_47ab(0xd);
}
