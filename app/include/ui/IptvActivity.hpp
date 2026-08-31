#pragma once

#include <borealis.hpp>

#include "api/HttpClient.hpp"
#include "iptv/IptvModels.hpp"
#include "iptv/IptvNavigator.hpp"
#include "iptv/IptvService.hpp"

namespace xuitch::ui {

class IptvActivity : public brls::Activity {
public:
    IptvActivity();
    ~IptvActivity() override = default;

    CONTENT_FROM_XML_RES("activity/iptv.xml");
    void onContentAvailable() override;

private:
    void refreshPlaylist();
    void renderCategories();
    void renderChannels();
    void clearBox(brls::Box* box);
    void setStatus(const std::string& text);

    api::HttpClient http;
    iptv::IptvService service;
    iptv::IptvPlaylist playlist;
    iptv::IptvNavigator navigator;

    brls::Box* categoryBox{nullptr};
    brls::Box* channelBox{nullptr};
    brls::Label* statusLabel{nullptr};
    brls::Label* countLabel{nullptr};
    brls::Button* refreshButton{nullptr};
};

} // namespace xuitch::ui
