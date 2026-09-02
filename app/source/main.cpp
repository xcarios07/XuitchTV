#include <borealis.hpp>

#include <clocale>
#include <cstdio>
#include <cstdlib>

#include "ui/MainActivity.hpp"
#include "ui/SplashActivity.hpp"

namespace {
constexpr const char* kBootLogPath = "sdmc:/switch/XuitchTV/startup.log";

void bootLog(const char* message)
{
    FILE* file = std::fopen(kBootLogPath, "a");
    if (!file)
        return;
    std::fprintf(file, "%s\n", message);
    std::fflush(file);
    std::fclose(file);
}

void resetBootLog()
{
    FILE* file = std::fopen(kBootLogPath, "w");
    if (!file)
        return;
    std::fprintf(file, "XuitchTV v0.6.0 media preview\n");
    std::fflush(file);
    std::fclose(file);
}
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    resetBootLog();
    bootLog("[01] entered main()");

    std::setlocale(LC_ALL, "C.UTF-8");
    bootLog("[02] locale configured");

    bootLog("[03] before Application::init()");
    if (!brls::Application::init()) {
        bootLog("[04] Application::init() returned false");
        return EXIT_FAILURE;
    }
    bootLog("[04] Application::init() OK");

    if (brls::Application::getPlatform()) {
        brls::Application::getPlatform()->exitToHomeMode(true);
        bootLog("[05] platform configured");
    }

    bootLog("[06] before createWindow()");
    brls::Application::createWindow("XuitchTV");
    bootLog("[07] createWindow() OK");

    brls::Application::setGlobalQuit(true);
    // Keep the main menu as the unpoppable root activity and briefly overlay
    // the splash. This prevents Back from ever returning to a stale splash.
    bootLog("[08] before MainActivity root construction");
    auto* mainActivity = new xuitch::ui::MainActivity();
    brls::Application::pushActivity(mainActivity, brls::TransitionAnimation::NONE);
    bootLog("[09] MainActivity root pushed");

    auto* splashActivity = new xuitch::ui::SplashActivity();
    brls::Application::pushActivity(splashActivity, brls::TransitionAnimation::NONE);
    bootLog("[10] SplashActivity overlay pushed");

    for (unsigned int frame = 0; frame < 75; ++frame) {
        if (!brls::Application::mainLoop())
            return EXIT_SUCCESS;
    }
    brls::Application::popActivity(brls::TransitionAnimation::NONE);
    bootLog("[11] splash completed and overlay popped");

    unsigned long long frames = 0;
    bootLog("[12] entering mainLoop()");
    while (brls::Application::mainLoop()) {
        ++frames;
        if (frames == 1)
            bootLog("[13] first menu frame completed");
        else if (frames == 60)
            bootLog("[14] 60 menu frames completed - UI stable");
    }

    bootLog("[15] mainLoop exited normally");
    return EXIT_SUCCESS;
}
