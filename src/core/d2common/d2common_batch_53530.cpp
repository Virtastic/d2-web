// 1.13c D2Common batch: +0x53530, +0x547D0 (both called only from +0x5A730). Markers only; bodies not ported.
// 1.13c D2Common+0x53530: fills the 4x5x3 chest-class table (DAT 0x6FDF089C) by looking up the txt row named
// "Act %d%s Chest %s" (act 1..5, suffix list at 0x6FDEEF94, kind list at 0x6FDEEF88) in the table at
// DAT_6FDF0890 (row size 0x2C, base DAT_6FDF0894); missing -> 0. REG?: none (no args).
// 1.13c D2Common+0x547D0: builds per-level (0xE4 stride in sgptDataTables+0xBF8) treasure-class groups: for each
// level with +0x1D set, for 3-step quality bands 3..0x60, creates a group via +0x53780, then per item-type
// row (0x1A8 stride, count DAT_6FDEFB94) filters (+0x12A==0, +0x133!=0, +0xFD level band, +0xF6<100 flag)
// and adds via +0x54020/+0x53650. REG?: this (+0x53780 result) used as thiscall-style arg.
