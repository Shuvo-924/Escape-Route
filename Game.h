#ifndef GAME_H
#define GAME_H

#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Window.hpp>
#include<SFML/OpenGL.hpp>
#include<memory>
#include<vector>
#include<string>
#include<iomanip>
#include<thread>
#include<mutex>
#include<set>
#include<queue>
#include<iostream>
#include<algorithm>
#include<ctime>
#include<fstream>
#include<sstream>
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
	Minimap minimap; //Minimap at the Top-Corner
	bool expand = false; //Bool to expand or collapse the Minimap
	Texture cancel; //Close button texture for Collapsing the Expanded Minimap
	Sprite cancelbtn; //Close button
	unique_ptr<Cars> RandomCars; //Random cars in the RoadMap
	unique_ptr<Signal> signals; //Signals Drawing and Controlling class
	Clock spawnDelay; //Clock to track time for Spawning cars
	int state = 1; //To control Fading from and into Prompt and Playing state

	vector<Texture> creditesImg; //To store images for Credits screen before Loading Screen
	vector<Sprite> credits; //To store Drawable objects for Credits screen before Loading Screen
	vector<Texture> loadingFrames; //To hold all the video frame textures
	Sprite loadingSprite; //The sprite to display the current frame
	unsigned int currentFrameIndex, creditsIndex = 0; //Index counter for next frame to Display
	Clock frameClock; //Times the switch between frames
	const Time timePerFrame = seconds(1.0f / 30.0f); //For a 30 FPS video
	int ellipsisCount; //To count the "..." of "Loading..." and restart from 1
	thread loadingThread; //For multiple series of operations i.e. Loading the assets in Background and Displaying Loading screen
	mutex loadingMutex; //Protects shared data
	bool isLoadingFinished = false,display = false; //Bool to trigger and terminate Loading Screen

	Text loadingEllipsisText; //The "Loading..." text
	Clock ellipsisClock; //Times the change of the ellipsis
	const Time timePerEllipsis = seconds(0.5f); //How long it takes for another dot to appear 1->. , 2->.. , 3->... , 4->. ,........

	Texture playerCar,policeCar, bg, cityimg, dot, ar, fadeImg; //Image data for Objects to use on screen
	Sprite plc, plr, rd, city, mark, arw, fadeDraw; //Objects
	double speed = 0; //PlayerCar speed
	const double pi = 3.14159265358979323846;

	SoundBuffer playerEngineBuffer; //To store the Audio of Engine sound
	SoundBuffer playerBrakeBuffer; //To store the Audio of Brake sound
	Sound playerEngineSound; //To control the Engine sound i.e. volume,playstate,loop etc.
	Sound playerBrakeSound; //To control the Brake sound
	
	unordered_set<Vector2f, Vector2fHash> borderArea; //Coordinates of Borders of the Road
	unordered_map<Vector2f, vector<Vector2f>, Vector2fHash> roadGraph; //Edges for each coordinate
	unordered_set<Vector2f, Vector2fHash> visited; //To check visited nodes
	vector<Vector2f> endpoints; //Starting position or Destination points
	vector<Vector2f> shortestPath; //To store the Shortest Path data
	const int TILE_SIZE = 512; //Size of each sliced image of the Large 29335x16504 Map
	const int MAP_WIDTH_TILES = 58; //Number of Columns
	const int MAP_HEIGHT_TILES = 33; //Number of Rows
	//To convert screen Coordinates to High resolution Image Coordinates
    double RENDER_WORLD_SCALE_X;
	double RENDER_WORLD_SCALE_Y;

	GameState currentState; //Current Gamestate
	RectangleShape fadeShape; //For Transitioning into and from Black screen
	Clock fadeClock; //To keep track of time and trigger Transition 
	const Time fadeDuration = seconds(2.f); //Duration of fade

	Font disp; //Text font used
	int start, end; //Starting and Ending Node
	string myTextString; //String to display as the Start and End points

	vector<Sprite> pathArrowSprites; //Stores the visible arrow sprites
	Clock pathAnimTimer; //Times the interval between arrows
	Time pathAnimInterval = seconds(0.012f); //The delay between each arrow displayed
	size_t pathAnimationIndex; //Tracks which point in the path we're animating next
	bool isAnimatingPath; //Flag to control if the animation is running

	void initMinimal(); //Function to load Essential elements for Initial screen
	void loadAssetsAndData(); //Function to load all the necessary elements
	void updateAndDrawLoadingAnimation(); //Function to draw Loading screen
	void reset(); //Function to restart the Game
	/*void buildComplexRoadGraph();
	void saveRoadGraph(const string& filename);*/
	bool loadRoadGraph(const string& filename); //Load Graph data from file instead of Calculating
	void prompt(); //For prompt screen to Input Start and End
	void playing(); //For playing screen

public:
	Game(); //Game class Constructor which is called when a (Game x) object is created
	virtual ~Game(); //Game class Destructor when program is closed

	const bool getWinOpen() const; //Bool to check if Window is currently open
	void pollEvents(); //Function to check for Events such as Keypress,Mouse movement etc.
	void update(); //Function to update the program in the background
	void findPath(int x, int y); //Function to call findShortestPath() with starting point and destination as Arguments
	vector<Vector2f> findShortestPath(const Vector2f& start, const Vector2f& end); //Function to find the Shortest Path
	void render(); //Function to Draw the objects to show on screen after Updating
};

#endif
