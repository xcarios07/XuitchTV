#include <cassert>
#include <iostream>
#include "iptv/IptvCatalog.hpp"
#include "iptv/M3uParser.hpp"

int main() {
    const std::string data = R"M3U(#EXTM3U
#EXTINF:-1 tvg-id="ParaguayTV.py" tvg-name="Paraguay TV" tvg-logo="https://img/logo.png" group-title="General",Paraguay TV
#EXTVLCOPT:http-referrer=https://www.paraguaytv.gov.py/
#EXTVLCOPT:http-user-agent=Mozilla/5.0 XuitchTV Test
https://example.invalid/paraguay.m3u8
#EXTINF:-1 tvg-id="Unicanal.py" group-title="Noticias",Unicanal
https://example.invalid/unicanal.m3u8
#EXTINF:-1 group-title="Noticias",Duplicado
https://example.invalid/unicanal.m3u8
)M3U";

    xuitch::iptv::IptvPlaylist playlist;
    std::string error;
    assert(xuitch::iptv::M3uParser::parse(data, playlist, &error));
    assert(playlist.channels.size() == 2);
    assert(playlist.channels[0].name == "Paraguay TV");
    assert(playlist.channels[0].tvgId == "ParaguayTV.py");
    assert(playlist.channels[0].groupTitle == "General");
    assert(playlist.channels[0].httpReferrer == "https://www.paraguaytv.gov.py/");
    assert(playlist.channels[0].httpUserAgent == "Mozilla/5.0 XuitchTV Test");
    assert(playlist.channels[1].groupTitle == "Noticias");

    auto grouped = xuitch::iptv::IptvCatalog::groupByCategory(playlist, false);
    assert(grouped.size() == 2);
    assert(grouped["General"].size() == 1);
    assert(grouped["Noticias"].size() == 1);

    playlist.channels[1].health = xuitch::iptv::StreamHealth::Unreachable;
    grouped = xuitch::iptv::IptvCatalog::groupByCategory(playlist, true);
    assert(grouped["General"].size() == 1);
    assert(grouped.find("Noticias") == grouped.end());

    std::cout << "phase4 iptv parser/catalog tests: OK\n";
}
