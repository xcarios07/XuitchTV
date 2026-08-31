#include <borealis.hpp>

#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

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

    std::fprintf(file, "XuitchTV v0.5.2 diagnostic boot\n");
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

    // Match the locale setup used by mature Borealis Switch applications.
    std::setlocale(LC_ALL, "C.UTF-8");
    bootLog("[02] locale configured");

    // Capture Borealis logs too. The custom checkpoints above remain useful
    // even if Borealis itself crashes before emitting a message.
    FILE* borealisLog = std::fopen("sdmc:/switch/XuitchTV/borealis.log", "w+");
    if (borealisLog)
        brls::Logger::setLogOutput(borealisLog);
    brls::Logger::setLogLevel(brls::LogLevel::LOG_DEBUG);
    bootLog("[03] logger configured");

    bootLog("[04] before Application::init()");
    if (!brls::Application::init()) {
        bootLog("[05] Application::init() returned false");
        return EXIT_FAILURE;
    }
    bootLog("[05] Application::init() OK");

    // Same Switch lifecycle behavior used by Switchfin.
    if (brls::Application::getPlatform()) {
        bootLog("[06] platform object OK");
        brls::Application::getPlatform()->exitToHomeMode(true);
        bootLog(brls::Application::getPlatform()->isApplicationMode()
            ? "[07] full application mode"
            : "[07] applet mode");
    } else {
        bootLog("[06] ERROR platform object is null");
    }

    bootLog("[08] before createWindow()");
    brls::Application::createWindow("XuitchTV Diagnostic");
    bootLog("[09] createWindow() OK");

    brls::Application::setGlobalQuit(false);
    bootLog("[10] global quit configured");

    // IMPORTANT: no RomFS/XML, config, network or MPV in this diagnostic.
    // A programmatic view tells us whether Borealis + deko3d itself is stable.
    auto* label = new brls::Label();
    label->setText("XuitchTV diagnostico OK\nBorealis + deko3d iniciaron correctamente.\nPulsa + para salir.");
    label->setFontSize(28.0f);
    label->setHorizontalAlign(brls::HorizontalAlign::CENTER);
    label->setVerticalAlign(brls::VerticalAlign::CENTER);
    bootLog("[11] diagnostic label created");

    auto* activity = new brls::Activity(label);
    bootLog("[12] diagnostic activity created");

    brls::Application::pushActivity(activity, brls::TransitionAnimation::NONE);
    bootLog("[13] diagnostic activity pushed");

    unsigned long long frames = 0;
    bootLog("[14] entering mainLoop()");
    while (brls::Application::mainLoop()) {
        ++frames;
        if (frames == 1)
            bootLog("[15] first frame completed");
        else if (frames == 60)
            bootLog("[16] 60 frames completed - UI stable");
    }

    bootLog("[17] mainLoop exited normally");
    if (borealisLog) {
        std::fflush(borealisLog);
        std::fclose(borealisLog);
    }
    return EXIT_SUCCESS;
}
