// The movies retail 1.13c plays before anything else: the two Blizzard logo films it boots into
// ahead of the trademark screen.
//
// Measured on this box (`the retail capture tool/frame-boot-linux.sh`, three retail launches, 2026-09-27):
// from the launch the screen is black for ~6 s while the game initialises, then
// `data\local\video\New_Bliz640x480.bik` plays in full (15.29 s, 367 frames at 24 fps),
// then `data\local\video\BlizNorth640x480.bik` (7.67 s, 184 frames), then ~2 s of black, then the
// trademark screen (client-parity C1g) and the menu. Each shot of the 70 s capture was matched to
// the movie frame it shows (ffmpeg decode, gain-fitted residual), which is how the two files are
// identified -- not from the string table: D2Launch.dll's table lists BlizNorth before New_BLIZ per
// resolution, while the screen order is the other way round.
//
// **The movies are NOT display-ramped.** Retail's capture of a movie frame matches our unramped RGB
// conversion to 0.14% RMSE and the same frame ramped by the shell's floor(v*1.55) to 2.6%, so the
// ramp the title screen needs (client-parity C1d; the movie is blitted straight to the primary
// surface, not through the game's palette) must be left OFF these phases. play/gameplay-shell.mjs
// skips applyDisplayRamp() while a movie is up; `s750` asserts a movie frame against retail's own.
//
// Prior art used: `reference/OpenDiablo2/d2game/d2gamescreen/blizzard_intro.go`
// names the same screen and its file (`/data/local/video/BlizNorth640x480.bik`) and
// `d2common/d2data/d2video/binkdecoder.go` the container header layout -- but its "decoder" parses
// the header only (GetNextFrame skips the audio packet and logs), so the video decode itself is not
// theirs. `reference/Diablerie`, `reference/AbyssEngine` and `reference/libd2` have no Bink video
// decoder at all. The decoder here is `blennies/unbikit` (MIT OR Apache-2.0), vendored under
// assets/vendor/ -- see vendor/README.md.
import { createBikDecoder } from './vendor/unbikit.js';
import { createBikAudioPlayer } from './cinematic-audio.mjs';

/**
 * Retail's boot movies, in the order they reach the screen (measured, see the file header). The path
 * spelling is the one in the disc listfile (`retail/listfiles/d2.lst`); MPQ lookup is case-insensitive.
 *
 * `phase` is the front-end phase each one owns in play/gameplay-shell.mjs' titlePhaseLog.
 *
 * Retail ships three resolutions of each -- 640x480, 640x240 and 320x480 (`D2Launch.dll`'s table) --
 * and picks by the display it is running on. Our canvas is 800x600, the 4:3 screen the 640x480 pair is
 * for; the other two exist for the letterboxed/stretched modes we do not draw.
 */
export const BOOT_MOVIES = [
  { phase: 'blizzard', path: 'data\\local\\video\\New_Bliz640x480.bik' },
  { phase: 'blizzard-north', path: 'data\\local\\video\\BlizNorth640x480.bik' },
];

/**
 * Bink stores YUV 4:2:0; the screen wants RGB. This is the integer BT.601 limited-range conversion
 * (the one every Bink player uses), and it is measured against retail rather than assumed: decoding
 * frame 276 of New_Bliz and converting here puts the picture within 0.14% RMSE of retail's own
 * screenshot of that frame (`the retail capture tool/frame-boot-linux.sh`).
 */
export function yuv420ToRgba(yuv, lineSize, width, height) {
  const [ys, us, vs] = lineSize;
  const cw = width >> 1, ch = height >> 1;
  const uOff = ys * height, vOff = uOff + us * ch;
  const out = new Uint8ClampedArray(width * height * 4);
  for (let y = 0; y < height; y++) {
    const yRow = y * ys, cRow = (y >> 1), outRow = y * width * 4;
    for (let x = 0; x < width; x++) {
      const Y = yuv[yRow + x];
      const U = yuv[uOff + cRow * us + (x >> 1)] - 128;
      const V = yuv[vOff + cRow * vs + (x >> 1)] - 128;
      const c = 298 * (Y - 16) + 128;
      const o = outRow + x * 4;
      const r = (c + 409 * V) >> 8, g = (c - 100 * U - 208 * V) >> 8, b = (c + 516 * U) >> 8;
      out[o] = r < 0 ? 0 : r > 255 ? 255 : r;
      out[o + 1] = g < 0 ? 0 : g > 255 ? 255 : g;
      out[o + 2] = b < 0 ? 0 : b > 255 ? 255 : b;
      out[o + 3] = 255;
    }
  }
  return out;
}

/** The .bik container's own header, read from the file's bytes (the phase's identity, level 1). */
export function parseBikHeader(bytes) {
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const magic = String.fromCharCode(bytes[0], bytes[1], bytes[2], bytes[3]);
  if (magic.slice(0, 3) !== 'BIK') throw new Error(`not a Bink file: ${JSON.stringify(magic)}`);
  const at = (o) => view.getUint32(o, true);
  return { magic, revision: bytes[3], totalSize: at(4), numFrames: at(8), width: at(20),
    height: at(24), fpsDividend: at(28), fpsDivider: at(32), flags: at(36), numAudioTracks: at(40) };
}

/**
 * Open one movie from the MPQ reader. The decoder streams from a Blob over the file's bytes; frames
 * are decoded in order (Bink inter frames depend on their predecessors), one at a time, and the ones
 * the clock has passed are dropped rather than drawn.
 *
 * `step(elapsedMs)` is what the shell calls each frame; it is async because a decode is, and it
 * yields between frames so a slow movie cannot stall the page's own loop. `draw()` is synchronous and
 * always blits the frame the last completed step left, so a frame never tears.
 */
export async function openMovie({ read, path, documentObject = globalThis.document,
  AudioContextClass = globalThis.AudioContext || globalThis.webkitAudioContext, gestureTarget = globalThis.window,
  audioPlayer = null, isCurrent = () => true, createDecoder = createBikDecoder }) {
  const bytes = read(path);
  const header = parseBikHeader(bytes);
  // A title-film row calls openMovie from the user's release handler. Create its Web Audio
  // context before the decoder's asynchronous startup so the trusted gesture can activate it.
  // The boot sequence passes its already-created shared player, so this is only owned here for
  // callers such as the explicit Cinematics menu.
  const ownsAudio = !audioPlayer;
  const audio = header.numAudioTracks ? (audioPlayer ?? createBikAudioPlayer(AudioContextClass, gestureTarget)) : null;
  let decoder;
  try { decoder = await createDecoder(new Blob([bytes])); }
  catch (error) { if (ownsAudio) audio?.close?.(); throw error; }
  // The front end may skip this phase while the decoder is being constructed. Do not decode or
  // create any phase-owned state once its opener has gone stale.
  if (!isCurrent()) { if (ownsAudio) audio?.close?.(); return null; }
  const fps = header.fpsDivider ? header.fpsDividend / header.fpsDivider : 0;
  if (!decoder.isSupported) {
    if (ownsAudio) audio?.close?.();
    throw new Error(`${path}: unbikit cannot decode this Bink stream`);
  }
  const canvas = documentObject.createElement('canvas');
  canvas.width = header.width; canvas.height = header.height;
  const ctx2d = canvas.getContext('2d', { willReadFrequently: true });
  const state = {
    path, header, fps,
    frameIndex: -1,             // the last frame DECODED into the buffer
    drawnIndex: -1,             // the last frame BLITTED to the movie canvas
    frames: header.numFrames,
    durationMs: fps ? (header.numFrames / fps) * 1000 : 0,
    error: '',
  };
  let rgba = null;
  let stepping = false;   // one decode at a time: step() is called from two clocks
  let audioStopped = false;
  const blit = () => {
    if (!rgba) return false;
    ctx2d.putImageData(new ImageData(rgba, header.width, header.height), 0, 0);
    state.drawnIndex = state.frameIndex;
    return true;
  };
  /**
   * The film is 640x480 and the front end's canvas is 800x600, so it has to be scaled to fill the
   * screen retail filled. The scaling is done HERE, with an explicit nearest-neighbour mapping --
   * source pixel `floor(x * sw / dw)` -- rather than by letting drawImage resample: the browser's
   * nearest rule is unspecified (centre or corner sample), and a level-3 comparison has to know
   * exactly which source pixel a canvas pixel came from. the retail capture tool/frame-boot-linux.sh resamples
   * retail's own capture with this same floor rule, so the two sides are the same operation.
   * The buffer is reused between frames; only a size or frame change rebuilds it.
   */
  let scaled = { w: 0, h: 0, index: -1, img: null };
  const scaleTo = (dstW, dstH) => {
    if (scaled.w !== dstW || scaled.h !== dstH || !scaled.img) {
      scaled = { w: dstW, h: dstH, index: -1, img: new ImageData(dstW, dstH) };
    }
    if (scaled.index === state.drawnIndex) return scaled.img;
    const out = scaled.img.data;
    for (let y = 0; y < dstH; y++) {
      const srcRow = Math.floor(y * header.height / dstH) * header.width, dstRow = y * dstW * 4;
      for (let x = 0; x < dstW; x++) {
        const s = (srcRow + Math.floor(x * header.width / dstW)) * 4, d = dstRow + x * 4;
        out[d] = rgba[s]; out[d + 1] = rgba[s + 1]; out[d + 2] = rgba[s + 2]; out[d + 3] = 255;
      }
    }
    scaled.index = state.drawnIndex;
    return scaled.img;
  };
  const decodeOne = async () => {
    const frame = await decoder.getNextFrame();
    if (!frame || !frame.videoFrame) return false;
    // A skip may change title phase while this asynchronous Bink decode is in flight. Do not let
    // its eventual completion enqueue sound after the phase transition cleared the old movie queue.
    if (!isCurrent() || audioStopped) return false;
    audio?.play(frame.audioTracks);
    rgba = yuv420ToRgba(frame.videoFrame.yuv, frame.videoFrame.lineSize, header.width, header.height);
    state.frameIndex++;
    return true;
  };
  try { await decodeOne(); }
  catch (error) { if (ownsAudio) audio?.close?.(); throw error; }
  blit();
  return {
    ...state,
    get frameIndex() { return state.frameIndex; },
    get drawnIndex() { return state.drawnIndex; },
    get error() { return state.error; },
    get finished() { return state.durationMs > 0 && state.frameIndex >= header.numFrames - 1; },
    get canvas() { return canvas; },
    get audioEvidence() { return audio?.evidence ?? null; },
    stopAudio() { audioStopped = true; return audio?.stop?.(); },
    // A late opener can belong to an old phase after a newer movie has begun using the shared
    // audio player. Discard only this movie's future packets; do not stop the shared player's queue.
    discard() { audioStopped = true; return ownsAudio ? audio?.close?.() : undefined; },
    close() { audioStopped = true; return ownsAudio ? audio?.close?.() : audio?.stop?.(); },
    /**
     * Decode every frame the clock has reached. Returns the number of frames decoded this call.
     * Two callers reach this (the frame loop and the shell's 200 ms timer, one of which a background
     * tab does not run), so it is re-entrancy guarded: a second call while a decode is in flight
     * returns immediately rather than advancing `frameIndex` twice for the same frame.
     */
    async step(elapsedMs) {
      if (state.error || stepping) return 0;
      stepping = true;
      try {
        const target = Math.min(Math.floor((elapsedMs / 1000) * fps), header.numFrames - 1);
        let n = 0;
        while (state.frameIndex < target) { if (!await decodeOne()) break; n++; }
        if (state.frameIndex > state.drawnIndex) blit();
        return n;
      } catch (error) { state.error = String(error); return 0; }
      finally { stepping = false; }
    },
    /** Blit the newest decoded frame without decoding (a skipped movie still shows its last frame). */
    flush() { if (state.frameIndex > state.drawnIndex) blit(); return state.drawnIndex; },
    /**
     * Decode to an absolute frame and blit it -- how a row pins a frame to compare against a retail
     * capture (s750, the way s198 pins the logo fire's tick). Backwards means starting the film again:
     * Bink inter frames are coded against their predecessors, so frame k is only reachable by decoding
     * 0..k. `decoder.reset()` re-streams the file from the beginning, which is why a sweep over a whole
     * film costs one rewind per pass and not a rewind per candidate.
     */
    async seek(index) {
      if (state.error) return -1;
      const target = Math.max(0, Math.min(index, header.numFrames - 1));
      if (target < state.frameIndex) {
        try {
          decoder.reset();
          state.frameIndex = -1; state.drawnIndex = -1; rgba = null; scaled.index = -1;
        } catch (error) { state.error = String(error); return -1; }
      }
      if (target > state.frameIndex) {
        try { while (state.frameIndex < target) { if (!await decodeOne()) break; } }
        catch (error) { state.error = String(error); }
      }
      blit();
      return state.drawnIndex;
    },
    /** Draw the movie scaled onto the screen, exactly as retail fills the display with it. */
    draw(outCtx, width, height) {
      outCtx.fillStyle = '#000'; outCtx.fillRect(0, 0, width, height);
      if (state.drawnIndex < 0) return false;
      outCtx.putImageData(scaleTo(width, height), 0, 0);
      return true;
    },
    /**
     * Identity evidence for the row: the RGB the decoder holds for the DRAWN frame at a few points,
     * paired with the canvas pixel the browser put there. `drawn` is read back from the canvas after a
     * render, so it is the blit being compared, not this module's report of what it asked for.
     */
    sample(canvasCtx, width, height) {
      if (state.drawnIndex < 0 || !rgba) return null;
      const img = canvasCtx.getImageData(0, 0, width, height).data;
      const points = [[0.2, 0.2], [0.5, 0.5], [0.8, 0.8], [0.35, 0.6], [0.65, 0.35]];
      return points.map(([fx, fy]) => {
        const cx = Math.floor(fx * width), cy = Math.floor(fy * height);
        // The same mapping draw() scaled with -- floor(cx * sw / dw) -- so this reads the source pixel
        // that canvas pixel actually came from, not a similarly-placed one.
        const sx = Math.floor(cx * header.width / width), sy = Math.floor(cy * header.height / height);
        const s = (sy * header.width + sx) * 4, d = (cy * width + cx) * 4;
        return { cx, cy, sx, sy,
          source: [rgba[s], rgba[s + 1], rgba[s + 2]],
          drawn: [img[d], img[d + 1], img[d + 2]] };
      });
    },
    evidence() {
      return { path, phase: null, frames: header.numFrames, fps, width: header.width,
        height: header.height, revision: header.revision, numFramesFromHeader: header.numFrames,
        durationMs: Math.round(state.durationMs), frameIndex: state.frameIndex,
        drawnIndex: state.drawnIndex, finished: state.frameIndex >= header.numFrames - 1,
        error: state.error };
    },
  };
}
