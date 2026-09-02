#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

for path in (ROOT / "resources" / "i18n").glob("*/*.json"):
    json.loads(path.read_text(encoding="utf-8"))

cmake = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
assert "project(XuitchTV)" in cmake
assert 'set(APP_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}")' in cmake
assert "SERVER_URL not defined" not in cmake
assert "SERVER_TOKEN not defined" not in cmake

main = (ROOT / "tsvitch" / "source" / "main.cpp").read_text(encoding="utf-8")
player = (ROOT / "tsvitch" / "source" / "activity" / "live_player_activity.cpp").read_text(encoding="utf-8")
assert "CLIENT::register_user" not in main
assert "CLIENT::check_user_id" not in main
assert "CLIENT::get_ad" not in player
assert "this->startLive();" in player

config = (ROOT / "tsvitch" / "source" / "utils" / "config_helper.cpp").read_text(encoding="utf-8")
assert "/config/XuitchTV" in config
assert "xuitchtv_config.json" in config
assert "XTREAM_SERVER_URL" in config
assert "M3U8_URL_ITEM" in config

workflow = (ROOT / ".github" / "workflows" / "build.yaml").read_text(encoding="utf-8")
assert "XuitchTV.nro" in workflow
assert "submodules: recursive" in workflow

icon = ROOT / "resources" / "icon" / "icon.jpg"
assert icon.exists() and icon.stat().st_size > 10_000

print("XuitchTV-Next source contract: OK")
