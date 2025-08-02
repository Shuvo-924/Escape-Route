#ifndef GAME_H
#define GAME_H

#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Window.hpp>
#include<SFML/OpenGL.hpp>
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
#include "TileMap.h"

using namespace sf;
using namespace std;

struct Vector2fHash { //Custom Hash function to use Vector2f data type in map and set
	size_t operator()(const Vector2f& v) const {
		return hash<float>()(v.x) ^ (hash<float>()(v.y) << 1);
	}
};

enum GameState {
	PROMPT,
	FADING_OUT,
	FADING_IN,
	PLAYING,
	RESTARTING_FADE_OUT,
	RESTARTING_FADE_IN
};

class Game
{

private:
	RenderWindow* window; //For Game Window
	View view; //For Viewing a certain portion of the Window
	Event ev; //For Event handling such as Keypress,Cursor,Mouse-clicks etc.
	VideoMode desktopMode; //For Window size
	TileMap* map; //Map visible around the Player Car

	//To convert screen Coordinates to High resolution Image Coordinates
	const float RENDER_WORLD_SCALE_X = 29335.0f / 1920.0f;
	const float RENDER_WORLD_SCALE_Y = 16504.0f / 1080.0f;

	Texture playerCar,policeCar, bg, dot, ar;//Image data for Objects to use on screen
	Sprite plc, plr, rd, mark, arw;//Objects
	double speed = 0, rt = 0;
	const double pi = 3.14159265358979323846;

	unordered_set<Vector2f, Vector2fHash> bluePath; //Set of (x,y) coordinates for drawing a Path (For regular roads and overpasses)
	unordered_set<Vector2f, Vector2fHash> greenPath; // For underpasses
	unordered_map<Vector2f, vector<Vector2f>, Vector2fHash> roadGraph; //Edges for each coordinate
	unordered_set<Vector2f, Vector2fHash> visited; //To check visited nodes
	vector<Vector2f> endpoints; //Starting position or Destination points
	vector<Vector2f> shortestPath; //To store the Shortest Path data
	const int TILE_SIZE = 512;
	const int MAP_WIDTH_TILES = 58;
	const int MAP_HEIGHT_TILES = 33;

	GameState currentState;
	RectangleShape fadeShape; //For Transitioning into and from Black screen
	Clock fadeClock; //To keep track of time and trigger Transition 
	const Time fadeDuration = seconds(1.25f); //Duration of fade

	Font disp;
	int start, end;
	string myTextString;

	vector<Sprite> pathArrowSprites; // Stores the visible arrow sprites.
	Clock pathAnimTimer; // Times the interval between arrows.
	Time pathAnimInterval = seconds(0.012f); // The delay between each arrow. Adjust for speed.
	size_t pathAnimationIndex; // Tracks which point in the path we're animating next.
	bool isAnimatingPath; // Flag to control if the animation is running.

	void reset(); //Function to restart the Game
	void initVar(); //Function to set Initial values of Variables
	void initWin(); //Function to set Initial Window
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
