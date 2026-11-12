// The skill buttons' on-screen parts: the Attack / Use hint, the skill wheel and the panel list. Drawn as
// a DOM layer over the canvas, like the controller's other grids; nothing here is drawn into the game.
import { WHEEL_SHOWN, PANELS, ACTIONS, actionLabel, wedgeCenter } from './pad-skill-sheet.mjs';

export function wheelEntries(ids, at, nameOf) {
  const start = Math.floor(at / WHEEL_SHOWN) * WHEEL_SHOWN, count = Math.min(WHEEL_SHOWN, ids.length - start);
  return Array.from({ length: count }, (_, i) => {
    const id = ids[start + i], a = wedgeCenter(i, count);
    return { id, label: id < 0 ? actionLabel(id, true) : nameOf(id) || `Skill ${id}`, on: start + i === at, x: Math.cos(a), y: Math.sin(a) };
  });
}

export function createSkillOverlay(viewport, font) {
  const make = (css) => { const el = document.createElement('div'); Object.assign(el.style, { position: 'absolute', display: 'none', pointerEvents: 'none', font, color: '#e8d8a8', zIndex: 30 }, css); viewport.appendChild(el); return el; };
  const hint = make({ left: '50%', bottom: '18%', transform: 'translateX(-50%)', background: 'rgba(0,0,0,0.55)', padding: '2px 8px' });
  const wheel = make({ left: '0', top: '0', width: '100%', height: '100%' });
  const strip = make({ left: '50%', top: '20%', transform: 'translateX(-50%)', background: 'rgba(0,0,0,0.8)', padding: '6px 12px', border: '1px solid #4a4a4a' });
  let last = '';
  return {
    // play: makeSkillPlay's object; ids: the wheel's entries; nameOf: a skill's name.
    render(play, ids, nameOf) {
      const ui = play.ui, mode = play.uiMode;
      const sig = JSON.stringify([mode, ui.hint, ui.at, ui.actions, ui.strip, ui.feedback, ids]);
      if (sig === last) return;
      last = sig;
      hint.style.display = mode === 'none' && ui.hint ? 'block' : 'none';
      hint.textContent = ui.hint;
      wheel.style.display = mode === 'wheel' ? 'block' : 'none';
      strip.style.display = mode === 'strip' ? 'block' : 'none';
      if (mode === 'wheel') {
        wheel.replaceChildren?.();
        if (!wheel.replaceChildren) wheel.children = [];
        const title = document.createElement('div');
        Object.assign(title.style, { position: 'absolute', left: '50%', top: '50%', transform: 'translate(-50%,-50%)', textAlign: 'center' });
        title.textContent = `${ui.actions ? 'Actions' : 'Skills'}${ui.feedback ? ` - ${ui.feedback}` : ''}`;
        wheel.appendChild(title);
        for (const e of wheelEntries(ui.actions ? ACTIONS : ids, ui.at, nameOf)) {
          const cell = document.createElement('div');
          Object.assign(cell.style, { position: 'absolute', left: `${50 + e.x * 22}%`, top: `${50 + e.y * 28}%`, transform: 'translate(-50%,-50%)',
            background: e.on ? '#8a6a1a' : 'rgba(0,0,0,0.7)', border: `1px solid ${e.on ? '#ffd24a' : '#4a4a4a'}`, padding: '3px 6px' });
          cell.textContent = e.label;
          wheel.appendChild(cell);
        }
      }
      if (mode === 'strip') strip.textContent = PANELS.map(([label], i) => (i === ui.strip ? `[${label}]` : label)).join('  ');
    },
    hide() { last = ''; for (const el of [hint, wheel, strip]) el.style.display = 'none'; },
  };
}
