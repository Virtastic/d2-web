#!/usr/bin/env bash
# Source this. d2_obj_seed <seed-dir> <obj-dir>: fill an empty object dir from another tree's objects for
# the same compiler flags, so a new worktree's first build compiles only the sources it changed.
# link-d2.sh decides staleness by the content of each object's inputs, so a copied object is reused
# exactly when its sources and headers are byte-identical here, and recompiled otherwise.
# Copies are reflinks where the filesystem has them (instant, copy-on-write), plain copies elsewhere;
# never hard links: a later compile here must not write into the other tree's files.
d2_obj_seed() {
  local seed="$1" obj="$2" n=0 o
  [ -d "$seed" ] && [ -d "$obj" ] || return 0
  [ -z "$(find "$obj" -maxdepth 1 -name '*.o' -print -quit 2>/dev/null)" ] || return 0
  cp -a --reflink=auto "$seed/." "$obj/" 2>/dev/null || true
  # The other tree may be compiling right now. Keep an object only if it is the same file the seed
  # still holds (size and mtime) and its digest was written after it; anything else is dropped and
  # compiled here.
  while IFS= read -r o; do
    rm -f "$obj/$o" "$obj/$o.d" "$obj/$o.inputs"
  done < <(
    { find "$seed" -maxdepth 1 -name '*.o' -printf 'S %f %s %T@\n'
      find "$obj" -maxdepth 1 \( -name '*.o' -o -name '*.o.inputs' -o -name '*.o.d' \) -printf 'C %f %s %T@\n'
    } | awk '
      $1 == "S" { s[$2] = $3 " " $4; next }
      { c[$2] = $3 " " $4; t[$2] = $4 }
      END {
        for (f in c) {
          if (f ~ /\.o\.(inputs|d)$/) { b = f; sub(/\.(inputs|d)$/, "", b); if (!(b in c)) print b; continue }
          if (!(f in s) || s[f] != c[f] || !((f ".d") in c) || !((f ".inputs") in c) || t[f ".inputs"] + 0 < t[f] + 0) print f
        }
      }' | sort -u
  )
  n=$(find "$obj" -maxdepth 1 -name '*.o' | wc -l)
  echo "==> seeded $n objects from $seed"
}

# d2_obj_seed_pick <dir>...: the candidate seed whose objects changed last (newest dir mtime), or nothing if none exists.
d2_obj_seed_pick() {
  local d best=""
  for d; do
    [ -d "$d" ] || continue
    if [ -z "$best" ] || [ "$d" -nt "$best" ]; then best=$d; fi
  done
  printf '%s' "$best"
}

# d2_obj_publish <tree-obj-dir> <store-dir> <base-commit>: publish a worktree's object dir for one flag set as the seed
# later worktrees copy. The main checkout only builds the default flags, so the seed for any other set (the session
# role's -DD2_CALLTRACE) used to be published once and never replaced: every later tree recompiled everything main had
# changed since (6000+ objects a build, 2026-11-16). The store is replaced when this tree is built on a newer main (the
# stored .base is an ancestor of <base>), or when the store is a day old; a tree on an older main never regresses it.
# The swap is two renames; the old copy is kept 15 minutes for a seeder that may still be reading it.
d2_obj_publish() {
  local src="$1" store="$2" base="$3" old t pub
  [ -d "$src" ] && [ -n "$base" ] || return 0
  [ -n "$(find "$src" -maxdepth 1 -name '*.o' -print -quit 2>/dev/null)" ] || return 0
  pub=$(dirname "$store"); mkdir -p "$pub" || return 0
  (
    flock -n 7 || exit 0   # another tree is publishing: its copy is as good as this one
    if [ -d "$store" ]; then
      old=$(cat "$store/.base" 2>/dev/null || true)
      [ "$old" != "$base" ] || exit 0
      if [ -n "$old" ] && ! git merge-base --is-ancestor "$old" "$base" 2>/dev/null &&
         [ -z "$(find "$store/.base" -mmin +1440 2>/dev/null)" ]; then exit 0; fi
    fi
    t=$(mktemp -d "$store.publish.XXXXXX") || exit 0
    if ! cp -a --reflink=auto "$src/." "$t/" 2>/dev/null; then rm -rf "$t"; exit 0; fi
    printf '%s\n' "$base" > "$t/.base"
    if [ -d "$store" ]; then mv -T "$store" "$t.old" && touch "$t.old" || { rm -rf "$t"; exit 0; }; fi
    mv -T "$t" "$store" || { rm -rf "$t"; exit 0; }
    echo "==> published object dir $(basename "$store") for main at ${base:0:12}"
    find "$pub" -mindepth 1 -maxdepth 1 -name '*.publish.*' -mmin +15 -exec rm -rf {} + 2>/dev/null
    exit 0
  ) 7>"$pub/.publish.lock"
}
