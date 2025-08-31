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

class Signal {

private:
	vector<Clock> cycle;
	vector<vector<Color>> clr;
	vector<node*> cycleVar;
	vector<node*> current; //To cycle across the 4 signals in an Intersection
	const Time wait = seconds(15);
	float height, width;
	void initvar();
public:
	Signal();
	virtual ~Signal();	
	static vector<Vector2f> dir;
	static vector<Vector2f> pos;
	static vector<Vector2f> points;
	static vector<vector<bool>> stop;
	static vector<vector<vector<Vector2f>>> lightPos;
	void drawsignals(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y);
	void drawColors(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y, float cx, float cy, Color color);
	void signalctrl(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y);
};

#endif

