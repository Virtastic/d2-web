import { decodeDatPalette } from './d2-formats.mjs';
import { decodeDcc } from './unit-formats.mjs';
import { indexedToRgba } from './unit-renderer.mjs';

const MISSILE_CEL = Object.freeze({ 1: 'Javelin' });

// Retail Missiles.txt supplies CelFile: `celFile(classId)` is the engine's table (d2_missile_celfile);
// without it only the Throw slice's Javelin is known. The frame advances with `tick`.
// TODO(client-parity F20): the missile's facing and Missiles.txt draw mode (Trans/additive) are not
// on the unit record yet -- direction 0, drawn opaque.
export function createMissileRenderer(read, documentObject = globalThis.document, celFile = null) {
  const palette = decodeDatPalette(read('data\\global\\palette\\units\\pal.dat'));
  const decoded = new Map(), rasters = new Map();
  return { draw(ctx, x, y, missile, direction = 0, tick = 0) {
    const cel = (celFile && celFile(missile?.classId)) || MISSILE_CEL[missile?.classId];
    if (!cel) return null;
    let dcc = decoded.get(cel);
    if (dcc === undefined) { try { dcc = decodeDcc(read(`data\\global\\missiles\\${cel}.dcc`)); } catch { dcc = null; } decoded.set(cel, dcc); }
    if (!dcc) return null;
    const celFile_ = cel;
    const dir = Math.min(dcc.directions - 1, Math.max(0, Math.trunc(direction)));
    const record = dcc.directionRecords[dir], frameIndex = Math.max(0, Math.trunc(tick)) % record.frames.length, frame = record.frames[frameIndex];
    const key = `${celFile_}:${dir}:${frameIndex}`; let canvas = rasters.get(key);
    if (!canvas) { canvas = documentObject.createElement('canvas'); canvas.width = frame.width; canvas.height = frame.height;
      canvas.getContext('2d').putImageData(new ImageData(indexedToRgba(frame.indexed, frame.opaque, palette),frame.width,frame.height),0,0);rasters.set(key,canvas); }
    ctx.drawImage(canvas,Math.round(x+frame.xOffset),Math.round(y+(frame.bottomUp?frame.yOffset:frame.yOffset-frame.height+1)));
    let pixelHash=0x811c9dc5,opaque=0;for(let i=0;i<frame.indexed.length;i++){pixelHash^=frame.indexed[i];pixelHash=Math.imul(pixelHash,0x01000193);opaque+=frame.opaque[i];}
    return {classId:missile.classId,celFile:celFile_,direction:dir,frame:frameIndex,width:frame.width,height:frame.height,opaque,pixelHash:pixelHash>>>0};
  }};
}
