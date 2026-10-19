// Prior art used: no D2MOO D2Client module. Existing stream_algorithms_wave_a
// owns the native window/checksum view. Local 1.13c decompiles and disassembly
// establish allocator register arguments and the union members freed by mode.
#include "inflate_lifecycle.h"
#include <new>

// 1.13c D2Client+0x0F3F0
void D2Client_ResetInflateBlocks(D2ClientInflateBlocks& blocks,
    const D2ClientInflateAllocator& allocator, uint32_t& streamChecksum,
    uint32_t* previousChecksum) {
    if (previousChecksum) *previousChecksum = blocks.window.checksum;
    if (blocks.mode == 4 || blocks.mode == 5)
        allocator.release(allocator.opaque, blocks.sub.trees.lengths);
    if (blocks.mode == 6)
        allocator.release(allocator.opaque, blocks.sub.codes);
    blocks.window.write = blocks.window.begin;
    blocks.window.read = blocks.window.begin;
    blocks.mode = 0;
    blocks.bitCount = 0;
    blocks.bitBuffer = 0;
    if (blocks.window.check) {
        blocks.window.checksum = blocks.window.check(0, nullptr, 0);
        streamChecksum = blocks.window.checksum;
    }
}

// 1.13c D2Client+0x0F450
int32_t D2Client_FreeInflateBlocks(D2ClientInflateBlocks* blocks,
    const D2ClientInflateAllocator& allocator, uint32_t& streamChecksum) {
    D2Client_ResetInflateBlocks(*blocks, allocator, streamChecksum, nullptr);
    allocator.release(allocator.opaque, blocks->window.begin);
    allocator.release(allocator.opaque, blocks->workspace);
    allocator.release(allocator.opaque, blocks);
    return 0;
}

// 1.13c D2Client+0x0FF60
D2ClientInflateBlocks* D2Client_NewInflateBlocks(const D2ClientInflateAllocator& allocator,
    uint32_t windowSize, uint32_t (*check)(uint32_t, const uint8_t*, uint32_t),
    uint32_t& streamChecksum) {
    void* storage = allocator.allocate(allocator.opaque, 1, sizeof(D2ClientInflateBlocks));
    if (!storage) return nullptr;
    auto* blocks = ::new (storage) D2ClientInflateBlocks;
    // Retail requests 8 * 1440 bytes: a native huft can contain a native pointer.
    blocks->workspace = static_cast<D2ClientInflateHuft*>(
        allocator.allocate(allocator.opaque, sizeof(D2ClientInflateHuft), 1440));
    if (!blocks->workspace) {
        allocator.release(allocator.opaque, blocks);
        return nullptr;
    }
    blocks->window.begin = static_cast<uint8_t*>(allocator.allocate(allocator.opaque, 1, windowSize));
    if (!blocks->window.begin) {
        allocator.release(allocator.opaque, blocks->workspace);
        allocator.release(allocator.opaque, blocks);
        return nullptr;
    }
    blocks->window.end = blocks->window.begin + windowSize;
    blocks->window.check = check;
    blocks->mode = 0;
    D2Client_ResetInflateBlocks(*blocks, allocator, streamChecksum, nullptr);
    return blocks;
}

// 1.13c D2Client+0x0E310
D2ClientInflateCodes* D2Client_NewInflateCodes(const D2ClientInflateAllocator& allocator,
    uint8_t literalBits, uint8_t distanceBits, D2ClientInflateHuft* literalTree,
    D2ClientInflateHuft* distanceTree) {
    void* storage = allocator.allocate(allocator.opaque, 1, sizeof(D2ClientInflateCodes));
    if (!storage) return nullptr;
    auto* codes = ::new (storage) D2ClientInflateCodes;
    codes->literalBits = literalBits;
    codes->distanceBits = distanceBits;
    codes->mode = 0;
    codes->literalTree = literalTree;
    codes->distanceTree = distanceTree;
    return codes;
}
