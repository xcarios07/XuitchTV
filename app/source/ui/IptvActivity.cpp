#include "ui/IptvActivity.hpp"

#include <cstdio>
#include <cctype>
#include <sstream>

#include "api/HttpClient.hpp"
#include "core/BuildInfo.hpp"
#include "iptv/IptvService.hpp"
#include "ui/PlayerActivity.hpp"

namespace xuitch::ui {

namespace {
constexpr const char* kIptvLogPath = "sdmc:/switch/XuitchTV/iptv.log";
constexpr const char* kParaguayPlaylistUrl =
    "https://raw.githubusercontent.com/iptv-org/iptv/master/streams/py.m3u";

void iptvLog(const char* message)
{
    FILE* file = std::fopen(kIptvLogPath, "a");
    if (!file)
        return;
    std::fprintf(file, "%s\n", message);
    std::fflush(file);
    std::fclose(file);
}

void iptvLog(const std::string& message)
{
    iptvLog(message.c_str());
}

void logView(const char* checkpoint, const void* view)
{
    char message[160] = {0};
    std::snprintf(message, sizeof(message), "%s - %s", checkpoint,
        view ? "found" : "MISSING");
    iptvLog(message);
}

std::string logoResourcePath(std::string tvgId)
{
    const std::string suffix = "@SD";
    if (tvgId.size() >= suffix.size()
        && tvgId.compare(tvgId.size() - suffix.size(), suffix.size(), suffix) == 0) {
        tvgId.erase(tvgId.size() - suffix.size());
    }

    // The current public catalog has no usable Unicanal raster logo.
    if (tvgId == "Unicanal.py" || tvgId.empty())
        return "images/xuitchtv_logo_transparent.png";

    for (char& value : tvgId) {
        const auto byte = static_cast<unsigned char>(value);
        if (!std::isalnum(byte) && value != '_' && value != '-')
            value = '_';
    }
    return "images/channels/" + tvgId + ".png";
}

brls::Box* makeChannelCard(const iptv::IptvChannel& selected, bool rightMargin)
{
    auto* card = new brls::Box(brls::Axis::ROW);
    card->setWidthPercentage(49);
    card->setHeight(118);
    if (rightMargin) card->setMarginRight(14);
    card->setPadding(10);
    card->setAlignItems(brls::AlignItems::CENTER);
    card->setBackgroundColor(nvgRGB(255, 255, 255));
    card->setCornerRadius(10);
    card->setFocusable(true);
    card->setHideHighlightBackground(false);

    auto* logo = new brls::Image();
    logo->setWidth(142);
    logo->setHeight(88);
    logo->setScalingType(brls::ImageScalingType::FIT);
    logo->setImageFromRes(logoResourcePath(selected.tvgId));
    logo->setMarginRight(14);
    card->addView(logo);

    auto* details = new brls::Box(brls::Axis::COLUMN);
    details->setGrow(1);

    auto* name = new brls::Label();
    name->setText(selected.name.empty() ? "Canal IPTV" : selected.name);
    name->setFontSize(17);
    name->setTextColor(nvgRGB(25, 31, 43));
    details->addView(name);

    auto* country = new brls::Label();
    country->setText(selected.groupTitle.empty()
        ? "Paraguay  ·  IPTV publico"
        : selected.groupTitle);
    country->setFontSize(13);
    country->setTextColor(nvgRGB(237, 113, 18));
    country->setMarginTop(7);
    details->addView(country);
    card->addView(details);

    card->registerClickAction([selected](brls::View*) {
        iptvLog("[60] channel selected: " + selected.name);
        auto* playerActivity = new PlayerActivity(selected);
        iptvLog("[61] PlayerActivity constructed");
        brls::Application::pushActivity(playerActivity,
            brls::TransitionAnimation::NONE);
        iptvLog("[62] PlayerActivity pushed");
        return true;
    });
    return card;
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
        refreshPlaylist();
        return true;
    });

    statusLabel->setText("Pulsa Actualizar para cargar IPTV Paraguay.");
    countLabel->setText("0 canales");
    iptvLog("[18] IPTV shell ready - waiting for manual refresh");
}

void IptvActivity::willAppear(bool resetState)
{
    iptvLog("[20] IptvActivity willAppear entered");
    brls::Activity::willAppear(resetState);
    iptvLog("[21] IptvActivity base willAppear returned");

    iptvLog("[22] before default focus probe");
    auto* content = getContentView();
    auto* focus = content ? content->getDefaultFocus() : nullptr;
    logView("[23] default focus probe", focus);
}

void IptvActivity::refreshPlaylist()
{
    iptvLog("[30] refreshPlaylist entered");
    setStatus("Conectando con la lista IPTV de Paraguay...");
    brls::Application::blockInputs();

    iptvLog("[31] before HttpClient construction");
    api::HttpClient http;
    http.setTimeoutSeconds(12);
    http.setUserAgent(core::userAgent());
    iptvLog("[32] HttpClient constructed");

    iptv::IptvService service(http);
    std::string error;
    iptvLog("[33] before HTTP download and M3U parse");
    const bool ok = service.refresh(kParaguayPlaylistUrl, playlist, &error);
    iptvLog(ok
        ? "[34] service.refresh returned success"
        : "[34] service.refresh returned failure: " + error);

    brls::Application::unblockInputs();

    if (!ok) {
        setStatus("No se pudo cargar IPTV: " + error);
        brls::Application::notify("Error al descargar la lista IPTV");
        return;
    }

    iptvLog("[35] parsed channels: " + std::to_string(playlist.channels.size()));
    navigator.setPlaylist(playlist, false);
    navigator.selectCategory("Todos");
    iptvLog("[36] navigator configured");

    iptvLog("[37] before renderCategories");
    renderCategories();
    iptvLog("[38] categories rendered: "
        + std::to_string(navigator.categories().size()));

    iptvLog("[39] before renderChannels");
    renderChannels();
    iptvLog("[40] visible channels rendered: "
        + std::to_string(navigator.visibleCount()));

    std::ostringstream text;
    text << "Lista cargada: " << playlist.channels.size()
         << " canales. Selecciona uno para abrir el reproductor.";
    setStatus(text.str());
    brls::Application::notify("IPTV Paraguay cargado");
    iptvLog("[41] refreshPlaylist completed");
}

void IptvActivity::renderCategories()
{
    clearBox(categoryBox);

    for (const auto& category : navigator.categories()) {
        auto* button = new brls::Button();
        button->setText(category);
        button->setMarginBottom(8);
        button->registerClickAction([this, category](brls::View*) {
            if (!navigator.selectCategory(category))
                return false;
            iptvLog("[50] category selected: " + category);
            renderChannels();
            return true;
        });
        categoryBox->addView(button);
    }
}

void IptvActivity::renderChannels()
{
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

    for (std::size_t index = 0; index < channels.size(); index += 2) {
        auto* row = new brls::Box(brls::Axis::ROW);
        row->setWidthPercentage(100);
        row->setMarginBottom(14);

        if (channels[index])
            row->addView(makeChannelCard(*channels[index], true));

        if (index + 1 < channels.size() && channels[index + 1]) {
            row->addView(makeChannelCard(*channels[index + 1], false));
        } else {
            auto* spacer = new brls::Box(brls::Axis::COLUMN);
            spacer->setWidthPercentage(49);
            row->addView(spacer);
        }
        channelBox->addView(row);
    }
}

void IptvActivity::clearBox(brls::Box* box)
{
    if (!box)
        return;

    auto& children = box->getChildren();
    while (!children.empty())
        box->removeView(children.back());
}

void IptvActivity::setStatus(const std::string& text)
{
    if (statusLabel)
        statusLabel->setText(text);
}

} // namespace xuitch::ui
