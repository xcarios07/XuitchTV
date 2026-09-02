#include "ui/PlayerActivity.hpp"

#include <cstdio>

#include "ui/VideoView.hpp"

namespace xuitch::ui {

namespace {
constexpr const char* kPlayerLogPath = "sdmc:/switch/XuitchTV/player.log";

void playerLog(const char* message, const char* mode = "a") {
    FILE* file = std::fopen(kPlayerLogPath, mode);
    if (!file) return;
    std::fprintf(file, "%s\n", message);
    std::fflush(file);
    std::fclose(file);
}

void playerLog(const std::string& message) {
    playerLog(message.c_str());
}

const char* stateName(player::PlayerState state) {
    switch (state) {
        case player::PlayerState::Opening: return "Opening";
        case player::PlayerState::Playing: return "Playing";
        case player::PlayerState::Paused: return "Paused";
        case player::PlayerState::Stopped: return "Stopped";
        case player::PlayerState::Error: return "Error";
        case player::PlayerState::Idle:
        default: return "Idle";
    }
}
} // namespace

PlayerActivity::PlayerActivity(iptv::IptvChannel value)
    : channel(std::move(value)) {
    playerLog("XuitchTV v0.6.1 OpenGL player diagnostic", "w");
    playerLog("[P01] PlayerActivity constructor completed");
}

PlayerActivity::~PlayerActivity() {
    playerLog("[P90] PlayerActivity destructor entered");
    // The Activity base owns/deletes the content tree after our members. Free
    // the mpv render context explicitly while the Player handle is still alive.
    if (videoView) videoView->detachPlayer();
    player.stop();
}

void PlayerActivity::onContentAvailable() {
    videoHost = dynamic_cast<brls::Box*>(getView("player/video/host"));
    titleLabel = dynamic_cast<brls::Label*>(getView("player/title"));
    statusLabel = dynamic_cast<brls::Label*>(getView("player/status"));
    playButton = dynamic_cast<brls::Button*>(getView("player/play"));
    pauseButton = dynamic_cast<brls::Button*>(getView("player/pause"));
    stopButton = dynamic_cast<brls::Button*>(getView("player/stop"));

    if (!videoHost || !titleLabel || !statusLabel || !playButton
        || !pauseButton || !stopButton) {
        brls::Application::crash("XuitchTV Player UI: faltan vistas requeridas.");
        return;
    }

    titleLabel->setText(channel.name.empty() ? "Canal IPTV" : channel.name);
    statusLabel->setText("Listo. Pulsa Reproducir para abrir el stream.");

    player.setStateCallback([this](player::PlayerState) {
        updateStatus();
    });

    videoView = new VideoView();
    videoView->setWidthPercentage(100);
    videoView->setHeightPercentage(100);
    videoHost->addView(videoView);
    pauseButton->setState(brls::ButtonState::DISABLED);
    playerLog("[P04] Player UI and VideoView ready - MPV not started");

    playButton->registerClickAction([this](brls::View*) {
        startPlayback();
        return true;
    });

    pauseButton->registerClickAction([this](brls::View*) {
        paused = !paused;
        if (!player.setPaused(paused)) {
            paused = !paused;
            updateStatus();
            return true;
        }
        pauseButton->setText(paused ? "Reanudar" : "Pausa");
        updateStatus();
        return true;
    });

    stopButton->registerClickAction([this](brls::View*) {
        player.stop();
        brls::Application::popActivity(brls::TransitionAnimation::NONE);
        return true;
    });

    registerAction("Volver", brls::BUTTON_B, [this](brls::View*) {
        player.stop();
        brls::Application::popActivity(brls::TransitionAnimation::NONE);
        return true;
    });
}

void PlayerActivity::willAppear(bool resetState) {
    playerLog("[P05] PlayerActivity willAppear entered");
    brls::Activity::willAppear(resetState);
    playerLog("[P06] PlayerActivity base willAppear returned");
}

void PlayerActivity::startPlayback() {
    if (started) return;

    playerLog("[P20] Reproducir clicked - before player.initialize");
    statusLabel->setText("Inicializando MPV...");
    if (!player.initialize()) {
        playerLog("[P21] player.initialize failed");
        statusLabel->setText("MPV no disponible: " + player.lastError());
        return;
    }
    playerLog("[P21] player.initialize returned success");

    const bool attached = videoView->attachPlayer(&player);
    playerLog(attached
        ? "[P22] VideoView attachPlayer returned success"
        : "[P22] VideoView attachPlayer returned failure");

    if (!player.open(channel.url)) {
        playerLog("[P23] player.open returned failure");
        statusLabel->setText("Error: " + player.lastError());
        return;
    }
    playerLog("[P23] player.open returned success");
    started = true;
    playButton->setState(brls::ButtonState::DISABLED);
    pauseButton->setState(brls::ButtonState::ENABLED);
    statusLabel->setText(attached
        ? "Abriendo stream..."
        : "Stream abierto; superficie de video no disponible.");
}

void PlayerActivity::updateStatus() {
    if (!statusLabel) return;
    std::string event = std::string("[P30] state=") + stateName(player.state());
    if (!player.lastError().empty()) event += " error=" + player.lastError();
    playerLog(event);
    if (!player.lastError().empty()) {
        statusLabel->setText("Error: " + player.lastError());
        return;
    }
    switch (player.state()) {
        case player::PlayerState::Opening: statusLabel->setText("Abriendo stream..."); break;
        case player::PlayerState::Playing: statusLabel->setText("Reproduciendo"); break;
        case player::PlayerState::Paused: statusLabel->setText("Pausado"); break;
        case player::PlayerState::Stopped: statusLabel->setText("Detenido"); break;
        case player::PlayerState::Error: statusLabel->setText("Error de reproduccion"); break;
        case player::PlayerState::Idle:
        default: statusLabel->setText("Listo"); break;
    }
}

} // namespace xuitch::ui
