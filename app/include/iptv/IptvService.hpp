#pragma once

#include <cstddef>
#include <string>
#include "api/HttpClient.hpp"
#include "iptv/IptvModels.hpp"

namespace xuitch::iptv {

class IptvService {
public:
    explicit IptvService(api::HttpClient& httpClient) : http(httpClient) {}

    bool refresh(const std::string& playlistUrl,
                 IptvPlaylist& playlist,
                 std::string* error = nullptr) const;

    StreamHealth probe(const IptvChannel& channel,
                       long timeoutSeconds = 6) const;

    std::size_t verify(IptvPlaylist& playlist,
                       long timeoutSeconds = 6,
                       std::size_t maxChannels = 0) const;

private:
    api::HttpClient& http;
};

} // namespace xuitch::iptv
