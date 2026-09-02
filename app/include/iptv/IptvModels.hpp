#pragma once

#include <string>
#include <vector>

namespace xuitch::iptv {

enum class StreamHealth {
    Unknown,
    Reachable,
    Unreachable
};

struct IptvChannel {
    std::string name;
    std::string url;
    std::string tvgId;
    std::string tvgName;
    std::string logoUrl;
    std::string groupTitle;
    std::string language;
    std::string country;
    std::string httpReferrer;
    std::string httpUserAgent;
    StreamHealth health{StreamHealth::Unknown};
};

struct IptvPlaylist {
    std::string sourceUrl;
    std::vector<IptvChannel> channels;
};

} // namespace xuitch::iptv
