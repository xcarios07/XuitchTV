#pragma once

#include <map>
#include <string>
#include <vector>
#include "iptv/IptvModels.hpp"

namespace xuitch::iptv {

class IptvCatalog {
public:
    static std::map<std::string, std::vector<const IptvChannel*>> groupByCategory(
        const IptvPlaylist& playlist,
        bool hideUnavailable = false);
};

} // namespace xuitch::iptv
