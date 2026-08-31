#include <borealis.hpp>

#if defined(__SWITCH__)
#include <switch.h>
#endif

#include "core/AppConfig.hpp"
#include "core/ConfigStore.hpp"
#include "ui/MainActivity.hpp"

int main(int argc, char* argv[])
{
    if (!brls::Application::init())
        return EXIT_FAILURE;

    auto& session = xuitch::core::AppConfig::instance().session();
    std::string configError;
    xuitch::core::ConfigStore::load(xuitch::core::ConfigStore::defaultPath(), session, &configError);

    brls::Application::createWindow("XuitchTV");
    brls::Application::setGlobalQuit(true);
    brls::Application::pushActivity(new xuitch::ui::MainActivity());

#if defined(__SWITCH__)
    if (appletGetAppletType() != AppletType_Application) {
        brls::Application::notify(
            "XuitchTV: para video estable, inicia Homebrew Menu con title takeover (memoria completa).");
    }
#endif

    while (brls::Application::mainLoop()) {}
    return EXIT_SUCCESS;
}
