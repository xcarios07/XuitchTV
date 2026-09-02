#include <borealis.hpp>

#include "activity/launcher_activity.hpp"
#include "utils/activity_helper.hpp"

namespace {

void showPortalNotice(const std::string& section) {
    auto* dialog = new brls::Dialog(
        section + " se cargará desde una fuente M3U/Xtream autorizada. "
        "Configura el servidor, usuario y contraseña en IPTV Settings.");
    dialog->addButton("Volver", []() {});
    dialog->addButton("Configurar", []() { Intent::openSettings(); });
    dialog->open();
}

}  // namespace

void LauncherActivity::onContentAvailable() {
    auto* tv = dynamic_cast<brls::Button*>(getView("launcher/tv"));
    auto* portal = dynamic_cast<brls::Button*>(getView("launcher/portal"));
    auto* movies = dynamic_cast<brls::Button*>(getView("launcher/movies"));
    auto* series = dynamic_cast<brls::Button*>(getView("launcher/series"));
    auto* sports = dynamic_cast<brls::Button*>(getView("launcher/sports"));
    auto* settings = dynamic_cast<brls::Button*>(getView("launcher/settings"));

    if (tv) {
        tv->setStyle(&brls::BUTTONSTYLE_PRIMARY);
        tv->registerClickAction([](brls::View*) {
            Intent::openTV();
            return true;
        });
    }
    if (portal) {
        portal->registerClickAction([](brls::View*) {
            Intent::openSettings();
            return true;
        });
    }
    if (movies) {
        movies->registerClickAction([](brls::View*) {
            showPortalNotice("Películas");
            return true;
        });
    }
    if (series) {
        series->registerClickAction([](brls::View*) {
            showPortalNotice("Series");
            return true;
        });
    }
    if (sports) {
        sports->registerClickAction([](brls::View*) {
            Intent::openTV();
            return true;
        });
    }
    if (settings) {
        settings->registerClickAction([](brls::View*) {
            Intent::openSettings();
            return true;
        });
    }
}
