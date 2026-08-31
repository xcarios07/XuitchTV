#pragma once

#include <string>
#include "iptv/IptvModels.hpp"

namespace xuitch::iptv {

class M3uParser {
public:
    static bool parse(const std::string& text,
                      IptvPlaylist& playlist,
                      std::string* error = nullptr);
};

} // namespace xuitch::iptv
