import { decodeDatPalette } from './d2-formats.mjs';
import { decodeDcc } from './unit-formats.mjs';
import { indexedToRgba } from './unit-renderer.mjs';

const MISSILE_CEL = Object.freeze({ 1: 'Javelin' });

// Retail Missiles.txt supplies CelFile. This first presentation slice freezes the owned Throw
// missile and keeps the class-to-file lookup explicit until the table export carries CelFile.
export function createMissileRenderer(read, documentObject = globalThis.document) {
  const palette = decodeDatPalette(read('data\\global\\palette\\units\\pal.dat'));
  const decoded = new Map(), rasters = new Map();
  return { draw(ctx, x, y, missile, direction = 0) {
    const celFile = MISSILE_CEL[missile?.classId];
    if (!celFile) return null;
    let dcc = decoded.get(celFile);
    if (!dcc) { dcc = decodeDcc(read(`data\\global\\missiles\\${celFile}.dcc`)); decoded.set(celFile, dcc); }
    const dir = Math.min(dcc.directions - 1, Math.max(0, Math.trunc(direction)));
    const record = dcc.directionRecords[dir], frameIndex = 0, frame = record.frames[frameIndex];
    const key = `${celFile}:${dir}:${frameIndex}`; let canvas = rasters.get(key);
    if (!canvas) { canvas = documentObject.createElement('canvas'); canvas.width = frame.width; canvas.height = frame.height;
      canvas.getContext('2d').putImageData(new ImageData(indexedToRgba(frame.indexed, frame.opaque, palette),frame.width,frame.height),0,0);rasters.set(key,canvas); }
    ctx.drawImage(canvas,Math.round(x+frame.xOffset),Math.round(y+(frame.bottomUp?frame.yOffset:frame.yOffset-frame.height+1)));
    let pixelHash=0x811c9dc5,opaque=0;for(let i=0;i<frame.indexed.length;i++){pixelHash^=frame.indexed[i];pixelHash=Math.imul(pixelHash,0x01000193);opaque+=frame.opaque[i];}
    return {classId:missile.classId,celFile,direction:dir,frame:frameIndex,width:frame.width,height:frame.height,opaque,pixelHash:pixelHash>>>0};
  }};
}
