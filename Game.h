#ifndef GAME_H
#define GAME_H

#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Window.hpp>
#include<vector>
#include<iostream>
#include<algorithm>
#include<ctime>
#include<fstream>

using namespace sf;
using namespace std;

class Game
{

private:
	RenderWindow* window;//For Game Window
	View view;//For Viewing a certain portion of the Window
	Event ev;//For Event handling such as Keypress,Cursor,Mouse-clicks etc.
	VideoMode desktopMode;//For Window size

	Texture playerCar,policeCar, bg, dot;//Image data for Objects to use on screen
	Sprite plc, plr, rd, mark;//Objects
	float speed = 0;
	const double pi = 3.14159265358979323846;
	vector<Vector2f> points;//Set of (x,y) coordinates for drawing a Path

	void initVar();//Function to set Initial values of Variables
	void initWin();//Function to set Initial Window

public:
	Game();
	virtual ~Game();

	const bool getWinOpen() const;

	void pollEvents();
	void update();
	void render();
};

#endif
