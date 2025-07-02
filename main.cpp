#include<iostream>
#include"Game.h"

using namespace sf;

int main() {
    Game game;
    while (game.getWinOpen()) {
        game.update();
        game.render();
    }
    return 0;
}