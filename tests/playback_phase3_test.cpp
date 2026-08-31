#include "api/PortalModels.hpp"
#include "player/PlaybackResolver.hpp"
#include "player/StreamSelector.hpp"

#include <cassert>
#include <iostream>

int main() {
    using namespace xuitch;

    api::SlbInfoResponseData data;
    data.playParams = "opaque-play-params";

    api::SlbCdnEntry slow;
    slow.cdnId = "cdn-backup";
    slow.weight = 2;
    slow.sparedAddr = "http://backup.example/movie.mp4";

    api::SlbCdnEntry preferred;
    preferred.cdnId = "cdn-main";
    preferred.cdnType = "main";
    preferred.weight = 10;
    preferred.token = "opaque-token";
    preferred.gslbParams = "opaque-gslb";
    preferred.urls.push_back({"primary", "https://media.example/live/master.m3u8"});
    preferred.mainAddr = "https://media.example/live/master.m3u8"; // duplicate on purpose

    data.cdnList = {slow, preferred};

    const auto plan = player::PlaybackResolver::fromSlb(data);
    assert(plan.playParams == "opaque-play-params");
    assert(plan.candidates.size() == 2); // duplicate URL removed

    const auto best = player::PlaybackResolver::selectBest(plan);
    assert(best.has_value());
    assert(best->url == "https://media.example/live/master.m3u8");
    assert(best->token == "opaque-token");
    assert(best->playParams == "opaque-play-params");
    assert(best->url.find("opaque-token") == std::string::npos); // never invent composition

    assert(player::PlaybackResolver::isHttpMediaUrl("https://a.example/x.m3u8"));
    assert(!player::PlaybackResolver::isHttpMediaUrl("file:///tmp/x.mp4"));


    api::StartPlayLiveResponseData live;
    live.liveAddressList = {
        {"HLS", "backup", "", "live-hd", "HD", "backup"},
        {"HLS", "main", "", "live-4k", "4K", "main"}
    };
    const auto liveBest = player::StreamSelector::selectLive(live, "4K", "main");
    assert(liveBest.has_value());
    assert(liveBest->playCode == "live-4k");

    api::StartPlayVodResponseData vod;
    api::VodEpisode ep;
    ep.episodeNumber = 2;
    ep.programContentId = "program-2";
    api::VodMovie hd; hd.contentId = "movie-hd"; hd.quality = "HD"; hd.videoFormat = "H264";
    api::VodMovie uhd; uhd.contentId = "movie-4k"; uhd.quality = "4K"; uhd.videoFormat = "H265";
    ep.movies = {hd, uhd};
    vod.episodes = {ep};
    const auto vodBest = player::StreamSelector::selectVod(vod, 2, "4K");
    assert(vodBest.has_value());
    assert(vodBest->contentId == "movie-4k");
    assert(vodBest->episodeNumber == 2);

    std::cout << "phase3 playback resolver/selector tests: OK\n";
    return 0;
}
