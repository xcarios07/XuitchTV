#include "ui/MainActivity.hpp"

#include "core/BuildInfo.hpp"

#include "ui/IptvActivity.hpp"

namespace xuitch::ui {

void MainActivity::onContentAvailable() {
    auto* iptvButton = dynamic_cast<brls::Button*>(getView("main/iptv/button"));
    auto* portalButton = dynamic_cast<brls::Button*>(getView("main/portal/button"));
    auto* versionLabel = dynamic_cast<brls::Label*>(getView("main/version"));

    if (versionLabel) {
        versionLabel->setText(std::string("XuitchTV v") + core::kAppVersion + " - First Hardware Preview");
    }

    if (iptvButton) {
        iptvButton->setStyle(&brls::BUTTONSTYLE_PRIMARY);
        iptvButton->registerClickAction([](brls::View*) {
            brls::Application::pushActivity(
                new IptvActivity(),
                brls::TransitionAnimation::SLIDE_LEFT);
            return true;
        });
    }

    // The portal API core exists, but its final browsing UI is not ready yet.
    if (portalButton) portalButton->setState(brls::ButtonState::DISABLED);
}

} // namespace xuitch::ui
