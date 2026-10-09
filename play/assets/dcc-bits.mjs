// Bounded LSB-first reader for DCC streams.
export class Bits {
  constructor(bytes, bit = 0, limit = bytes.length * 8) { this.bytes = bytes; this.bit = bit; this.limit = limit; }
  read(count, label) {
    if (count < 0 || count > 32 || this.bit + count > this.limit) {
      throw new RangeError(`truncated DCC ${label} at bit ${this.bit} reading ${count}, limit ${this.limit}`);
    }
    // Consume at most five byte fragments, including an unaligned 32-bit field.
    // Each fragment fits in eight bits; the final shift can set the sign bit,
    // so retain the unsigned conversion below.
    let value = 0, consumed = 0, bit = this.bit;
    while (consumed < count) {
      const offset = bit & 7, take = Math.min(8 - offset, count - consumed);
      value |= ((this.bytes[bit >>> 3] >>> offset) & ((1 << take) - 1)) << consumed;
      bit += take; consumed += take;
    }
    this.bit = bit;
    return value >>> 0;
  }
  signed(count, label) {
    const v = this.read(count, label);
    if (!count || count === 32) return count === 32 ? v | 0 : 0;
    return v & 2 ** (count - 1) ? v - 2 ** count : v;
  }
}
