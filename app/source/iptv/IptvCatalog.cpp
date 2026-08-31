#include "iptv/IptvCatalog.hpp"

namespace xuitch::iptv {

std::map<std::string, std::vector<const IptvChannel*>> IptvCatalog::groupByCategory(
    const IptvPlaylist& playlist,
    bool hideUnavailable) {
    std::map<std::string, std::vector<const IptvChannel*>> groups;
    for (const auto& channel : playlist.channels) {
        if (hideUnavailable && channel.health == StreamHealth::Unreachable) continue;
        const std::string group = channel.groupTitle.empty() ? "Paraguay" : channel.groupTitle;
        groups[group].push_back(&channel);
    }
    return groups;
}

} // namespace xuitch::iptv
