// BNClient 0xDB70 (EAX = &buffer out, ECX = socket; stack name, size, FILETIME*, mode, flags byte; ret 0x14): receive a
// whole download into memory. buffer = SMemAlloc(size, BnDownload.cpp, 0x387, 8), stored at *out; then recv chunks of
// <= 0x5B4 at buffer+done until done == size. A recv of -1 or <= 0, or the cancel word DAT+0x3F614 set, frees the buffer
// (line 0x391), clears *out and returns 0. In mode 0x80000002 each chunk sets the progress word DAT+0x3F508 =
// done*100/size. Afterwards flag bit 0 stores it through 0x13A00 (name, buffer, size, ft, 0x13C680, 0x278D00) and bit 1
// writes it to disk through 0xD280 (name, ft, buffer, size). Returns 1.
#include <cstdint>

struct BnDb70Seams {
    uint32_t (*alloc)(uint32_t size, uint32_t file, uint32_t line, uint32_t flags);     // 0x8018 Storm #401
    void (*free_)(uint32_t p, uint32_t file, uint32_t line, uint32_t flags);            // 0x801E Storm #403
    int32_t (*recv)(uint32_t sock, uint32_t buf, uint32_t n, uint32_t flags);          // 0x7FEE WSOCK32 16
    void (*store)(const char* name, uint32_t buf, uint32_t size, const uint32_t* ft, uint32_t a, uint32_t b); // 0x13A00
    void (*write_ft)(const char* name, const uint32_t* ft, uint32_t buf, uint32_t size); // 0xD280
    uint32_t* progress;                                                                 // DAT+0x3F508
    const uint32_t* cancel;                                                             // DAT+0x3F614
    uint32_t file;                                                                      // "..\Source\D2BNClient\BnDownload.cpp"
};

// 1.13c BNClient+0x0DB70
extern "C" uint32_t bnclient_recv_buffer_db70(uint32_t* out, uint32_t sock, const char* name, uint32_t size, const uint32_t* ft,
                                              uint32_t mode, uint32_t flags, const BnDb70Seams* s) {
    *out = s->alloc(size, s->file, 0x387, 8);
    uint32_t done = 0;
    while (done < size) {
        uint32_t n = size - done;
        if (n > 0x5B4) n = 0x5B4;
        const int32_t r = s->recv(sock, *out + done, n, 0);
        if (r == -1 || r <= 0 || *s->cancel != 0) {
            s->free_(*out, s->file, 0x391, 0);
            *out = 0;
            return 0;
        }
        done += (uint32_t)r;
        if (mode == 0x80000002u) *s->progress = done * 100u / size;
    }
    if (flags & 1) s->store(name, *out, size, ft, 0x13C680, 0x278D00);
    if (flags & 2) s->write_ft(name, ft, *out, size);
    return 1;
}
