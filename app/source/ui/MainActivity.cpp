#include "ui/MainActivity.hpp"

#include <cstdio>

#include "ui/IptvActivity.hpp"

namespace xuitch::ui {

namespace {
constexpr const char* kIptvLogPath = "sdmc:/switch/XuitchTV/iptv.log";

void iptvLog(const char* message, const char* mode = "a")
{
    FILE* file = std::fopen(kIptvLogPath, mode);
    if (!file)
        return;
    std::fprintf(file, "%s\n", message);
    std::fflush(file);
    std::fclose(file);
}
} // namespace

void MainActivity::onContentAvailable()
{
    auto* iptvButton = dynamic_cast<brls::Button*>(getView("main/iptv/button"));
    auto* portalButton = dynamic_cast<brls::Button*>(getView("main/portal/button"));
    auto* moviesButton = dynamic_cast<brls::Button*>(getView("main/movies/button"));
    auto* seriesButton = dynamic_cast<brls::Button*>(getView("main/series/button"));
    auto* sportsButton = dynamic_cast<brls::Button*>(getView("main/sports/button"));
    auto* versionLabel = dynamic_cast<brls::Label*>(getView("main/version"));

    if (versionLabel) {
        versionLabel->setText("XuitchTV v0.6.2 - Stream Headers Preview");
    }

    if (iptvButton) {
        iptvButton->setStyle(&brls::BUTTONSTYLE_PRIMARY);
        iptvButton->registerClickAction([](brls::View*) {
            iptvLog("XuitchTV v0.6.2 stream headers preview", "w");
            iptvLog("[01] IPTV button click callback entered");
            iptvLog("[02] before new IptvActivity");
            auto* iptvActivity = new IptvActivity();
            iptvLog("[03] new IptvActivity returned");
            iptvLog("[04] before pushActivity - transition NONE");
            brls::Application::pushActivity(iptvActivity,
                brls::TransitionAnimation::NONE);
            iptvLog("[05] pushActivity returned");
            return true;
        });
    }

    // The portal API core exists, but its final browsing UI is not ready yet.
    if (portalButton) portalButton->setState(brls::ButtonState::DISABLED);
    if (moviesButton) moviesButton->setState(brls::ButtonState::DISABLED);
    if (seriesButton) seriesButton->setState(brls::ButtonState::DISABLED);
    if (sportsButton) sportsButton->setState(brls::ButtonState::DISABLED);
}

void MainActivity::onPause()
{
    iptvLog("[06] MainActivity onPause entered");
}

} // namespace xuitch::ui
