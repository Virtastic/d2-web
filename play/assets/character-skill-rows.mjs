// The character screen's two skill rows: retail D2Client+0xBCEA0 calls 0x2FD30 once for UNITS_GetLeftSkill (D2Common
// ord 10828) and once for UNITS_GetRightSkill (ord 10507). 0x2FD30 reads the skill's SkillDesc record through 0x17F0
// (our d2_client_skilldesc_for_skill): the row title is wStrAlt (+0x0E) upper-cased, the damage cell is drawn only when
// wDescDam (+0x12) names a non-null entry of the 0xF5210 calc table (< 0x90), the attack-rating cell only when wDescAtt
// (+0x14) names a non-null entry of the 0xF52A0 table (< 0x48). The engine owns that gating (d2_client_skill_row_cells).
import { skillRowCells } from './client-rules.mjs';
export const skillRowHasDamage = (descdam) => (skillRowCells(descdam, 0) & 1) === 1;
export const skillRowHasRating = (descatt) => (skillRowCells(0, descatt) & 2) === 2;

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
