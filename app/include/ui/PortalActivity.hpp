#pragma once

#include <borealis.hpp>

#include <string>

namespace xuitch::ui {

class PortalActivity : public brls::Activity {
public:
    explicit PortalActivity(std::string section = "PORTAL TV");

    CONTENT_FROM_XML_RES("activity/portal.xml");
    void onContentAvailable() override;

private:
    void reloadConfig();
    void probeServer();
    void renderStatus(const std::string& detail = {});
    bool portalConfigured() const;

    std::string section;
    brls::Label* titleLabel{nullptr};
    brls::Label* stateLabel{nullptr};
    brls::Label* detailLabel{nullptr};
    brls::Label* hostLabel{nullptr};
    brls::Label* portalCodeLabel{nullptr};
    brls::Label* deviceLabel{nullptr};
    brls::Button* reloadButton{nullptr};
    brls::Button* probeButton{nullptr};
};

} // namespace xuitch::ui
