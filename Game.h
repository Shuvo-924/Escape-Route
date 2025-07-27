//Uncomment line 268 in Game.cpp to check all paths from one node to another

#ifndef GAME_H
#define GAME_H

#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Window.hpp>
#include<vector>
#include<set>
#include<iostream>
#include<algorithm>
#include<ctime>
#include<fstream>
#include<sstream>
#include<queue>
#include<unordered_map>
#include<unordered_set>

using namespace sf;
using namespace std;

struct Vector2fHash { //Custom Hash function to use Vector2f data type in map and set
	size_t operator()(const Vector2f& v) const {
		return hash<float>()(v.x) ^ (hash<float>()(v.y) << 1);
	}
};

class Game
{

private:
	RenderWindow* window;//For Game Window
	View view;//For Viewing a certain portion of the Window
	Event ev;//For Event handling such as Keypress,Cursor,Mouse-clicks etc.
	VideoMode desktopMode;//For Window size

	Texture playerCar,policeCar, bg, dot;//Image data for Objects to use on screen
	Sprite plc, plr, rd, mark;//Objects
	float speed = 0, rt = 0;
	const double pi = 3.14159265358979323846;
	float zoom = 1.f;
	int cur = 0, var = 0;

	unordered_set<Vector2f, Vector2fHash> bluePath; //Set of (x,y) coordinates for drawing a Path (For regular roads and overpasses)
	unordered_set<Vector2f, Vector2fHash> greenPath; // For underpasses
	unordered_map<Vector2f, vector<Vector2f>, Vector2fHash> roadGraph; //Edges for each coordinate
	unordered_set<Vector2f, Vector2fHash> visited; //To check visited nodes
	vector<Vector2f> endpoints; //Starting position or Destination points
	vector<Vector2f> shortestPath; //To store the Shortest Path data

	void initVar();//Function to set Initial values of Variables
	void initWin();//Function to set Initial Window
	/*void buildComplexRoadGraph();
	void addBridgeEdges();
	void saveRoadGraph(const string& filename);*/
	bool loadRoadGraph(const string& filename); //Load Graph data from file instead of Calculating

public:
	Game();
	virtual ~Game();

	const bool getWinOpen() const;

	void pollEvents();
	void update();
	void findPath(int x, int y);
	vector<Vector2f> findShortestPath(const Vector2f& start, const Vector2f& end);
	void render();
};

#endif
