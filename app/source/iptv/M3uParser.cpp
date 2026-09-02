#include "iptv/M3uParser.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_set>

namespace xuitch::iptv {
namespace {

std::string trim(std::string value) {
    auto notSpace = [](unsigned char c) { return !std::isspace(c); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
    value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
    return value;
}

std::string attr(const std::string& line, const std::string& key) {
    const std::string needle = key + "=\"";
    const auto start = line.find(needle);
    if (start == std::string::npos) return {};
    const auto valueStart = start + needle.size();
    const auto end = line.find('"', valueStart);
    if (end == std::string::npos) return {};
    return line.substr(valueStart, end - valueStart);
}

std::string channelName(const std::string& line) {
    bool quoted = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '"') quoted = !quoted;
        if (line[i] == ',' && !quoted) return trim(line.substr(i + 1));
    }
    return {};
}

bool isHttpUrl(const std::string& value) {
    return value.rfind("https://", 0) == 0 || value.rfind("http://", 0) == 0;
}

} // namespace

bool M3uParser::parse(const std::string& text,
                      IptvPlaylist& playlist,
                      std::string* error) {
    playlist.channels.clear();
    if (text.empty()) {
        if (error) *error = "Empty M3U playlist";
        return false;
    }

    std::istringstream input(text);
    std::string line;
    IptvChannel pending;
    bool hasPending = false;
    bool sawHeader = false;
    std::unordered_set<std::string> seenUrls;

    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        line = trim(line);
        if (line.empty()) continue;

        if (line.rfind("#EXTM3U", 0) == 0) {
            sawHeader = true;
            continue;
        }

        if (line.rfind("#EXTINF:", 0) == 0) {
            pending = IptvChannel{};
            pending.tvgId = attr(line, "tvg-id");
            pending.tvgName = attr(line, "tvg-name");
            pending.logoUrl = attr(line, "tvg-logo");
            pending.groupTitle = attr(line, "group-title");
            pending.language = attr(line, "tvg-language");
            pending.country = attr(line, "tvg-country");
            pending.name = channelName(line);
            if (pending.name.empty()) pending.name = pending.tvgName;
            hasPending = true;
            continue;
        }

        constexpr const char* referrerPrefix = "#EXTVLCOPT:http-referrer=";
        constexpr const char* userAgentPrefix = "#EXTVLCOPT:http-user-agent=";
        if (hasPending && line.rfind(referrerPrefix, 0) == 0) {
            pending.httpReferrer = trim(line.substr(std::char_traits<char>::length(referrerPrefix)));
            continue;
        }
        if (hasPending && line.rfind(userAgentPrefix, 0) == 0) {
            pending.httpUserAgent = trim(line.substr(std::char_traits<char>::length(userAgentPrefix)));
            continue;
        }

        if (line[0] == '#') continue;
        if (!hasPending || !isHttpUrl(line)) continue;

        pending.url = line;
        if (pending.name.empty()) pending.name = "IPTV Channel";
        if (seenUrls.insert(pending.url).second) {
            playlist.channels.emplace_back(std::move(pending));
        }
        pending = IptvChannel{};
        hasPending = false;
    }

    if (!sawHeader && playlist.channels.empty()) {
        if (error) *error = "Input is not a valid extended M3U playlist";
        return false;
    }
    if (playlist.channels.empty()) {
        if (error) *error = "M3U playlist contains no HTTP/HTTPS channels";
        return false;
    }
    return true;
}

} // namespace xuitch::iptv
