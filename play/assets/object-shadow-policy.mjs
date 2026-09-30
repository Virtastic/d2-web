// Object shadows have one table-owned exception to the COF layer rule.  Objects.txt's DrawUnder=2
// (`Z5`, RogueCorpse) is emitted below the floor and retail's draw list has its cel but no shadow.
// The engine owns the object's class/mode/anchor; this module reads only the retail table that names
// the exception and supplies a reason that the renderer carries into per-blit evidence.

function rows(bytes) {
  const lines = new TextDecoder().decode(bytes).replace(/\r/g, '').split('\n').filter(Boolean);
  const header = lines.shift().split('\t');
  const drawUnder = header.indexOf('DrawUnder'), bl0 = header.indexOf('BlocksLight0');
  if (drawUnder < 0 || bl0 < 0) throw new Error('Objects.txt lacks DrawUnder or BlocksLight0');
  // Keyed by ROW INDEX, the class id the engine uses: the Id column repeats (409 is both `99` and `px`).
  return new Map(lines.map((line, index) => {
    const values = line.split('\t');
    return [index, { drawUnder: Number(values[drawUnder]) || 0,
      blocksLight: Array.from({ length: 8 }, (_, m) => Number(values[bl0 + m]) || 0) }];
  }));
}

/** Retail's object-table gate before an otherwise Shadow-set COF layer is submitted. */
export function createObjectShadowPolicy(read) {
  let byClass = null;
  const load = () => {
    if (!byClass) byClass = rows(read('data\\global\\excel\\objects.txt'));
    return byClass;
  };
  return {
    decide(object) {
      const classId = Number(object?.classId);
      const row = load().get(classId) || { drawUnder: 0, blocksLight: null };
      const drawUnder = row.drawUnder;
      // D2Client 1.13c sets the unit's no-shadow bit (0x20, D2MOO's misnamed UNITFLAG_DRAWSHADOW) at object
      // creation and on every mode change when Objects.txt BlocksLight[mode] is 0; the shadow draw returns
      // early on it. Measured: 1,429/1,429 shadowed
      // object instances in the retail captures have BlocksLight[mode]=1; `99` and R1/R2/R3 have 0.
      if (row.blocksLight && !row.blocksLight[Number(object?.mode) | 0]) return { draw: false, reason: 'objects-blockslight-0', drawUnder };
      // Measured retail exception: Z5 / Objects.txt 56 is DrawUnder 2 and gets no `shadow dcc:`.
      // Do not generalise this to DrawUnder 1: the corpus has no such comparison.
      return drawUnder === 2
        ? { draw: false, reason: 'objects-drawunder-2', drawUnder }
        : { draw: true, reason: 'cof-shadow-layer', drawUnder };
    },
  };
}
