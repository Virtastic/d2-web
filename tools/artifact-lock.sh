#!/usr/bin/env bash
# Source this before reading/writing a worktree's engine or staged site. Keep the
# descriptor through nested build/stage/cache calls; other worktrees use other locks.
d2_artifact_lock() {
  local root lock
  root=$(cd "$1" && pwd -P)
  lock="$root/.git-artifacts.lock"
  if [ "${D2_ARTIFACT_LOCK:-}" = "$lock" ] && [ "$lock" -ef /dev/fd/8 ]; then return; fi
  exec 8>"$lock"
  flock -x 8
  export D2_ARTIFACT_LOCK="$lock"
}
