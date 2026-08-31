#include "ui/IptvActivity.hpp"

#include <sstream>

#include "core/AppConfig.hpp"
#include "core/BuildInfo.hpp"
#include "ui/PlayerActivity.hpp"

namespace xuitch::ui {

namespace {
std::string healthPrefix(iptv::StreamHealth health) {
    switch (health) {
        case iptv::StreamHealth::Reachable: return "[OK] ";
        case iptv::StreamHealth::Unreachable: return "[OFF] ";
        case iptv::StreamHealth::Unknown:
        default: return "";
    }
}
}

IptvActivity::IptvActivity()
    : service(http) {
    http.setTimeoutSeconds(15);
    http.setUserAgent(core::userAgent());
}

void IptvActivity::onContentAvailable() {
    categoryBox = dynamic_cast<brls::Box*>(getView("iptv/categories"));
    channelBox = dynamic_cast<brls::Box*>(getView("iptv/channels"));
    statusLabel = dynamic_cast<brls::Label*>(getView("iptv/status"));
    countLabel = dynamic_cast<brls::Label*>(getView("iptv/count"));
    refreshButton = dynamic_cast<brls::Button*>(getView("iptv/refresh"));

    if (!categoryBox || !channelBox || !statusLabel || !countLabel || !refreshButton) {
        brls::Application::crash("XuitchTV IPTV UI: faltan vistas requeridas.");
        return;
    }

    refreshButton->registerClickAction([this](brls::View*) {
        refreshPlaylist();
        return true;
    });

    setStatus("Pulsa A en Actualizar para cargar IPTV Paraguay.");
    countLabel->setText("0 canales");

    auto& session = core::AppConfig::instance().session();
    if (!session.iptvEnabled) {
        setStatus("IPTV esta desactivado en config.json.");
        refreshButton->setState(brls::ButtonState::DISABLED);
    }
}

void IptvActivity::refreshPlaylist() {
    auto& session = core::AppConfig::instance().session();
    if (!session.iptvEnabled) {
        setStatus("IPTV esta desactivado.");
        return;
    }

    setStatus("Cargando playlist IPTV...");
    brls::Application::blockInputs();

    std::string error;
    const bool ok = service.refresh(session.iptvPlaylistUrl, playlist, &error);

    brls::Application::unblockInputs();

    if (!ok) {
        setStatus("No se pudo cargar IPTV: " + error);
        brls::Application::notify("Error al cargar IPTV");
        return;
    }

    navigator.setPlaylist(playlist, session.iptvHideUnavailable);
    navigator.selectCategory("Todos");
    renderCategories();
    renderChannels();

    std::ostringstream text;
    text << "Playlist actualizada: " << playlist.channels.size() << " canales.";
    setStatus(text.str());
    brls::Application::notify("IPTV Paraguay actualizado");
}

void IptvActivity::renderCategories() {
    clearBox(categoryBox);

    for (const auto& category : navigator.categories()) {
        auto* button = new brls::Button();
        button->setText(category);
        button->setStyle(category == navigator.selectedCategory()
            ? &brls::BUTTONSTYLE_HIGHLIGHT
            : &brls::BUTTONSTYLE_DEFAULT);
        button->setMarginBottom(8);
        button->registerClickAction([this, category](brls::View*) {
            if (!navigator.selectCategory(category)) return false;
            // Do not rebuild the category box from inside its own button
            // callback: that would delete the currently executing view.
            renderChannels();
            return true;
        });
        categoryBox->addView(button);
    }
}

void IptvActivity::renderChannels() {
    clearBox(channelBox);
    const auto channels = navigator.visibleChannels();

    countLabel->setText(std::to_string(channels.size()) + " canales");

    if (channels.empty()) {
        auto* empty = new brls::Label();
        empty->setText("No hay canales para esta categoria.");
        empty->setFontSize(18);
        channelBox->addView(empty);
        return;
    }

    for (const auto* channel : channels) {
        if (!channel) continue;
        auto* button = new brls::Button();
        std::string text = healthPrefix(channel->health) + channel->name;
        if (!channel->groupTitle.empty() && navigator.selectedCategory() == "Todos") {
            text += "  -  " + channel->groupTitle;
        }
        button->setText(text);
        button->setMarginBottom(8);

        // Copy the channel into the callback: the activity/player remains safe
        // if the playlist is refreshed later.
        const iptv::IptvChannel selected = *channel;
        button->registerClickAction([selected](brls::View*) {
            brls::Application::pushActivity(
                new PlayerActivity(selected),
                brls::TransitionAnimation::SLIDE_LEFT);
            return true;
        });
        channelBox->addView(button);
    }
}

void IptvActivity::clearBox(brls::Box* box) {
    if (!box) return;
    auto& children = box->getChildren();
    while (!children.empty()) {
        box->removeView(children.back());
    }
}

void IptvActivity::setStatus(const std::string& text) {
    if (statusLabel) statusLabel->setText(text);
}

} // namespace xuitch::ui
