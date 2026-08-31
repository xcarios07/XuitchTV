#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/.http-test"
g++ -std=c++17 -Wall -Wextra -Werror \
  -I"$ROOT/app/include" \
  "$ROOT/app/source/util/JsonLite.cpp" \
  "$ROOT/app/source/api/PortalModels.cpp" \
  "$ROOT/app/source/api/HttpClient.cpp" \
  "$ROOT/app/source/api/ApiClient.cpp" \
  "$ROOT/tests/http_compile_test.cpp" \
  $(pkg-config --cflags --libs libcurl) \
  -o "$OUT"
"$OUT"
rm -f "$OUT"
