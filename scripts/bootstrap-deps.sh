#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BOREALIS_REF="${BOREALIS_REF:-switchfin}"
mkdir -p "$ROOT/library"

if [[ ! -d "$ROOT/library/borealis/.git" ]]; then
  git clone --depth 1 --branch "$BOREALIS_REF" --recurse-submodules --shallow-submodules \
    https://github.com/dragonflylee/borealis.git "$ROOT/library/borealis"
else
  git -C "$ROOT/library/borealis" fetch --depth 1 origin "$BOREALIS_REF"
  git -C "$ROOT/library/borealis" checkout -B "$BOREALIS_REF" FETCH_HEAD
  git -C "$ROOT/library/borealis" submodule update --init --recursive --depth 1
fi
printf 'Borealis (%s) listo en %s\n' "$BOREALIS_REF" "$ROOT/library/borealis"
