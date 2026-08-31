#include <cassert>
#include <string>
#include "player/Player.hpp"

int main() {
    xuitch::player::Player player;
    int callbacks = 0;
    xuitch::player::PlayerState last = xuitch::player::PlayerState::Idle;
    player.setStateCallback([&](xuitch::player::PlayerState state) {
        ++callbacks;
        last = state;
    });

    assert(!player.backendAvailable());
    assert(!player.initialize());
    assert(player.state() == xuitch::player::PlayerState::Error);
    assert(last == xuitch::player::PlayerState::Error);
    assert(callbacks == 1);
    assert(!player.lastError().empty());

    player.stop();
    assert(player.state() == xuitch::player::PlayerState::Stopped);
    assert(callbacks == 2);
    return 0;
}
