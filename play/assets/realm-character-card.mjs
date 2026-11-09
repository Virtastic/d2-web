// The character-select card for one realm CHARLIST2 entry. The entry's flags u16 is the statstring
// status byte: 0x20 expansion, 0x04 hardcore, 0x08 dead, 0x40 ladder.
export function realmCharacterCard(c) {
  return {
    name: c.name,
    playerClass: c.charClass,
    level: c.level,
    expansion: (c.flags & 0x20) !== 0,
    hardcore: (c.flags & 0x04) !== 0,
    dead: (c.flags & 0x08) !== 0,
    ladder: (c.flags & 0x40) !== 0,
  };
}
