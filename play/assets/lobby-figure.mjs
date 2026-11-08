// The character figure the Battle.net lobby stands in the user band and beside the character details:
// the class's town-neutral COF layers, each component picked from the statstring's eleven gear bytes and
// dyed by its eleven colour bytes (the same byte meaning the character select reads from a save header).
// Returns canvases with offsets from the anchor (left x, bottom row y); a layer whose art is missing from
// the player's assets is listed in `missing` instead of throwing, so one absent file cannot blank the
// lobby. Nothing here reads the clock: the caller passes the elapsed milliseconds.
import { componentColourTransform } from './frontend.mjs';
import { decodeCof, decodeDcc } from './unit-formats.mjs';
import { playerClassInfo, dccDirection } from './unit-renderer.mjs';
import { tnFrame, drawsSince } from './charselect-anim.mjs';
import { parseStatstring } from './bnet-statstring.mjs';

const COMP = ['HD', 'TR', 'LG', 'RA', 'LA', 'RH', 'LH', 'SH', 'S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8'];
const UNITS_PAL = 'data\\global\\palette\\Units\\pal.dat';
const PL2 = 'data\\global\\palette\\Act1\\pal.pl2';

export function createLobbyFigure({ read, decodeDatPalette, componentToken = null, documentObject = globalThis.document }) {
  let units = null, pl2 = null;
  const dccCache = new Map(), cofCache = new Map(), made = new Map();
  const missing = new Set();
  const layerCanvas = (frame, transform) => {
    const c = documentObject.createElement('canvas');
    c.width = Math.max(1, frame.width); c.height = Math.max(1, frame.height);
    if (!frame.width || !frame.height) return c;
    const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
    for (let k = 0; k < frame.indexed.length; k++) if (frame.opaque[k]) {
      const s = (transform ? transform[frame.indexed[k]] : frame.indexed[k]) * 4;
      rgba[k * 4] = units[s]; rgba[k * 4 + 1] = units[s + 1]; rgba[k * 4 + 2] = units[s + 2]; rgba[k * 4 + 3] = 255;
    }
    c.getContext('2d').putImageData(new ImageData(rgba, frame.width, frame.height), 0, 0);
    return c;
  };
  const cached = (map, key, make) => { if (!map.has(key)) map.set(key, make()); return map.get(key); };
  // `stat`: a statstring. `elapsedMs`: time since the figure appeared. Returns { layers: [{canvas, left, top}], missing, frameIndex }, or null with no character data.
  function figure(stat, elapsedMs = 0) {
    const p = parseStatstring(stat);
    if (!p.valid || p.classId < 0 || p.classId > 6) return null;
    try { units ??= decodeDatPalette(read(UNITS_PAL)); pl2 ??= read(PL2); } catch { missing.add(UNITS_PAL); return null; }
    const app = p.equipment, colours = p.colors;
    const token = playerClassInfo(p.classId).token;
    const item = (i) => { const v = app[i]; if (v === undefined || v === 0xFF || v === 0) return null; const [code, wclass] = (componentToken?.(v) || '').split(' '); return code ? { code, wclass } : null; };
    const rh = item(5), lh = item(6), wclass = (rh?.wclass || lh?.wclass || 'hth').toUpperCase();
    const cofPath = `data\\global\\CHARS\\${token}\\COF\\${token}TN${wclass}.COF`;
    let cof;
    try { cof = cached(cofCache, cofPath, () => decodeCof(read(cofPath))); } catch { missing.add(cofPath); return null; }
    const frameIndex = tnFrame(drawsSince(Math.floor(Math.max(0, elapsedMs) * 60 / 1000), 0), cof.speed, cof.framesPerDirection);
    const key = `${token}|${wclass}|${app.join()}|${colours.join()}|${frameIndex}`;
    return cached(made, key, () => {
      const artDirection = dccDirection(0, cof.directions), frames = new Map(), transforms = new Map(), lost = [];
      for (const layer of cof.layerRecords) {
        const comp = COMP[layer.component]; if (!comp) continue;
        const v = app[layer.component], it = item(layer.component);
        const base = it ? it.code : (v >= 1 && v <= 3 ? ['lit', 'med', 'hvy'][v - 1] : 'lit');
        const path = `data\\global\\CHARS\\${token}\\${comp}\\${token}${comp}${base}TN${layer.weaponClass.toUpperCase()}.DCC`;
        try {
          const dcc = cached(dccCache, path, () => decodeDcc(read(path), { directions: [artDirection] }));
          if (artDirection >= dcc.directions || frameIndex >= dcc.framesPerDirection) throw new RangeError('no frame');
          frames.set(layer.component, dcc.directionRecords[artDirection].frames[frameIndex]);
          const t = componentColourTransform(pl2, colours[layer.component], read); if (t) transforms.set(layer.component, t);
        } catch { lost.push(path); missing.add(path); }
      }
      const layers = [];
      for (const component of cof.priority[0][frameIndex]) {
        const f = frames.get(component); if (!f) continue;
        layers.push({ component, canvas: layerCanvas(f, transforms.get(component) ?? null), left: f.xOffset, top: f.bottomUp ? f.yOffset : f.yOffset - f.height + 1 });
      }
      return { layers, missing: lost, frameIndex };
    });
  }
  return { figure, get missing() { return [...missing]; } };
}
