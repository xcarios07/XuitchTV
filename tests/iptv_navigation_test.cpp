#include <cassert>
#include <iostream>

#include "iptv/IptvNavigator.hpp"

int main() {
    xuitch::iptv::IptvPlaylist playlist;
    playlist.channels = {
        {"Paraguay TV", "https://a.invalid/live.m3u8", "ParaguayTV.py", "Paraguay TV", "", "General", "", "PY", xuitch::iptv::StreamHealth::Reachable},
        {"Canal Noticias", "https://b.invalid/live.m3u8", "News.py", "Canal Noticias", "", "Noticias", "", "PY", xuitch::iptv::StreamHealth::Unknown},
        {"Canal Caido", "https://c.invalid/live.m3u8", "Dead.py", "Canal Caido", "", "Noticias", "", "PY", xuitch::iptv::StreamHealth::Unreachable},
    };

    xuitch::iptv::IptvNavigator nav;
    nav.setPlaylist(playlist, false);
    assert(nav.categories().size() == 3); // Todos, General, Noticias
    assert(nav.visibleCount() == 3);
    assert(nav.selectCategory("Noticias"));
    assert(nav.visibleCount() == 2);
    assert(nav.channelAt(0)->name == "Canal Noticias");

    nav.setSearch("caido");
    assert(nav.visibleCount() == 1);
    assert(nav.channelAt(0)->tvgId == "Dead.py");

    nav.setSearch("");
    nav.setHideUnavailable(true);
    assert(nav.visibleCount() == 1);
    assert(nav.channelAt(0)->name == "Canal Noticias");

    assert(nav.selectCategory("Todos"));
    assert(nav.visibleCount() == 2);

    std::cout << "phase4B iptv navigation tests: OK\n";
}
