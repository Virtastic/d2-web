// Browser frontend for the configured Battle.net-compatible realm path. The endpoint comes from
// bnet-servers.json; these controls collect only player credentials and game/character choices.
const CLASSES = ['Amazon', 'Sorceress', 'Necromancer', 'Paladin', 'Barbarian', 'Druid', 'Assassin'];

export function createRealmLoginDialog({ documentObject = globalThis.document, onAuthenticate, onJoin, onClose }) {
  const panel = documentObject.createElement('section');
  panel.id = 'realm-login-panel';
  panel.hidden = true;
  panel.setAttribute('role', 'dialog');
  panel.setAttribute('aria-modal', 'true');
  panel.setAttribute('aria-labelledby', 'realm-dialog-title');
  panel.style.cssText = 'position:absolute;inset:0;z-index:20;display:none;place-items:center;background:#000b;color:#ddd;font:14px monospace';
  panel.innerHTML = `<form style="width:min(420px,92%);padding:24px;background:#11110f;border:1px solid #8b7444;box-shadow:0 8px 40px #000">
    <h2 id="realm-dialog-title" style="margin:0 0 16px;color:#c9ad68;font:22px Georgia,serif">BATTLE.NET LOGIN</h2>
    <p id="realm-region" style="color:#aaa;margin:0 0 14px"></p>
    <div id="realm-login-fields">
      <label style="display:block;margin:10px 0">Account<br><input id="realm-account" autocomplete="username" required maxlength="31" style="width:100%;padding:8px;background:#080808;color:#ddd;border:1px solid #493d27"></label>
      <label style="display:block;margin:10px 0">Password<br><input id="realm-password" type="password" autocomplete="current-password" required style="width:100%;padding:8px;background:#080808;color:#ddd;border:1px solid #493d27"></label>
      <button id="realm-login-submit" type="submit">LOG IN</button>
    </div>
    <div id="realm-character-fields" hidden>
      <label style="display:block;margin:10px 0">Character<br><select id="realm-character" required style="width:100%;padding:8px;background:#080808;color:#ddd;border:1px solid #493d27"></select></label>
      <label style="display:block;margin:10px 0">Game name<br><input id="realm-game" required maxlength="15" style="width:100%;padding:8px;background:#080808;color:#ddd;border:1px solid #493d27"></label>
      <label style="display:block;margin:10px 0">Game password (optional)<br><input id="realm-game-password" type="password" maxlength="15" style="width:100%;padding:8px;background:#080808;color:#ddd;border:1px solid #493d27"></label>
      <button id="realm-join-submit" type="submit">CREATE AND JOIN GAME</button>
    </div>
    <p id="realm-status" role="status" aria-live="polite" style="min-height:2.8em;color:#b8a77e"></p>
    <button id="realm-cancel" type="button">CANCEL</button>
  </form>`;
  const $ = (id) => panel.querySelector(`#${id}`);
  const form = panel.querySelector('form');
  // The two stages share one dialog but only one stage is active; run explicit validation below so
  // browser constraint validation never targets controls hidden in the inactive stage.
  form.noValidate = true;
  let busy = false;
  const syncControls = () => {
    for (const field of panel.querySelectorAll('input, select, button')) {
      const group = field.closest('#realm-login-fields, #realm-character-fields');
      field.disabled = busy || Boolean(group?.hidden);
    }
  };
  const setBusy = (value) => {
    busy = value; syncControls();
  };
  const status = (message) => { $('realm-status').textContent = message; };
  form.addEventListener('submit', async (event) => {
    event.preventDefault();
    if (busy) return;
    if (!$('realm-login-fields').hidden) {
      if (!$('realm-account').value.trim() || !$('realm-password').value) { status('Enter an account and password.'); return; }
      setBusy(true); status('Connecting to the configured realm…');
      try {
        const characters = await onAuthenticate({ account: $('realm-account').value.trim(), password: $('realm-password').value });
        const select = $('realm-character'); select.replaceChildren();
        for (const character of characters) {
          const option = documentObject.createElement('option'); option.value = character.name;
          option.textContent = `${character.name} · level ${character.level} ${CLASSES[character.charClass] || `class ${character.charClass}`}`;
          select.append(option);
        }
        if (!characters.length) throw new Error('This realm account has no characters.');
        $('realm-login-fields').hidden = true; $('realm-character-fields').hidden = false; syncControls();
        $('realm-dialog-title').textContent = 'SELECT CHARACTER'; status('Choose a character and create a game.');
      } catch (error) { status(String(error)); }
      finally { setBusy(false); }
    } else {
      if (!$('realm-character').value || !$('realm-game').value.trim()) { status('Choose a character and enter a game name.'); return; }
      setBusy(true); status('Creating the game and joining its game server…');
      try { status(await onJoin({ character: $('realm-character').value, gameName: $('realm-game').value.trim(), gamePassword: $('realm-game-password').value })); }
      catch (error) {
        // The shell tears down MCP/D2GS on failure; return to a real login state so the
        // visible retry control cannot submit against a freed protocol session.
        $('realm-character-fields').hidden = true; $('realm-login-fields').hidden = false;
        $('realm-dialog-title').textContent = 'BATTLE.NET LOGIN';
        syncControls(); status(`${String(error)} Log in again to retry.`);
      }
      finally { setBusy(false); }
    }
  });
  $('realm-cancel').addEventListener('click', () => { if (!busy) onClose(); });
  return {
    element: panel,
    get visible() { return !panel.hidden; },
    show(region) {
      $('realm-region').textContent = `Configured region: ${region}`;
      $('realm-dialog-title').textContent = 'BATTLE.NET LOGIN';
      $('realm-login-fields').hidden = false; $('realm-character-fields').hidden = true; syncControls();
      $('realm-account').value = ''; $('realm-password').value = ''; $('realm-game').value = '';
      status(''); panel.hidden = false; panel.style.display = 'grid'; $('realm-account').focus();
    },
    close() { panel.hidden = true; panel.style.display = 'none'; },
    status,
    evidence() { return { visible: !panel.hidden, title: $('realm-dialog-title').textContent,
      selectedCharacter: $('realm-character').value, status: $('realm-status').textContent }; },
  };
}
