#include "core/Game.h"
#include "core/SettingsManager.h"

int main(int argc, char* argv[]) {
    Core::SettingsManager settings;
    settings.Load("settings.json");

    Core::Game game(std::move(settings));
    game.Run();
    return 0;
}