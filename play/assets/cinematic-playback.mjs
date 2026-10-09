/** Own one explicit title-film open/play lifecycle, including its phase-owned audio resources. */
export function createCinematicPlayback(open, now = () => performance.now()) {
  let generation = 0, openingToken = 0, movie = null, playing = null, startedAt = 0, error = '';
  const stop = () => {
    generation++;
    const ownedMovie = movie;
    movie = null; playing = null; startedAt = 0; openingToken = 0;
    ownedMovie?.close?.();
  };
  const play = (index, path) => {
    if (openingToken) return false;
    const token = ++generation;
    openingToken = token; playing = index; error = '';
    const isCurrent = () => generation === token && playing === index;
    let opening;
    try { opening = open({ path, isCurrent }); }
    catch (cause) { opening = Promise.reject(cause); }
    Promise.resolve(opening)
      .then((openedMovie) => {
        if (!isCurrent()) { openedMovie?.discard?.(); return; }
        if (!openedMovie) { stop(); return; }
        movie = openedMovie; startedAt = now();
      })
      .catch((cause) => {
        if (!isCurrent()) return;
        error = `${path}: ${cause}`;
        stop();
      })
      .finally(() => { if (openingToken === token) openingToken = 0; });
    return true;
  };
  return {
    play, stop,
    get movie() { return movie; },
    get playing() { return playing; },
    get startedAt() { return startedAt; },
    get opening() { return openingToken !== 0; },
    get error() { return error; },
  };
}
