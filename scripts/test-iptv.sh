#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CXX="${CXX:-g++}"
"$CXX" -std=c++17 -Wall -Wextra -Werror \
  -I"$ROOT/app/include" \
  "$ROOT/tests/iptv_phase4_test.cpp" \
  "$ROOT/app/source/iptv/M3uParser.cpp" \
  "$ROOT/app/source/iptv/IptvCatalog.cpp" \
  -o /tmp/xuitchtv_iptv_test
/tmp/xuitchtv_iptv_test
