// D2Win's type-1 edit control accepts ordinary WM_CHAR bytes through a bitset recovered from 1.13c.
export function isRetailEditBoxCharacter(key) {
  if (typeof key !== 'string' || key.length !== 1) return false;
  const code = key.charCodeAt(0);
  return (code >= 0x20 && code <= 0x7e) || code === 0x91 || code === 0x92 || (code >= 0xa0 && code <= 0xfe);
}
