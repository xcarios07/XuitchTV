#include "player/PlaybackResolver.hpp"

#include <algorithm>
#include <cctype>
#include <set>

namespace xuitch::player {
namespace {

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool contains(const std::string& haystack, const std::string& needle) {
    return lower(haystack).find(lower(needle)) != std::string::npos;
}

void addCandidate(PlaybackPlan& plan,
                  std::set<std::string>& seen,
                  const api::SlbCdnEntry& cdn,
                  const std::string& url,
                  const std::string& tag) {
    if (url.empty() || !seen.insert(url).second) return;

    PlaybackCandidate candidate;
    candidate.url = url;
    candidate.cdnId = cdn.cdnId;
    candidate.cdnType = cdn.cdnType;
    candidate.tag = tag.empty() ? cdn.tag : tag;
    candidate.token = cdn.token;
    candidate.gslbParams = cdn.gslbParams;
    candidate.playParams = plan.playParams;
    candidate.weight = cdn.weight;
    candidate.score = PlaybackResolver::isHttpMediaUrl(url) ? 1 : -1000;
    plan.candidates.emplace_back(std::move(candidate));
}

} // namespace

bool PlaybackResolver::isHttpMediaUrl(const std::string& url) {
    const auto value = lower(url);
    return value.rfind("https://", 0) == 0 || value.rfind("http://", 0) == 0;
}

int PlaybackResolver::score(const PlaybackCandidate& candidate) {
    if (!isHttpMediaUrl(candidate.url)) return -1000;

    int total = 10;
    const auto url = lower(candidate.url);
    const auto tag = lower(candidate.tag);
    const auto type = lower(candidate.cdnType);

    if (url.rfind("https://", 0) == 0) total += 8;
    if (url.find(".m3u8") != std::string::npos) total += 30;
    else if (url.find(".mpd") != std::string::npos) total += 25;
    else if (url.find(".mp4") != std::string::npos || url.find(".mkv") != std::string::npos) total += 20;

    if (tag == "main" || tag == "primary" || tag == "default") total += 10;
    if (contains(type, "main") || contains(type, "primary")) total += 5;

    // Portal-provided weight is useful as a tie-breaker but must not dominate
    // protocol/media compatibility.
    total += std::clamp(candidate.weight, -20, 20);
    return total;
}

PlaybackPlan PlaybackResolver::fromSlb(const api::SlbInfoResponseData& data) {
    PlaybackPlan plan;
    plan.playParams = data.playParams;
    std::set<std::string> seen;

    for (const auto& cdn : data.cdnList) {
        for (const auto& item : cdn.urls) {
            addCandidate(plan, seen, cdn, item.url, item.tag);
        }
        addCandidate(plan, seen, cdn, cdn.mainAddr, "main");
        addCandidate(plan, seen, cdn, cdn.sparedAddr, "spare");
    }

    for (auto& candidate : plan.candidates) candidate.score = score(candidate);
    std::stable_sort(plan.candidates.begin(), plan.candidates.end(), [](const auto& a, const auto& b) {
        return a.score > b.score;
    });
    return plan;
}

std::optional<PlaybackCandidate> PlaybackResolver::selectBest(const PlaybackPlan& plan) {
    for (const auto& candidate : plan.candidates) {
        if (candidate.score >= 0 && isHttpMediaUrl(candidate.url)) return candidate;
    }
    return std::nullopt;
}

} // namespace xuitch::player
