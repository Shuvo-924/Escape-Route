#ifndef SIGNALCONTROL_H
#define SIGNALCONTROL_H

#include <SFML/Graphics.hpp>
#include <fstream>

using namespace std;
using namespace sf;

struct node{
	node* next;
	int val;
};

struct sign {
	bool stop;
	float dir;
};

class Signal {

private:
	vector<Vector2f> dir;
	vector<vector<vector<Vector2f>>> lightPos;
	vector<vector<Color>> clr;
	vector<Clock> cycle;
	node* cycleVar;
	node* current;
	float height, width;
	void initvar();
public:
	Signal();
	virtual ~Signal();
	vector<Vector2f> pos;
	vector<Vector2f> points;
	vector<vector<sign>> stop;
	void drawsignals(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y);
	void drawColors(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y, float cx, float cy, Color color);
	void signalctrl(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y);
};

#endif

