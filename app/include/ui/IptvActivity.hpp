#pragma once

#include <borealis.hpp>

namespace xuitch::ui {

class IptvActivity : public brls::Activity {
public:
    ~IptvActivity() override = default;

    CONTENT_FROM_XML_RES("activity/iptv.xml");
    void onContentAvailable() override;

private:
    brls::Box* categoryBox{nullptr};
    brls::Box* channelBox{nullptr};
    brls::Label* statusLabel{nullptr};
    brls::Label* countLabel{nullptr};
    brls::Button* refreshButton{nullptr};
};

} // namespace xuitch::ui

