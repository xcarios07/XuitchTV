#pragma once

#include <optional>
#include <string>

#include "api/PortalModels.hpp"

namespace xuitch::player {

struct LiveStreamSelection {
    std::string playCode;
    std::string quality;
    std::string tag;
    std::string avFormat;
    std::string cdnType;
    std::string license;
};

struct VodStreamSelection {
    int episodeNumber{0};
    std::string programContentId;
    std::string contentId;
    std::string quality;
    std::string videoFormat;
    std::string encodeFormat;
    std::string audioType;
};

class StreamSelector {
public:
    static std::optional<LiveStreamSelection> selectLive(
        const api::StartPlayLiveResponseData& data,
        const std::string& preferredQuality = {},
        const std::string& preferredTag = {});

    static std::optional<VodStreamSelection> selectVod(
        const api::StartPlayVodResponseData& data,
        int episodeNumber = 0,
        const std::string& preferredQuality = {});

private:
    static int qualityRank(const std::string& quality);
};

} // namespace xuitch::player
