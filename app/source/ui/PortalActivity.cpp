#include "ui/PortalActivity.hpp"

#include <cstdio>
#include <utility>

#include "api/HttpClient.hpp"
#include "core/AppConfig.hpp"
#include "core/BuildInfo.hpp"
#include "core/ConfigStore.hpp"

namespace xuitch::ui {
namespace {
constexpr const char* kPortalLogPath = "sdmc:/switch/XuitchTV/portal.log";

void portalLog(const std::string& message)
{
    FILE* file = std::fopen(kPortalLogPath, "a");
    if (!file)
        return;
    std::fprintf(file, "%s\n", message.c_str());
    std::fflush(file);
    std::fclose(file);
}

bool isHttpUrl(const std::string& value)
{
    return value.rfind("https://", 0) == 0 || value.rfind("http://", 0) == 0;
}
} // namespace

PortalActivity::PortalActivity(std::string requestedSection)
    : section(std::move(requestedSection))
{
}

void PortalActivity::onContentAvailable()
{
    titleLabel = dynamic_cast<brls::Label*>(getView("portal/title"));
    stateLabel = dynamic_cast<brls::Label*>(getView("portal/state"));
    detailLabel = dynamic_cast<brls::Label*>(getView("portal/detail"));
    hostLabel = dynamic_cast<brls::Label*>(getView("portal/host"));
    portalCodeLabel = dynamic_cast<brls::Label*>(getView("portal/code"));
    deviceLabel = dynamic_cast<brls::Label*>(getView("portal/device"));
    reloadButton = dynamic_cast<brls::Button*>(getView("portal/reload"));
    probeButton = dynamic_cast<brls::Button*>(getView("portal/probe"));

    if (titleLabel)
        titleLabel->setText(section);

    if (reloadButton) {
        reloadButton->registerClickAction([this](brls::View*) {
            reloadConfig();
            return true;
        });
    }
    if (probeButton) {
        probeButton->registerClickAction([this](brls::View*) {
            probeServer();
            return true;
        });
    }

    portalLog("[01] PortalActivity ready: " + section);
    reloadConfig();
}

bool PortalActivity::portalConfigured() const
{
    const auto& session = core::AppConfig::instance().session();
    return isHttpUrl(session.portalBaseUrl)
        && session.portalBaseUrl.find("YOUR_AUTHORIZED") == std::string::npos
        && !session.portalCode.empty();
}

void PortalActivity::reloadConfig()
{
    core::Session loaded;
    std::string error;
    const std::string path = core::ConfigStore::defaultPath();
    const bool loadedFile = core::ConfigStore::load(path, loaded, &error);
    core::AppConfig::instance().session() = std::move(loaded);

    portalLog(loadedFile
        ? "[02] config.json loaded"
        : "[02] config.json not loaded: " + error);

    if (portalConfigured()) {
        renderStatus("Configuracion cargada. Prueba el servidor antes de iniciar sesion.");
    } else if (loadedFile) {
        renderStatus("Faltan portalBaseUrl o portalCode en config.json.");
    } else {
        renderStatus("Copia config.example.json como config.json y agrega los datos de tu servicio autorizado.");
    }
}

void PortalActivity::renderStatus(const std::string& detail)
{
    const auto& session = core::AppConfig::instance().session();
    const bool configured = portalConfigured();

    if (stateLabel) {
        stateLabel->setText(session.authenticated
            ? "SESION AUTENTICADA"
            : configured ? "PORTAL CONFIGURADO" : "CONFIGURACION PENDIENTE");
    }
    if (detailLabel)
        detailLabel->setText(detail);
    if (hostLabel)
        hostLabel->setText(session.portalBaseUrl.empty() ? "Sin servidor" : session.portalBaseUrl);
    if (portalCodeLabel)
        portalCodeLabel->setText(session.portalCode.empty() ? "No configurado" : "Configurado");
    if (deviceLabel)
        deviceLabel->setText(session.deviceId.empty() ? "Se generara al iniciar sesion" : "Configurado");
    if (probeButton)
        probeButton->setState(configured ? brls::ButtonState::ENABLED : brls::ButtonState::DISABLED);
}

void PortalActivity::probeServer()
{
    if (!portalConfigured()) {
        brls::Application::notify("Primero configura tu portal autorizado");
        return;
    }

    const auto& session = core::AppConfig::instance().session();
    renderStatus("Comprobando conexion con el servidor...");
    brls::Application::blockInputs();

    api::HttpClient http;
    http.setTimeoutSeconds(10);
    http.setUserAgent(core::userAgent());
    const auto response = http.get(session.portalBaseUrl);

    brls::Application::unblockInputs();
    if (response.statusCode > 0) {
        const std::string result = "Servidor accesible (HTTP "
            + std::to_string(response.statusCode)
            + "). El siguiente paso es autenticar y descargar el catalogo.";
        portalLog("[03] portal probe HTTP " + std::to_string(response.statusCode));
        renderStatus(result);
        brls::Application::notify("Servidor del portal accesible");
    } else {
        const std::string reason = response.error.empty()
            ? "sin respuesta HTTP" : response.error;
        portalLog("[03] portal probe failed: " + reason);
        renderStatus("No se pudo conectar: " + reason);
        brls::Application::notify("No se pudo conectar con el portal");
    }
}

} // namespace xuitch::ui
