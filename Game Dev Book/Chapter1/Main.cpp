/// This project is based off the book "Game Programming in C++"

#include "Game.hpp"

int main(int argc, char** argv) {
    // create an instance of the Game class
    Game game;

    // initialize the game
    bool success = game.Initialize();
    if (success) {
        // run game loop
        game.RunLoop();
    }
    // game loop finished, quit game
    game.ShutDown();
    return 0;
}
