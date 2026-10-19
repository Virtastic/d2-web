#pragma once
#include <cstddef>
#include <cstdint>
// Native views of caller-owned buffers; pointers never contain retail addresses.
struct D2ClientInflateWindow {
    uint8_t* begin;
    uint8_t* end;
    uint8_t* read;
    uint8_t* write;
    uint32_t (*check)(uint32_t, const uint8_t*, uint32_t);
    uint32_t checksum;
};
struct D2ClientInflateOutput {
    uint8_t* next;
    uint32_t available;
    uint32_t total;
    uint32_t checksum;
};
void D2Client_CopyPitchedRows(uint8_t*, const uint8_t*, std::ptrdiff_t destinationPitch,
                            std::ptrdiff_t sourcePitch, uint32_t rows, uint32_t width);
int32_t D2Client_FlushInflateWindow(D2ClientInflateWindow&, D2ClientInflateOutput&, int32_t);
uint32_t D2Client_StreamAdler32(uint32_t, const uint8_t*, uint32_t);
// Key length nonzero; retail state layout is S[256], x, y.
uint32_t D2Client_InitStreamCipher(uint8_t (&state)[258], const uint8_t*, uint32_t keyLength);
