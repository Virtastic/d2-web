// Paperdoll identity is the encoded DC6 frame retail asks D2Gfx to blit.  Keep this
// separate from palette work: draw-list vendor-malah-act5 proves these four cels use
// pal=00000000, so an inventory graphic mismatch is a file/frame mismatch first.
export function fnv1a32(bytes) {
  let hash = 0x811c9dc5;
  for (const byte of bytes) { hash ^= byte; hash = Math.imul(hash, 0x01000193); }
  return hash >>> 0;
}

export function paperdollIdentity(item, frame, left, top, slot) {
  return { id: item.id, where: 'body', bodyLoc: item.bodyLoc, invFile: item.invFile,
    frame: 0, celHash: fnv1a32(frame.encoded), left, top, slot };
}
