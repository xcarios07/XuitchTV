#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/.iptv-navigation-test"
g++ -std=c++17 -Wall -Wextra -Werror \
  -I"$ROOT/app/include" \
  "$ROOT/app/source/iptv/IptvNavigator.cpp" \
  "$ROOT/tests/iptv_navigation_test.cpp" \
  -o "$OUT"
"$OUT"
rm -f "$OUT"
