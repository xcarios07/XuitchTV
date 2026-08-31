#include "ui/PlayerActivity.hpp"

#include "ui/VideoView.hpp"

namespace xuitch::ui {

PlayerActivity::PlayerActivity(iptv::IptvChannel value)
    : channel(std::move(value)) {}

PlayerActivity::~PlayerActivity() {
    // The Activity base owns/deletes the content tree after our members. Free
    // the mpv render context explicitly while the Player handle is still alive.
    if (videoView) videoView->detachPlayer();
    player.stop();
}

void PlayerActivity::onContentAvailable() {
    videoHost = dynamic_cast<brls::Box*>(getView("player/video/host"));
    titleLabel = dynamic_cast<brls::Label*>(getView("player/title"));
    statusLabel = dynamic_cast<brls::Label*>(getView("player/status"));
    pauseButton = dynamic_cast<brls::Button*>(getView("player/pause"));
    stopButton = dynamic_cast<brls::Button*>(getView("player/stop"));

    if (!videoHost || !titleLabel || !statusLabel || !pauseButton || !stopButton) {
        brls::Application::crash("XuitchTV Player UI: faltan vistas requeridas.");
        return;
    }

    titleLabel->setText(channel.name.empty() ? "Canal IPTV" : channel.name);
    statusLabel->setText("Inicializando reproductor...");

    player.setStateCallback([this](player::PlayerState) {
        updateStatus();
    });

    videoView = new VideoView();
    videoView->setWidthPercentage(100);
    videoView->setHeightPercentage(100);
    videoHost->addView(videoView);

    if (!player.initialize()) {
        statusLabel->setText("MPV no disponible: " + player.lastError());
        pauseButton->setState(brls::ButtonState::DISABLED);
    } else {
        const bool attached = videoView->attachPlayer(&player);
        if (!attached) {
            statusLabel->setText("MPV listo; backend de video pendiente/no disponible.");
        }
        if (player.open(channel.url)) {
            statusLabel->setText(attached
                ? "Abriendo stream..."
                : "Stream enviado a MPV (sin superficie de video disponible)." );
        } else {
            statusLabel->setText("Error: " + player.lastError());
        }
    }

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
        brls::Application::popActivity(brls::TransitionAnimation::SLIDE_RIGHT);
        return true;
    });

    registerAction("Volver", brls::BUTTON_B, [this](brls::View*) {
        player.stop();
        brls::Application::popActivity(brls::TransitionAnimation::SLIDE_RIGHT);
        return true;
    });
}

void PlayerActivity::updateStatus() {
    if (!statusLabel) return;
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
