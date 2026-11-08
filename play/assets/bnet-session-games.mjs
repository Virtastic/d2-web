// The join panel's games for this realm session. The realm answers SID_GETADVLISTEX with an empty list, so the
// games this session created, and the games it joined from a listed row, are kept here and shown under whatever
// the realm does list (a realm row with the same name wins). Pure of the page: plain objects in, rows out.
// A remembered game is `{ name, password, difficulty, description }`, difficulty as the statstring digit the
// list shows ('0'..'2' softcore, '3' and up hardcore).
export function createSessionGames() {
  let local = [], realm = [];
  return {
    // Keeps a game this session made or joined; the newest first, one row per name. A game with no difficulty
    // (a typed join of a game the list did not show) is not kept, since the panel could not filter it.
    remember(game) {
      if (!game?.name || typeof game.difficulty !== 'number') return false;
      const row = { name: String(game.name), password: Boolean(game.password), difficulty: game.difficulty, description: String(game.description ?? '') };
      local = [row, ...local.filter((g) => g.name !== row.name)];
      return true;
    },
    // The last SID_GETADVLISTEX reply, as parsed rows.
    setRealm(rows) { realm = rows.map((g) => ({ ...g })); },
    // The realm's rows, then the remembered games the realm does not list.
    list() {
      const names = new Set(realm.map((g) => g.name));
      return [...realm.map((g) => ({ ...g })), ...local.filter((g) => !names.has(g.name)).map((g) => ({ ...g }))];
    },
  };
}
