#include <borealis.hpp>

#include <clocale>
#include <cstdio>
#include <cstdlib>

#include "ui/MainActivity.hpp"

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
    std::fprintf(file, "XuitchTV v0.5.6 IPTV navigation diagnostic\n");
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
    bootLog("[08] before MainActivity construction");

    // UI restore test only: no config.json, network, IPTV refresh or MPV startup here.
    auto* mainActivity = new xuitch::ui::MainActivity();
    bootLog("[09] MainActivity constructed (XML loaded)");

    brls::Application::pushActivity(mainActivity, brls::TransitionAnimation::NONE);
    bootLog("[10] MainActivity pushed");

    unsigned long long frames = 0;
    bootLog("[11] entering mainLoop()");
    while (brls::Application::mainLoop()) {
        ++frames;
        if (frames == 1)
            bootLog("[12] first UI frame completed");
        else if (frames == 60)
            bootLog("[13] 60 UI frames completed - real UI stable");
    }

    bootLog("[14] mainLoop exited normally");
    return EXIT_SUCCESS;
}
