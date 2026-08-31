#include "ui/IptvActivity.hpp"

#include <cstdio>

namespace xuitch::ui {

namespace {
constexpr const char* kIptvLogPath = "sdmc:/switch/XuitchTV/iptv.log";

void iptvLog(const char* message)
{
    FILE* file = std::fopen(kIptvLogPath, "a");
    if (!file)
        return;
    std::fprintf(file, "%s\n", message);
    std::fflush(file);
    std::fclose(file);
}

void logView(const char* checkpoint, const void* view)
{
    char message[160] = {0};
    std::snprintf(message, sizeof(message), "%s - %s", checkpoint,
        view ? "found" : "MISSING");
    iptvLog(message);
}
} // namespace

void IptvActivity::onContentAvailable()
{
    iptvLog("[11] onContentAvailable entered - XML parsed");

    categoryBox = dynamic_cast<brls::Box*>(getView("iptv/categories"));
    logView("[12] iptv/categories", categoryBox);

    channelBox = dynamic_cast<brls::Box*>(getView("iptv/channels"));
    logView("[13] iptv/channels", channelBox);

    statusLabel = dynamic_cast<brls::Label*>(getView("iptv/status"));
    logView("[14] iptv/status", statusLabel);

    countLabel = dynamic_cast<brls::Label*>(getView("iptv/count"));
    logView("[15] iptv/count", countLabel);

    refreshButton = dynamic_cast<brls::Button*>(getView("iptv/refresh"));
    logView("[16] iptv/refresh", refreshButton);

    if (!categoryBox || !channelBox || !statusLabel || !countLabel || !refreshButton) {
        iptvLog("[17] diagnostic shell incomplete - required view missing");
        brls::Application::notify("IPTV diagnostico: faltan vistas XML");
        return;
    }

    refreshButton->registerClickAction([this](brls::View*) {
        iptvLog("[19] diagnostic refresh button pressed - network disabled");
        statusLabel->setText("Shell IPTV estable. Red/libcurl siguen desactivados.");
        brls::Application::notify("Diagnostico v0.5.5: sin red");
        return true;
    });

    statusLabel->setText("Diagnostico v0.5.5: shell sin red/libcurl/MPV.");
    countLabel->setText("0 canales");
    iptvLog("[18] diagnostic IPTV shell ready");
}

} // namespace xuitch::ui
