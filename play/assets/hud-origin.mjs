// Where retail's 800x600 coordinates land on a WxH game view. SGD2FreeRes anchors every piece of the 800x600 screen to
// the centre, the bottom or an edge; at 800x600 every origin is (0,0) so the retail (?parity=1) view is unchanged.
//   centre       panels:            left = W/2 + (x-400), top = H/2 + (y-300)
//   bottom       belt, HUD text:    left = W/2 + (x-400), top = H - (600-y)
//   bottom-left  left skill button and popup: left = x, top = H - (600-y)
//   bottom-right right skill button, help:    left = W-800 + x, top = H - (600-y)
export const viewScale = (w, h) => Math.min(1, w / 800, h / 600);
export function hudOrigin(w, h, anchor = 'centre') {
  const scale = viewScale(w, h), cx = (w - 800 * scale) / 2;
  const bottom = anchor !== 'centre';
  return { scale, x: anchor.endsWith('left') ? 0 : anchor.endsWith('right') ? w - 800 * scale : cx,
    y: bottom ? h - 600 * scale : (h - 600 * scale) / 2 };
}
// The camera shift (SGD2FreeRes set_screen_shift): with a panel on one half the hero moves to the middle of the free half.
export const panelShift = (w, left, right) => (left === right ? 0 : right ? -Math.round(w / 4) : Math.round(w / 4));
