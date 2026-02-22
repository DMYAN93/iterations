#include "core/Game.h"

int main(int argc, char* argv[]) {
    Core::Game game("Iterations", 800, 600);
    game.Run();
    return 0;
}