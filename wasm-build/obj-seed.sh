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
  echo "==> seeded $n objects from the main checkout"
}
