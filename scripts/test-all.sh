#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
"$ROOT/scripts/test-core.sh"
"$ROOT/scripts/test-http-build.sh"
"$ROOT/scripts/test-config.sh"
"$ROOT/scripts/test-playback.sh"
"$ROOT/scripts/test-player-core.sh"
"$ROOT/scripts/test-iptv.sh"
"$ROOT/scripts/test-iptv-navigation.sh"
"$ROOT/scripts/test-ui-contract.sh"
echo "XuitchTV host test suite: OK"
