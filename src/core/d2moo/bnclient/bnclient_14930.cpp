// 1.13c BNClient+0x14930: SHA-1 update on the 0x5C context (bit count lo/hi at +0/+4, state at +8, buffer at +0x1C).
// EAX = data, EDI = context, stack word 0 = length. The 64-bit bit count advances first; buffered bytes are kept in
// the context and whole blocks go through the compression step 0x14470 (bn_sha1_block_14470).
#include <cstdint>
#include <cstring>

extern "C" void bn_sha1_block_14470(uint32_t* h, const uint8_t* p);

extern "C" void bnclient_14930(uint32_t* ctx, const uint8_t* data, uint32_t length) {
    const uint64_t count = uint64_t(ctx[0]) | uint64_t(ctx[1]) << 32;
    const uint32_t used = uint32_t(count >> 3) & 0x3f;
    const uint64_t total = count + (uint64_t(length) << 3);
    ctx[0] = uint32_t(total);
    ctx[1] = uint32_t(total >> 32);
    uint8_t* buf = reinterpret_cast<uint8_t*>(ctx + 7);
    uint32_t done = 0;
    if (used) {
        const uint32_t fill = 64 - used;
        if (length < fill) {
            std::memcpy(buf + used, data, length);
            return;
        }
        std::memcpy(buf + used, data, fill);
        bn_sha1_block_14470(ctx + 2, buf);
        done = fill;
    }
    for (; length - done >= 64; done += 64) bn_sha1_block_14470(ctx + 2, data + done);
    std::memcpy(buf, data + done, length - done);
}
