// The character screen's two skill rows: retail D2Client+0xBCEA0 calls 0x2FD30 once for UNITS_GetLeftSkill (D2Common
// ord 10828) and once for UNITS_GetRightSkill (ord 10507). 0x2FD30 reads the skill's SkillDesc record through 0x17F0
// (our d2_client_skilldesc_for_skill): the row title is wStrAlt (+0x0E) upper-cased, the damage cell is drawn only when
// wDescDam (+0x12) names a non-null entry of the 0xF5210 calc table (< 0x90), the attack-rating cell only when wDescAtt
// (+0x14) names a non-null entry of the 0xF52A0 table (< 0x48). The null entries below are read from those tables.
const DAM_NULL = new Set([0, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x5a, 0x69, 0x89]);
const ATT_NULL = new Set([0, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe]);
export const skillRowHasDamage = (descdam) => descdam < 0x90 && !DAM_NULL.has(descdam);
export const skillRowHasRating = (descatt) => descatt < 0x48 && !ATT_NULL.has(descatt);

/** [skill id, wStrAlt, wDescDam, wDescAtt] for the left then the right skill (8 ints; -1 for a hand with no record),
 *  then the 0x35270 damage cells (8 ints) and their two texts. */
export function readCharacterSkillRows(Module, selected) {
  const out = [];
  for (const id of [selected?.left ?? -1, selected?.right ?? -1]) {
    const p = id >= 0 && Module._d2_client_skilldesc_for_skill ? Number(Module.ccall('d2_client_skilldesc_for_skill', 'number', ['number'], [id])) : 0;
    if (!p) { out.push(id, -1, -1, -1); continue; }
    const v = new DataView(Module.HEAPU8.buffer, p, 0x16);
    out.push(id, v.getUint16(0x0e, true), v.getUint16(0x12, true), v.getUint16(0x14, true));
  }
  // The damage cell D2Client 0x35270 draws for a descdam 5 skill (d2client_35270_wire.cpp): [drawn, colour mode, min, max]
  // for the left then the right hand, then the two drawn texts ('' when that hand's cell is not the 0x35270 one).
  const cells = [], texts = [];
  for (const h of [0, 1]) {
    if (!Module._d2_client_char_skill_damage) { cells.push(0, 0, 0, 0); texts.push(''); continue; }
    const p = Module._malloc(16);
    try { const ok = Module.ccall('d2_client_char_skill_damage', 'number', ['number', 'bigint'], [h, BigInt(p)]);
      cells.push(...new Int32Array(Module.HEAPU8.buffer.slice(Number(p), Number(p) + 16)));
      texts.push(ok ? Module.ccall('d2_client_char_skill_damage_text', 'string', ['number'], [h]) : ''); } finally { Module._free(p); }
  }
  return out.concat(cells, texts);
}
