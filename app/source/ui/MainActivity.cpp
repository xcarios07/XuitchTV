#include "ui/MainActivity.hpp"

#include <cstdio>
#include <string>

#include "core/AppConfig.hpp"
#include "ui/IptvActivity.hpp"
#include "ui/PortalActivity.hpp"

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
        versionLabel->setText("XuitchTV v0.7.0 - Portal Foundation");
    }

    if (iptvButton) {
        iptvButton->setStyle(&brls::BUTTONSTYLE_PRIMARY);
        iptvButton->registerClickAction([](brls::View*) {
            iptvLog("XuitchTV v0.7.0 portal foundation", "w");
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

    const auto openPortalSection = [](const std::string& section) {
        brls::Application::pushActivity(new PortalActivity(section),
            brls::TransitionAnimation::NONE);
    };
    if (portalButton) {
        portalButton->registerClickAction([openPortalSection](brls::View*) {
            openPortalSection("PORTAL TV");
            return true;
        });
    }
    if (moviesButton) {
        moviesButton->registerClickAction([openPortalSection](brls::View*) {
            openPortalSection("PELICULAS");
            return true;
        });
    }
    if (seriesButton) {
        seriesButton->registerClickAction([openPortalSection](brls::View*) {
            openPortalSection("SERIES");
            return true;
        });
    }
    if (sportsButton) {
        sportsButton->registerClickAction([openPortalSection](brls::View*) {
            openPortalSection("DEPORTES");
            return true;
        });
    }

    auto* serviceStatus = dynamic_cast<brls::Label*>(getView("main/service/status"));
    auto* vodStatus = dynamic_cast<brls::Label*>(getView("main/vod/status"));
    auto* sportsStatus = dynamic_cast<brls::Label*>(getView("main/sports/status"));
    const auto& session = core::AppConfig::instance().session();
    const bool portalConfigured = !session.portalBaseUrl.empty()
        && !session.portalCode.empty()
        && session.portalBaseUrl.find("YOUR_AUTHORIZED") == std::string::npos;
    if (serviceStatus)
        serviceStatus->setText(portalConfigured ? "●  PORTAL CONFIGURADO" : "●  IPTV EN LINEA");
    if (vodStatus)
        vodStatus->setText(portalConfigured
            ? "Portal listo para validar conexion." : "Configura tu portal autorizado para continuar.");
    if (sportsStatus)
        sportsStatus->setText(portalConfigured
            ? "Portal listo para validar conexion." : "Disponible al configurar el portal.");
}

void MainActivity::onPause()
{
    iptvLog("[06] MainActivity onPause entered");
}

} // namespace xuitch::ui
