#ifndef SPAWNCARS_H
#define SPAWNCARS_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <utility>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include "CustomFunctions.h"
#include "SpatialGrid.h"

using namespace sf;
using namespace std;

struct Car { //Struct Car for holding individual Cars Data
	Sprite body;
	double speed;
	double lastspeed;
	Sound engineSound;
	Sound brakeSound;
	Sound hornSound;
	bool still = false;
	Clock honk;
	bool out = false;
	unsigned int id; //Unique ID to identify each car
	unordered_set<Vector2f, Vector2fHash> vis;
	bool isFollowingCurve;
	vector<Vector2f> currentCurvePath; //To store the ordered path of the current curve
	size_t curvePathIndex; // To track progress along the path
};

class Cars {

private:
	SpatialGrid grid; //To limit the area of scanning for nearby cars
	vector<Texture> skins; //To store Car Images
	vector<Car> bodies; //To store the Drawable Objects(Cars)
	vector<pair<Vector2f,float>> points;
	list<Car> moving; //To iterate through the Cars currently on the map and control their movement
	unordered_set<Vector2f, Vector2fHash> curveStartPoints; //Coordinates for where a car needs to start turning
	unordered_set<Vector2f, Vector2fHash> allCurvePoints; //All coordinates of the Curved Paths
	unordered_map<Vector2f, vector<Vector2f>, Vector2fHash> curveGraph; //To store the ordered edges of the Curves
	SoundBuffer engineRunningBuffer;
	//Same as Players car
	SoundBuffer brakeBuffer;
	SoundBuffer hornBuffer;

	unsigned int plr_id;
	double scaleX = 29335.0f / 1920.0f;
	double scaleY = 16504.0f / 1080.0f;
	Vector2f size;
	Clock SpawnDelay; //To keep track of time and Trigger spawn
	Time wait = seconds(30); //Add new cars after wait
	void initVar(); //Function to load initial data
	vector<Vector2f> findOrderedCurvePath(const Vector2f& startNode); //Function to find edges of the Curved path points

public:
	Cars(float worldWidth, float worldHeight); //Constructor for the class Cars
	void AddCars(); //Function to add new Cars data
	void Spawn(); //Function to spawn the cars onto the Map
	void drawCars(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y, Sprite& plr, double& spd); //Function to control the cars
};

#endif
