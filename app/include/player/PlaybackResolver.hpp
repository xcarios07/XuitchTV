#pragma once

#include <optional>
#include <string>
#include <vector>

#include "api/PortalModels.hpp"

namespace xuitch::player {

struct PlaybackCandidate {
    std::string url;
    std::string cdnId;
    std::string cdnType;
    std::string tag;
    std::string token;
    std::string gslbParams;
    std::string playParams;
    int weight{0};
    int score{0};
};

struct PlaybackPlan {
    std::vector<PlaybackCandidate> candidates;
    std::string playParams;

    bool empty() const { return candidates.empty(); }
};

class PlaybackResolver {
public:
    // Builds a deterministic list of URLs already exposed by the authorized
    // portal response. Opaque token/play_params fields are preserved as
    // metadata and are not appended or transformed without verified protocol
    // behavior.
    static PlaybackPlan fromSlb(const api::SlbInfoResponseData& data);

    static std::optional<PlaybackCandidate> selectBest(const PlaybackPlan& plan);
    static bool isHttpMediaUrl(const std::string& url);

private:
    static int score(const PlaybackCandidate& candidate);
};

} // namespace xuitch::player
