// Object shadows have one table-owned exception to the COF layer rule.  Objects.txt's DrawUnder=2
// (`Z5`, RogueCorpse) is emitted below the floor and retail's draw list has its cel but no shadow.
// The engine owns the object's class/mode/anchor; this module reads only the retail table that names
// the exception and supplies a reason that the renderer carries into per-blit evidence.

function rows(bytes) {
  const lines = new TextDecoder().decode(bytes).replace(/\r/g, '').split('\n').filter(Boolean);
  const header = lines.shift().split('\t');
  const id = header.indexOf('Id'), drawUnder = header.indexOf('DrawUnder');
  if (id < 0 || drawUnder < 0) throw new Error('Objects.txt lacks Id or DrawUnder');
  return new Map(lines.map((line) => {
    const values = line.split('\t');
    return [Number(values[id]), Number(values[drawUnder]) || 0];
  }));
}

/** Retail's object-table gate before an otherwise Shadow-set COF layer is submitted. */
export function createObjectShadowPolicy(read) {
  let drawUnderByClass = null;
  const load = () => {
    if (!drawUnderByClass) drawUnderByClass = rows(read('data\\global\\excel\\objects.txt'));
    return drawUnderByClass;
  };
  return {
    decide(object) {
      const classId = Number(object?.classId);
      const drawUnder = load().get(classId) || 0;
      // Measured retail exception: Z5 / Objects.txt 56 is DrawUnder 2 and gets no `shadow dcc:`.
      // Do not generalise this to DrawUnder 1: the corpus has no such comparison.
      return drawUnder === 2
        ? { draw: false, reason: 'objects-drawunder-2', drawUnder }
        : { draw: true, reason: 'cof-shadow-layer', drawUnder };
    },
  };
}
