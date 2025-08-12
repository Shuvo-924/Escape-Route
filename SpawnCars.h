#ifndef SPAWNCARS_H
#define SPAWNCARS_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <unordered_map>
#include "CustomFunctions.h"
#include "SpatialGrid.h"

using namespace sf;
using namespace std;

struct Car {
	Sprite body;
	double speed;
	unsigned int id;
};

class Cars {

private:
	SpatialGrid grid;
	vector<Texture> skins;
	vector<Car> bodies;
	vector<pair<Vector2f,float>> points;
	list<Car> moving;
	unsigned int plr_id;
	double scaleX = 29335.0f / 1920.0f;
	double scaleY = 16504.0f / 1080.0f;
	Vector2f size;
	Clock SpawnDelay;
	Time wait = seconds(30);
	void initVar();

public:
	Cars(float worldWidth, float worldHeight);
	void AddCars();
	void Spawn();
	void drawCars(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y, Sprite& plr, double& spd);
};

#endif
