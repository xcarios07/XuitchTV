#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/.core-test"
g++ -std=c++17 -Wall -Wextra -Werror \
  -I"$ROOT/app/include" \
  "$ROOT/app/source/util/JsonLite.cpp" \
  "$ROOT/app/source/api/PortalModels.cpp" \
  "$ROOT/tests/core_phase3_test.cpp" \
  -o "$OUT"
"$OUT"
rm -f "$OUT"
