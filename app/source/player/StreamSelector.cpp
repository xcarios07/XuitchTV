#include "player/StreamSelector.hpp"

#include <algorithm>
#include <cctype>

namespace xuitch::player {
namespace {
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool equalNoCase(const std::string& a, const std::string& b) {
    return lower(a) == lower(b);
}

bool containsNoCase(const std::string& a, const std::string& b) {
    return lower(a).find(lower(b)) != std::string::npos;
}
} // namespace

int StreamSelector::qualityRank(const std::string& quality) {
    const auto q = lower(quality);
    if (q.find("8k") != std::string::npos || q.find("4320") != std::string::npos) return 600;
    if (q.find("4k") != std::string::npos || q.find("uhd") != std::string::npos || q.find("2160") != std::string::npos) return 500;
    if (q.find("fhd") != std::string::npos || q.find("1080") != std::string::npos) return 400;
    if (q == "hd" || q.find("720") != std::string::npos) return 300;
    if (q.find("sd") != std::string::npos || q.find("576") != std::string::npos || q.find("480") != std::string::npos) return 200;
    if (!q.empty()) return 100;
    return 0;
}

std::optional<LiveStreamSelection> StreamSelector::selectLive(
    const api::StartPlayLiveResponseData& data,
    const std::string& preferredQuality,
    const std::string& preferredTag) {

    const api::LiveAddress* best = nullptr;
    int bestScore = -1;

    for (const auto& item : data.liveAddressList) {
        if (item.playCode.empty()) continue;
        int score = qualityRank(item.quality);
        if (!preferredQuality.empty()) {
            if (equalNoCase(item.quality, preferredQuality)) score += 1000;
            else if (containsNoCase(item.quality, preferredQuality) || containsNoCase(preferredQuality, item.quality)) score += 300;
        }
        if (!preferredTag.empty() && equalNoCase(item.tag, preferredTag)) score += 800;
        if (equalNoCase(item.tag, "main") || equalNoCase(item.tag, "default")) score += 20;
        if (score > bestScore) { bestScore = score; best = &item; }
    }

    if (!best) return std::nullopt;
    return LiveStreamSelection{
        best->playCode, best->quality, best->tag, best->avFormat, best->cdnType, best->license
    };
}

std::optional<VodStreamSelection> StreamSelector::selectVod(
    const api::StartPlayVodResponseData& data,
    int episodeNumber,
    const std::string& preferredQuality) {

    const api::VodEpisode* selectedEpisode = nullptr;
    if (episodeNumber > 0) {
        for (const auto& episode : data.episodes) {
            if (episode.episodeNumber == episodeNumber) { selectedEpisode = &episode; break; }
        }
    }
    if (!selectedEpisode && !data.episodes.empty()) selectedEpisode = &data.episodes.front();
    if (!selectedEpisode) return std::nullopt;

    const api::VodMovie* best = nullptr;
    int bestScore = -1;
    for (const auto& movie : selectedEpisode->movies) {
        if (movie.contentId.empty()) continue;
        int score = qualityRank(movie.quality);
        if (!preferredQuality.empty()) {
            if (equalNoCase(movie.quality, preferredQuality)) score += 1000;
            else if (containsNoCase(movie.quality, preferredQuality) || containsNoCase(preferredQuality, movie.quality)) score += 300;
        }
        if (score > bestScore) { bestScore = score; best = &movie; }
    }

    if (!best) return std::nullopt;
    return VodStreamSelection{
        selectedEpisode->episodeNumber,
        selectedEpisode->programContentId,
        best->contentId,
        best->quality,
        best->videoFormat,
        best->encodeFormat,
        best->audioType
    };
}

} // namespace xuitch::player
