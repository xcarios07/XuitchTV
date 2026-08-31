#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/.player-core-test"
g++ -std=c++17 -Wall -Wextra -Werror -DXUITCHTV_HAS_MPV=0 \
  -I"$ROOT/app/include" \
  "$ROOT/app/source/player/Player.cpp" \
  "$ROOT/tests/player_core_test.cpp" \
  -o "$OUT"
"$OUT"
rm -f "$OUT"
echo "phase5 player state/error tests: OK"
