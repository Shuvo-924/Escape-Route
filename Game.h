#ifndef GAME_H
#define GAME_H

#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Window.hpp>
#include<vector>
#include<queue>
#include<iostream>
#include<algorithm>
#include<ctime>
#include<cmath>
#include<list>

using namespace sf;
using namespace std;

class Game
{

private:
	RenderWindow* window;
	Event ev;
	VideoMode desktopMode;

	void initVar();
	void initWin();

public:
	Game();
	virtual ~Game();

	const bool getWinOpen() const;

	void pollEvents();
	void update();
	void render();
};

#endif
