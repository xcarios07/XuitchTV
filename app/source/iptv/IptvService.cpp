#include "iptv/IptvService.hpp"
#include "iptv/M3uParser.hpp"

namespace xuitch::iptv {

bool IptvService::refresh(const std::string& playlistUrl,
                          IptvPlaylist& playlist,
                          std::string* error) const {
    if (playlistUrl.empty()) {
        if (error) *error = "IPTV playlist URL is empty";
        return false;
    }
    const auto response = http.get(playlistUrl);
    if (!response.ok()) {
        if (error) {
            *error = !response.error.empty()
                ? response.error
                : "HTTP " + std::to_string(response.statusCode);
        }
        return false;
    }

    IptvPlaylist parsed;
    parsed.sourceUrl = playlistUrl;
    if (!M3uParser::parse(response.body, parsed, error)) return false;
    playlist = std::move(parsed);
    return true;
}

StreamHealth IptvService::probe(const IptvChannel& channel,
                                long timeoutSeconds) const {
    if (channel.url.empty()) return StreamHealth::Unreachable;
    const auto response = http.probe(channel.url, timeoutSeconds, 4096);
    return response.reachable ? StreamHealth::Reachable : StreamHealth::Unreachable;
}

std::size_t IptvService::verify(IptvPlaylist& playlist,
                                long timeoutSeconds,
                                std::size_t maxChannels) const {
    std::size_t reachable = 0;
    std::size_t checked = 0;
    for (auto& channel : playlist.channels) {
        if (maxChannels > 0 && checked >= maxChannels) break;
        channel.health = probe(channel, timeoutSeconds);
        if (channel.health == StreamHealth::Reachable) ++reachable;
        ++checked;
    }
    return reachable;
}

} // namespace xuitch::iptv
