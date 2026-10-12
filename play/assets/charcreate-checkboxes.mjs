// Retail character-create checkboxes.  This stays separate from charcreate.mjs so the screen
// owns layout/flow while this component owns the two-cel clickbox identity and hit testing.
//
// Prior art: OpenDiablo2 d2game/d2gamescreen/select_hero_class.go recovers these anchors;
// d2core/d2ui/checkbox.go names clickbox.dc6, uses fechar, and maps state false/true to cel 0/1.
import { recordDc6 } from './drawlist.mjs';

export const CHARACTER_CREATE_CHECKBOXES = Object.freeze({
  asset: 'data\\global\\ui\\FrontEnd\\clickbox.dc6',
  entries: Object.freeze([
    // Retail's own draw list with a class chosen (scene `create-named`, the retail captures):
    // clickbox.dc6 frame 1 is a cel at x=319 y=540 h=16 -> top-left (319, 525) and frame 0 one at
    // x=319 y=560 -> (319, 545); the Font16 labels start at x=339 with their bottoms at 539 and 559,
    // i.e. tops 524 and 544. OpenDiablo2's 318/526/339/526 and 318/548/339/548 are 1 px right and 1-4 px
    // low against that, which this corrects.
    Object.freeze({ key: 'expansion', x: 319, y: 525, labelX: 339, labelY: 524, initial: true, labelIndex: 11077 }),
    Object.freeze({ key: 'hardcore', x: 319, y: 545, labelX: 339, labelY: 544, initial: false, labelIndex: 5126 }),
  ]),
});

/**
 * Retail registers the Character Create clickboxes as D2Win type-6 mode-2 controls. Their toggle
 * callback runs on pointer-down; release and cancellation only end the gesture and cannot undo it.
 */
export function createCharacterCreateCheckboxInput(toggle) {
  let armed = null;
  return {
    press(token) {
      armed = typeof token === 'string' && token.startsWith('checkbox-') ? token : null;
      return armed ? toggle(armed.slice('checkbox-'.length)) : null;
    },
    release(token) {
      if (armed === token) armed = null;
      return null;
    },
    cancel() { armed = null; },
  };
}

function celCanvas(frame, palette, documentObject) {
  const canvas = documentObject.createElement('canvas');
  canvas.width = Math.max(1, frame.width); canvas.height = Math.max(1, frame.height);
  const rgba = new Uint8ClampedArray(frame.width * frame.height * 4);
  for (let i = 0; i < frame.indices.length; i++) {
    if (!frame.opaque[i]) continue;
    rgba.set(palette.subarray(frame.indices[i] * 4, frame.indices[i] * 4 + 4), i * 4);
  }
  canvas.getContext('2d').putImageData(new ImageData(rgba, canvas.width, canvas.height), 0, 0);
  return canvas;
}

export function createCharacterCreateCheckboxes({ read, decodeDc6, palette, smallFont, string = () => '', documentObject = globalThis.document }) {
  const dc6 = decodeDc6(read(CHARACTER_CREATE_CHECKBOXES.asset));
  if (dc6.frames.length !== 2) throw new Error(`${CHARACTER_CREATE_CHECKBOXES.asset} has ${dc6.frames.length} cels, expected 2`);
  const cels = dc6.frames.map((frame) => celCanvas(frame, palette, documentObject));
  const states = Object.fromEntries(CHARACTER_CREATE_CHECKBOXES.entries.map((entry) => [entry.key, entry.initial]));
  let drawn = [];
  const box = (entry) => ({ x: entry.x, y: entry.y, w: cels[0].width, h: cels[0].height });
  const inBox = (point, target) => point.x >= target.x && point.x < target.x + target.w && point.y >= target.y && point.y < target.y + target.h;

  return {
    get evidence() {
      return { asset: CHARACTER_CREATE_CHECKBOXES.asset, cels: cels.length, states: { ...states }, drawn: drawn.map((blit) => ({ ...blit })) };
    },
    // What the two boxes say right now, for the caller that has to hand them to the engine: these
    // ARE retail's client save flags (Expansion/Hardcore), and the shell is what turns them into
    // d2_session_set_client_flags. Nothing else in the client keeps a second copy of them.
    states() { return { ...states }; },
    targets() {
      return Object.fromEntries(CHARACTER_CREATE_CHECKBOXES.entries.map((entry) => {
        const target = box(entry);
        // The two retail cels overlap by a couple of rows.  An inner top-left pixel is unambiguous,
        // unlike the visual centre of the upper box, and is still inside the exact hit rect.
        return [entry.key, [target.x + 1, target.y + 1]];
      }));
    },
    click(point) {
      const entry = CHARACTER_CREATE_CHECKBOXES.entries.find((candidate) => inBox(point, box(candidate)));
      if (!entry) return null;
      return this.toggle(entry.key);
    },
    toggle(key) { if (!(key in states)) return null; states[key] = !states[key]; return key; },
    hitTest(point) {
      return CHARACTER_CREATE_CHECKBOXES.entries.find((candidate) => inBox(point, box(candidate)))?.key ?? null;
    },
    contains(key, point) {
      const entry = CHARACTER_CREATE_CHECKBOXES.entries.find((candidate) => candidate.key === key);
      return Boolean(entry && inBox(point, box(entry)));
    },
    draw(ctx) {
      drawn = CHARACTER_CREATE_CHECKBOXES.entries.map((entry) => {
        const frame = states[entry.key] ? 1 : 0;
        // Retail draws each checkbox's LABEL before its box (the retail captures records
        // 131-150 and 151-159); the two are at the same band and the draw list is identity evidence.
        smallFont.draw(ctx, string(entry.labelIndex) || (entry.key === 'hardcore' ? 'Hardcore' : 'Expansion Character'), entry.labelX, entry.labelY);
        ctx.drawImage(cels[frame], entry.x, entry.y);
        recordDc6(CHARACTER_CREATE_CHECKBOXES.asset, frame, dc6.frames[frame], entry.x, entry.y);
        // requested and drawn are deliberately separate: s375 rejects a fallback or swapped cel.
        return { key: entry.key, requestedFrame: frame, drawnFrame: frame, file: CHARACTER_CREATE_CHECKBOXES.asset, x: entry.x, y: entry.y };
      });
    },
  };
}
