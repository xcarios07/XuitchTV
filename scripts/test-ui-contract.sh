#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
python3 - "$ROOT" <<'PY'
from pathlib import Path
import sys
root = Path(sys.argv[1])
required_files = [
    'app/include/ui/IptvActivity.hpp',
    'app/include/ui/SplashActivity.hpp',
    'app/source/ui/IptvActivity.cpp',
    'app/include/ui/PlayerActivity.hpp',
    'app/source/ui/PlayerActivity.cpp',
    'app/include/ui/VideoView.hpp',
    'app/source/ui/VideoView.cpp',
    'resources/xml/activity/main.xml',
    'resources/xml/activity/splash.xml',
    'resources/xml/activity/iptv.xml',
    'resources/xml/activity/player.xml',
    'resources/images/splash.png',
    'resources/images/xuitchtv_logo_transparent.png',
]
for rel in required_files:
    p = root / rel
    assert p.exists() and p.stat().st_size > 0, f'missing {rel}'

checks = {
    'resources/xml/activity/main.xml': ['main/iptv/button', 'main/portal/button', 'main/movies/button', 'main/series/button', 'main/sports/button', 'xuitchtv_logo_transparent.png'],
    'resources/xml/activity/splash.xml': ['images/splash.png'],
    'resources/xml/activity/iptv.xml': ['iptv/refresh', 'iptv/categories', 'iptv/channels', 'iptv/status', 'iptv/count'],
    'resources/xml/activity/player.xml': ['player/video/host', 'player/title', 'player/status', 'player/play', 'player/pause', 'player/stop'],
}
for rel, needles in checks.items():
    text = (root / rel).read_text(encoding='utf-8')
    for needle in needles:
        assert needle in text, f'{needle} missing from {rel}'

video = (root / 'app/source/ui/VideoView.cpp').read_text(encoding='utf-8')
assert 'MPV_RENDER_API_TYPE_DEKO3D' in video
assert 'MPV_RENDER_PARAM_DEKO3D_FBO' in video
assert 'getFramebuffer' in video
assert 'mpv_render_context_render' in video

main = (root / 'app/source/ui/MainActivity.cpp').read_text(encoding='utf-8')
assert 'new IptvActivity()' in main
assert 'XuitchTV v0.6.2 - Stream Headers Preview' in main
assert 'brls::TransitionAnimation::NONE' in main
assert 'brls::TransitionAnimation::SLIDE_LEFT' not in main
for checkpoint in ('[01]', '[02]', '[03]', '[04]', '[05]'):
    assert checkpoint in main, f'{checkpoint} missing from MainActivity diagnostic log'

iptv_header = (root / 'app/include/ui/IptvActivity.hpp').read_text(encoding='utf-8')
for forbidden in ('HttpClient', 'IptvService', 'PlayerActivity'):
    assert forbidden not in iptv_header, f'{forbidden} must remain out of the activity lifetime'
assert 'IptvNavigator' in iptv_header
assert 'IptvPlaylist' in iptv_header

iptv = (root / 'app/source/ui/IptvActivity.cpp').read_text(encoding='utf-8')
for checkpoint in ('[11]', '[12]', '[13]', '[14]', '[15]', '[16]', '[18]', '[20]', '[21]', '[22]', '[23]', '[30]', '[31]', '[32]', '[33]', '[34]', '[35]', '[36]', '[37]', '[38]', '[39]', '[40]', '[41]'):
    assert checkpoint in iptv, f'{checkpoint} missing from IptvActivity diagnostic log'
for required in ('api::HttpClient http', 'iptv::IptvService service(http)', 'service.refresh(', 'renderCategories()', 'renderChannels()', 'new PlayerActivity', 'makeChannelCard', 'images/channels/'):
    assert required in iptv, f'{required} missing from v0.6.2 media preview'
for forbidden in ('AppConfig::', 'SLIDE_LEFT'):
    assert forbidden not in iptv, f'{forbidden} must remain disabled in v0.6.2 media preview'

player = (root / 'app/source/ui/PlayerActivity.cpp').read_text(encoding='utf-8')
for checkpoint in ('[P01]', '[P04]', '[P05]', '[P06]', '[P20]', '[P21]', '[P22]', '[P23]', '[P90]'):
    assert checkpoint in player, f'{checkpoint} missing from PlayerActivity diagnostic log'
assert 'startPlayback()' in player
assert 'player.initialize(channel.httpReferrer, channel.httpUserAgent)' in player
assert 'TransitionAnimation::SLIDE_RIGHT' not in player

video = (root / 'app/source/ui/VideoView.cpp').read_text(encoding='utf-8')
for required in ('BOREALIS_USE_OPENGL', 'MPV_RENDER_API_TYPE_OPENGL', 'MPV_RENDER_PARAM_OPENGL_FBO', 'nvglCreateImageFromHandleGL3', 'nvgImagePattern'):
    assert required in video, f'{required} missing from OpenGL renderer'

logos = list((root / 'resources/images/channels').glob('*.png'))
assert len(logos) >= 60, f'expected embedded channel logos, found {len(logos)}'

boot = (root / 'app/source/main.cpp').read_text(encoding='utf-8')
assert 'new xuitch::ui::SplashActivity()' in boot
assert 'frame < 75' in boot
assert boot.index('new xuitch::ui::MainActivity()') < boot.index('new xuitch::ui::SplashActivity()')
assert 'popActivity(brls::TransitionAnimation::NONE)' in boot
print('v0.6.2 stream headers player contract test: OK')
PY
