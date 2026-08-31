#pragma once

#include <borealis.hpp>

#include "iptv/IptvModels.hpp"
#include "player/Player.hpp"

namespace xuitch::ui {

class VideoView;

class PlayerActivity : public brls::Activity {
public:
    explicit PlayerActivity(iptv::IptvChannel channel);
    ~PlayerActivity() override;

    CONTENT_FROM_XML_RES("activity/player.xml");
    void onContentAvailable() override;

private:
    void updateStatus();

    iptv::IptvChannel channel;
    player::Player player;
    VideoView* videoView{nullptr};
    brls::Box* videoHost{nullptr};
    brls::Label* titleLabel{nullptr};
    brls::Label* statusLabel{nullptr};
    brls::Button* pauseButton{nullptr};
    brls::Button* stopButton{nullptr};
    bool paused{false};
};

} // namespace xuitch::ui
