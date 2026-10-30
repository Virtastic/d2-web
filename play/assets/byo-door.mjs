// The front door of a bring-your-own-files host: one card, a checklist of the archives the game needs,
// a way to give them, and a Play button that wakes when they are all there. Resolves with the mounts.
import { createCollection, fromDrop, pickFiles, pickFolder, restore, forget } from './byo-source.mjs';
import { ROLES } from './byo-detect.mjs';

const CSS = `
body.door #viewport,body.door #session-bar,body.door .controls,body.door #log,body.door #hud{display:none}
#door{display:none;min-height:100vh;align-items:center;justify-content:center;padding:24px;box-sizing:border-box}
body.door #door{display:flex}
#door .card{width:100%;max-width:440px;padding:32px 32px 24px;background:#0b0c09;border:1px solid #493d27;border-radius:4px;text-align:left;transition:border-color .15s}
#door .card.over{border-color:#c9ad68}
#door h1{margin:0 0 6px;font:400 28px Georgia,serif;color:#c9ad68;letter-spacing:.02em}
#door .sub{margin:0 0 24px;color:#8a8069;font-size:13px;line-height:1.5}
#door ul{list-style:none;margin:0 0 6px;padding:0}
#door li{display:flex;align-items:center;gap:12px;padding:7px 0;color:#a89c80;font-size:14px}
#door li i{flex:none;width:16px;height:16px;border:1px solid #493d27;border-radius:50%;box-sizing:border-box;position:relative}
#door li.have{color:#d8cdb0}
#door li.have i{background:#c9ad68;border-color:#c9ad68}
#door li.have i::after{content:"";position:absolute;left:4px;top:2px;width:4px;height:7px;border:solid #0b0c09;border-width:0 2px 2px 0;transform:rotate(45deg)}
#door li small{margin-left:auto;color:#6b634f;font-size:12px;font-family:ui-monospace,monospace}
#door li.opt{color:#6b634f}
#door .more{margin:10px 0 0;color:#6b634f;font-size:12px}
#door .row{display:flex;gap:10px;margin-top:22px}
#door button{font:inherit;font-size:14px;padding:10px 16px;border-radius:3px;cursor:pointer;border:1px solid #493d27;background:transparent;color:#d8cdb0}
#door button:hover:not(:disabled){border-color:#c9ad68}
#door button.go{flex:1;background:#c9ad68;border-color:#c9ad68;color:#0b0c09;font-weight:600}
#door button.go:disabled{background:transparent;border-color:#2a251a;color:#4a4333;cursor:default}
#door button:focus-visible,#door a:focus-visible{outline:2px solid #c9ad68;outline-offset:2px}
#door .note{margin:18px 0 0;padding:10px 12px;border-left:2px solid #c9ad68;background:#12130e;color:#b9ad8e;font-size:13px;line-height:1.5}
#door .note.bad{border-color:#a4452f}
#door .note a,#door .link{color:#c9ad68;background:none;border:0;padding:0;font-size:inherit;text-decoration:underline;cursor:pointer}
#door .legal{margin:22px 0 0;color:#5a5340;font-size:11px;line-height:1.5}
`;

const el = (tag, cls, text) => { const e = document.createElement(tag); if (cls) e.className = cls; if (text != null) e.textContent = text; return e; };

export function frontDoor({ onReady } = {}) {
  const style = el('style'); style.textContent = CSS; document.head.appendChild(style);
  const root = el('main'); root.id = 'door';
  const card = el('div', 'card');
  card.append(el('h1', '', 'Diablo II'), el('p', 'sub', 'Play in your browser with your own copy of the game. Your files stay on this device — nothing is uploaded.'));
  const list = el('ul'), opt = el('p', 'more'), notes = el('div'), row = el('div', 'row');
  const pick = el('button', '', 'Choose files'), go = el('button', 'go', 'Play');
  go.disabled = true;
  const folder = el('button', 'link', 'or choose a folder');
  row.append(pick, go);
  const hint = el('p', 'more'); hint.append('Drop your Diablo II files here, ', folder, '.');
  card.append(list, opt, hint, notes, row, el('p', 'legal', 'Unofficial fan project, not affiliated with or endorsed by Blizzard Entertainment. You need to own Diablo II: Lord of Destruction (1.13c).'));
  root.append(card);
  document.body.append(root);
  document.body.classList.add('door');

  const rows = {};
  for (const [role, r] of Object.entries(ROLES).filter(([, r]) => r.required)) {
    const li = el('li'); li.append(el('i'), el('span', '', r.label), el('small', '', r.note));
    list.append(li); rows[role] = li;
  }
  const col = createCollection();
  add({});   // the host's 1.13c patch shim ticks the Patch row before any file is given

  function paint() {
    const s = col.state;
    for (const [role, li] of Object.entries(rows)) li.classList.toggle('have', !!s.slots[role]?.have);
    const extras = Object.entries(ROLES).filter(([, r]) => !r.required);
    const got = extras.filter(([r]) => s.slots[r]?.have).length;
    opt.textContent = `Optional: speech, music and videos ${got ? `(${got} of ${extras.length} found)` : '— the game runs without them'}.`;
    notes.replaceChildren();
    for (const i of s.issues) {
      const n = el('p', 'note' + (i.code === 'casc' || i.code === 'truncated' ? ' bad' : ''), i.text);
      notes.append(n);
    }
    if (s.busy) notes.append(el('p', 'note', 'Reading files…'));
    go.disabled = !s.ready || !!s.busy;
    pick.textContent = s.ready ? 'Change files' : 'Choose files';
  }
  async function add(src) { const p = col.add(src); paint(); await p; paint(); }

  pick.onclick = async () => add(await pickFiles());
  folder.onclick = async () => add(await pickFolder());
  card.addEventListener('dragover', (e) => { e.preventDefault(); card.classList.add('over'); });
  card.addEventListener('dragleave', () => card.classList.remove('over'));
  card.addEventListener('drop', async (e) => { e.preventDefault(); card.classList.remove('over'); add(await fromDrop(e.dataTransfer)); });

  // A returning visitor on Chromium: the browser remembered the handles; re-open them quietly if it
  // still allows, else offer one click that may prompt.
  restore(false).then(async ({ count, handles }) => {
    if (handles.length) await add({ handles });
    if (count && !col.state.ready) {
      const again = el('button', 'link', 'Use the files from last time');
      again.onclick = async () => { again.remove(); const r = await restore(true); await add({ handles: r.handles }); };
      hint.append(' ', again);
    }
  });

  return new Promise((resolve) => {
    go.onclick = () => {
      go.disabled = true; pick.disabled = true;
      document.body.classList.remove('door');
      root.remove();
      resolve(col.mounts());
    };
    window.__d2Door = { collection: col, add, paint, forget };   // test hook: feed files without a picker
    if (onReady) onReady(col);
  });
}
