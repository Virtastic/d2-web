// F91: EXIT DIABLO II.
//
// Retail ENDS on this button: OpenDiablo2's main menu handler is `os.Exit(0)`
// (d2game/d2gamescreen/main_menu.go `onExitButtonClicked`) and OpenD2's front end agrees that EXIT
// terminates the client rather than entering another menu. On a realm connection it logs out first; either way the process is gone. A
// browser page cannot do that, and must not try to fake it: whether a tab closes is the browser's
// decision, not the page's, and `window.close()` is refused outright for a tab the page did not
// open. What the client CAN do is everything up to that line, and then say what happened:
//
//   1. run the exit fade -- retail's screen goes black as the client tears down, and the last frame
//      the process ever draws is black;
//   2. ATTEMPT `window.close()`;
//   3. if the tab is still here when the attempt returns (the browser refused), put a notice on the
//      screen naming the one action left to the player, instead of stranding them on a black page.
//
// The client used to rebuild the title menu here, which is a button that does nothing: it returned
// the player to the menu they had just left.
//
// The fade DURATION is chosen, not captured -- retail's exit is the one screen no capture can reach
// (the process is gone by the time the frame would be read), so this is a declared number a later
// capture can replace, kept in EXIT_FADE_MS where it is visible rather than buried in a draw call.
export const EXIT_FADE_MS = 1000;
// How long after the close attempt the notice goes up. The attempt is synchronous and either closes
// the tab (nothing after this runs) or is refused (the tab survives and the notice is the truth); the
// delay only keeps the black frame from flashing straight into text.
export const EXIT_PANEL_DELAY_MS = 250;
// The notice. Retail has no such string -- retail could close itself, so it needed none -- so it is
// OURS and is declared as such. It is drawn in the front
// end's own font so the one screen the browser leaves behind still looks like this client.
export const EXIT_PANEL_TEXT = 'You can close this tab.';
// The 800x600 front-end frame the notice is laid out in, like every other front-end screen.
export const EXIT_PANEL_FRAME = { width: 800, height: 600 };

/**
 * The exit sequence's state machine. Owns one clock (the page's own) and the two timers that fire
 * after the fade; the shell owns when it starts and what `close` does.
 *
 * `font` is the front-end font the notice is drawn in (its DC6 supplies the glyphs and its `path`
 * the identity a row compares against what was requested); a missing font draws nothing, and the
 * evidence says so rather than pretending a notice went up.
 */
export function createExitSequence({ now = () => performance.now(), close = () => globalThis.window?.close?.(),
  font = null, fadeMs = EXIT_FADE_MS, panelDelayMs = EXIT_PANEL_DELAY_MS, text = EXIT_PANEL_TEXT } = {}) {
  let s = null, timers = [];
  const clear = () => { for (const t of timers) clearTimeout(t); timers = []; };
  const alphaOf = () => (s ? Math.min(1, Math.max(0, (now() - s.startedAt) / fadeMs)) : 0);
  const drawNotice = (ctx, w, h) => {
    const frame = EXIT_PANEL_FRAME;
    const scale = Math.min(w / frame.width, h / frame.height);
    const ox = Math.round((w - frame.width * scale) / 2), oy = Math.round((h - frame.height * scale) / 2);
    const glyphs = [];
    if (!font) return { text, glyphs, error: 'no front-end font' };
    ctx.save();
    ctx.imageSmoothingEnabled = false;
    ctx.translate(ox, oy); ctx.scale(scale, scale);
    font.drawAnchored(ctx, text, Math.round((frame.width - font.measure(text)) / 2),
      Math.round(frame.height / 2 + font.lineHeight / 2), (g) => glyphs.push({ character: g.character, index: g.index, path: g.path, x: g.x, y: g.y }));
    ctx.restore();
    return { text, glyphs, error: '' };
  };
  return {
    get active() { return s !== null; },
    get evidence() {
      if (!s) return { active: false, phase: null, alpha: 0, alphaAtDraw: 0, overlayDrawn: false, fadeMs, closeAttempted: false, closeError: '', text, drawn: null };
      return { active: true, phase: s.phase, alpha: alphaOf(), alphaAtDraw: s.alphaAtDraw ?? 0, overlayDrawn: s.overlayDrawn ?? false,
        fadeMs, closeAttempted: s.closeAttempted, closeError: s.closeError ?? '', text, drawn: s.drawn };
    },
    /** Begin the exit. Idempotent: a second click during the fade changes nothing. */
    start() {
      if (s) return false;
      s = { startedAt: now(), phase: 'fading', closeAttempted: false, closeError: '', drawn: null };
      // Timers, not frames: the frame loop does not run in a background tab, and both the close
      // attempt and the notice must happen whether or not a frame was drawn since the click.
      timers.push(setTimeout(() => {
        if (!s) return;
        s.closeAttempted = true;
        try { close(); } catch (error) { s.closeError = String(error); }
      }, fadeMs));
      // The attempt above either closed the tab -- this never runs -- or was refused, and the notice
      // is the honest answer to "the tab is still here".
      timers.push(setTimeout(() => { if (s && s.closeAttempted) s.phase = 'panel'; }, fadeMs + panelDelayMs));
      return true;
    },
    reset() { clear(); s = null; },
    /** The overlay: the fade, then the notice. `__d2ExitNoFade` is the row's control (s927): the
     *  wrong variant of this path, which never fades, and through which the row's own fade comparison
     *  must fail. `alphaAtDraw` is recorded HERE, at the moment the frame is painted, not re-read from
     *  the clock afterwards: the alpha a row compares its sampled pixel against has to be the alpha
     *  that pixel was drawn at. */
    draw(ctx, w, h) {
      if (!s) return;
      // `alphaAtDraw`/`overlayDrawn` are recorded unconditionally, including under the control flag:
      // the wrong variant reports the alpha the fade WOULD have been drawn at, so a row's comparison
      // still fails on the pixel it sampled rather than on a flag that says "no overlay".
      const a = alphaOf();
      s.alphaAtDraw = a; s.overlayDrawn = a > 0 && !globalThis.__d2ExitNoFade;
      if (s.overlayDrawn) { ctx.save(); ctx.globalAlpha = a; ctx.fillStyle = '#000'; ctx.fillRect(0, 0, w, h); ctx.restore(); }
      if (s.phase === 'panel') s.drawn = drawNotice(ctx, w, h);
    },
  };
}
