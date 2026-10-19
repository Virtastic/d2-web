// BNClient 1.13c arithmetic leaves, derived from the cached retail decompiles.
// Prior art cross-check: bncsutil/src/bncsutil/nls.c uses GMP limb arithmetic
// for authentication; it does not supply these retail limb/IDEA routines.
#include <cstdint>

// 1.13c BNClient+0x08160
// Positive limb count. Limbs are most-significant first; carry enters at the
// last limb. Keep the descending read/write order, including partial overlap.
extern "C" uint32_t bn_limb_multiply_add(uint32_t count, uint32_t multiplier,
                                       const uint32_t* input, uint32_t* output,
                                       uint32_t carry) {
    do {
        --count;
        const uint64_t product = uint64_t(input[count]) * multiplier + carry;
        carry = uint32_t(product >> 32);
        output[count] = uint32_t(product);
    } while (count != 0);
    return carry;
}

// 1.13c BNClient+0x122E0
// IDEA represents 65536 by zero. The caller supplies a 16-bit key word.
extern "C" uint32_t bn_idea_inverse(uint32_t value) {
    if (value < 2) return value;
    uint32_t coefficient = 65537u / value;
    uint32_t remainder = 65537u % value;
    if (remainder == 1) return (1u - coefficient) & 0xffffu;
    uint32_t other = 1;
    do {
        const uint32_t quotient = value / remainder;
        const uint32_t next = value % remainder;
        other += quotient * coefficient;
        value = next;
        if (value == 1) return other;
        const uint32_t oldRemainder = remainder;
        remainder = oldRemainder % value;
        coefficient += (oldRemainder / value) * other;
    } while (remainder != 1);
    return (1u - coefficient) & 0xffffu;
}

// 1.13c BNClient+0x12260
// Expand eight words to 52; copy words in order rather than snapshotting the
// key, because retail permits aliases with observable sequential writes.
extern "C" void bn_idea_expand_key(const uint16_t* key, uint16_t* expanded) {
    for (uint32_t i = 0; i != 8; ++i) expanded[i] = key[i];
    uint32_t index = 0;
    for (uint32_t remaining = 44; remaining != 0; --remaining) {
        const uint32_t next = (index + 1) & 7;
        expanded[index + 8] = uint16_t((expanded[(index + 2) & 7] >> 7) |
                                               (uint32_t(expanded[next]) << 9));
        expanded += (index + 1) & 8;
        index = next;
    }
}
