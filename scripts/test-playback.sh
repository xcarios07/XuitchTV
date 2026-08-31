#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/.playback-test"
g++ -std=c++17 -Wall -Wextra -Werror \
  -I"$ROOT/app/include" \
  "$ROOT/app/source/player/PlaybackResolver.cpp" \
  "$ROOT/app/source/player/StreamSelector.cpp" \
  "$ROOT/tests/playback_phase3_test.cpp" \
  -o "$OUT"
"$OUT"
rm -f "$OUT"
