#pragma once

#include <borealis.hpp>

#include <string>

#include "iptv/IptvModels.hpp"
#include "iptv/IptvNavigator.hpp"

namespace xuitch::ui {

class IptvActivity : public brls::Activity {
public:
    ~IptvActivity() override = default;

    CONTENT_FROM_XML_RES("activity/iptv.xml");
    void onContentAvailable() override;
    void willAppear(bool resetState = false) override;

private:
    void refreshPlaylist();
    void renderCategories();
    void renderChannels();
    void clearBox(brls::Box* box);
    void setStatus(const std::string& text);

    iptv::IptvPlaylist playlist;
    iptv::IptvNavigator navigator;

    brls::Box* categoryBox{nullptr};
    brls::Box* channelBox{nullptr};
    brls::Label* statusLabel{nullptr};
    brls::Label* countLabel{nullptr};
    brls::Button* refreshButton{nullptr};
};

} // namespace xuitch::ui
