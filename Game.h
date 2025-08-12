#ifndef GAME_H
#define GAME_H

#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Window.hpp>
#include<SFML/OpenGL.hpp>
#include<vector>
#include<string>
#include<iomanip>
#include<thread>
#include<mutex>
#include<set>
#include<iostream>
#include<list>
#include<algorithm>
#include<ctime>
#include<fstream>
#include<sstream>
#include<queue>
#include<unordered_map>
#include<unordered_set>
#include "CustomFunctions.h"
#include "TileMap.h"
#include "SpawnCars.h"
#include "SignalControl.h"
#include "MiniMap.h"

using namespace sf;
using namespace std;

enum GameState {
	LOADING,
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
	Image icon; //Game icon
	TileMap* map; //Map visible around the Player Car
	Minimap minimap;
	bool expand = false;
	Texture cancel;
	Sprite cancelbtn;
	Cars RandomCars; //Random cars in the RoadMap
	Clock spawnDelay;
	Signal signals;
	int state = 1;

	vector<Texture> loadingFrames; // Will hold all the video frame textures
	Sprite loadingSprite;               // The sprite to display the current frame
	unsigned int currentFrameIndex;
	Clock frameClock;                   // Times the switch between frames
	const Time timePerFrame = seconds(1.0f / 30.0f); // For a 30 FPS video
	int ellipsisCount;
	thread loadingThread;
	mutex loadingMutex; // Protects shared data
	bool isLoadingFinished = false;

	Text loadingEllipsisText;           // The "Loading..." text
	Clock ellipsisClock;                // Times the change of the ellipsis
	const Time timePerEllipsis = seconds(0.5f);

	Texture playerCar,policeCar, bg, dot, ar, fadeImg;//Image data for Objects to use on screen
	Sprite plc, plr, rd, mark, arw, fadeDraw;//Objects
	double speed = 0;
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
	//To convert screen Coordinates to High resolution Image Coordinates
    double RENDER_WORLD_SCALE_X;
	double RENDER_WORLD_SCALE_Y;

	GameState currentState; //Current Gamestate
	RectangleShape fadeShape; //For Transitioning into and from Black screen
	Clock fadeClock; //To keep track of time and trigger Transition 
	const Time fadeDuration = seconds(2.f); //Duration of fade

	Font disp;
	int start, end;
	string myTextString;

	vector<Sprite> pathArrowSprites; // Stores the visible arrow sprites.
	Clock pathAnimTimer; // Times the interval between arrows.
	Time pathAnimInterval = seconds(0.012f); // The delay between each arrow. Adjust for speed.
	size_t pathAnimationIndex; // Tracks which point in the path we're animating next.
	bool isAnimatingPath; // Flag to control if the animation is running.

	void initMinimal();
	void loadAssetsAndData();
	void updateAndDrawLoadingAnimation();
	void reset(); //Function to restart the Game
	/*void buildComplexRoadGraph();
	void addBridgeEdges();
	void saveRoadGraph(const string& filename);*/
	bool loadRoadGraph(const string& filename); //Load Graph data from file instead of Calculating
	void prompt();
	void playing();

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
