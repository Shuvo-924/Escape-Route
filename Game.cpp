//Uncomment line 264 to check all paths from one node to another



#include "Game.h"

//Private functions
void Game::initVar()
{
	this->window = nullptr;
	this->playerCar.loadFromFile("Car(P).png");
	this->bg.loadFromFile("Routes_Map1.png");
	this->rd.setTexture(bg);
	this->rd.setScale(0.1171875, 0.1171620742);
	this->plr.setTexture(playerCar);
	this->plr.setScale(0.0045,0.0045);
	ifstream rasta("path.txt");
	float x, y;
	while (rasta >> x >> y) {
		bluePath.insert({ x, y });
	}
	rasta.close();
	rasta.open("Under.txt");
	while (rasta >> x >> y) {
		greenPath.insert({ x, y });
	}
	rasta.close();
	ifstream start("Nodes.txt");
	while (start >> x >> y) {
		endpoints.push_back({ x, y });
	}
	start.close();
	const string graphCacheFile = "roadGraph.cache";
	loadRoadGraph(graphCacheFile);
	/*this->plr.setPosition(0, 398);
	this->plr.setRotation(0);
	FloatRect bounds = this->plr.getGlobalBounds();
	this->plr.setOrigin(this->plr.getPosition().x + bounds.width / 2, this->plr.getPosition().y + bounds.height / 2);*/
}

void Game::initWin()
{
	this->desktopMode = VideoMode::getDesktopMode();
	this->window = new RenderWindow(this->desktopMode, "Escape Route", Style::Close | Style::Titlebar);
}

//void Game::buildComplexRoadGraph()
//{
//	roadGraph.clear();
//	const int searchRadius = 2;
//	for (const auto& coord : bluePath) {
//		for (int dy = -searchRadius; dy <= searchRadius; ++dy) {
//			for (int dx = -searchRadius; dx <= searchRadius; ++dx) {
//				if (dx == 0 && dy == 0) continue;
//				Vector2f neighbor = coord + Vector2f(dx, dy);
//				if (bluePath.count(neighbor)) {
//					roadGraph[coord].push_back(neighbor);
//				}
//			}
//		}
//	}
//
//	// Connect all green path segments
//	for (const auto& coord : greenPath) {
//		for (int dy = -searchRadius; dy <= searchRadius; ++dy) {
//			for (int dx = -searchRadius; dx <= searchRadius; ++dx) {
//				if (dx == 0 && dy == 0) continue;
//				Vector2f neighbor = coord + Vector2f(dx, dy);
//				if (greenPath.count(neighbor)) {
//					roadGraph[coord].push_back(neighbor);
//				}
//			}
//		}
//	}
//	addBridgeEdges();
//}
//
//void Game::addBridgeEdges()
//{
//	const int maxBridgeLength = 40;
//	const vector<Vector2f> checkDirections = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
//
//	for (const auto& coord : bluePath) {
//		// For each blue pixel, check if it's next to a green pixel
//		for (const auto& dir : checkDirections) {
//			Vector2f adjacent = coord + dir;
//
//			if (greenPath.count(adjacent)) {
//				// This blue pixel is a "bridge abutment".
//				// Now we need to scan across the green path.
//				// The scan direction is the same as the direction to the first green pixel.
//				Vector2f currentPos = coord;
//				for (int i = 0; i < maxBridgeLength; ++i) {
//					currentPos += dir; // Move one step across the bridge
//
//					// If we find another blue pixel, we've found the other side!
//					if (bluePath.count(currentPos)) {
//						Vector2f otherSide = currentPos;
//						// Add a mutual edge and stop this scan
//						roadGraph[coord].push_back(otherSide);
//						roadGraph[otherSide].push_back(coord);
//						break;
//					}
//
//					// If we hit a non-green pixel before finding a blue one, it's not a bridge
//					if (!greenPath.count(currentPos)) {
//						break;
//					}
//				}
//			}
//		}
//	}
//}
//
//void Game::saveRoadGraph(const string& filename)
//{
//	ofstream outFile(filename);
//	cout << "Saving road graph to cache file: " << filename << "..." << endl;
//
//	// The file format will be:
//	// NodeX NodeY : Neighbor1X Neighbor1Y Neighbor2X Neighbor2Y ...
//	for (const auto& [node, neighbors] : roadGraph) {
//		outFile << node.x << " " << node.y << " :";
//		for (const auto& neighbor : neighbors) {
//			outFile << " " << neighbor.x << " " << neighbor.y;
//		}
//		outFile << "\n";
//	}
//	outFile.close();
//	cout << "Graph saved successfully." << endl;
//}

bool Game::loadRoadGraph(const string& filename)
{
	ifstream inFile(filename);
	if (!inFile.is_open()) {
		// This is not an error, it just means the cache doesn't exist yet.
		return false;
	}

	cout << "Loading road graph from cache file: " << filename << "..." << endl;
	roadGraph.clear(); // Ensure we start with a fresh graph

	string line;
	while (getline(inFile, line)) {
		if (line.empty()) continue;
		stringstream ss(line);
		Vector2f sourceNode;
		char colon;
		ss >> sourceNode.x >> sourceNode.y >> colon;

		if (colon != ':') {
			cerr << "Warning: Malformed line in graph cache. Skipping." << endl;
			continue;
		}

		vector<Vector2f> neighbors;
		Vector2f neighborNode;
		while (ss >> neighborNode.x >> neighborNode.y) {
			neighbors.push_back(neighborNode);
		}
		roadGraph[sourceNode] = neighbors;
	}
	inFile.close();
	cout << "Graph loaded successfully from cache." << endl;
	return !roadGraph.empty();
}

//Constructors & Destructors
Game::Game() {
	this->initVar();
	this->initWin();
}

Game::~Game() {
	delete this->window;
}

const bool Game::getWinOpen() const
{
	return this->window->isOpen();
}

void Game::pollEvents()
{
	while (this->window->pollEvent(this->ev)) {
		switch (ev.type)
		{
		case Event::Closed:
			this->window->close();
			break;

		case Event::KeyPressed:
			if (this->ev.key.code == Keyboard::Escape)
				this->window->close();
			break;
		/*case Event::MouseWheelScrolled:
			if (ev.mouseWheelScroll.delta > 0 && zoom > 0) {
				this->view.zoom(0.9f);
				this->zoom *= 0.9f;
			}
			else if (ev.mouseWheelScroll.delta < 0 && zoom < 2) {
				this->view.zoom(1.1f);
				this->zoom *= 1.1f;
			}
			this->window->setView(view);
			break;
		case Event::MouseButtonPressed:
			if (ev.mouseButton.button == sf::Mouse::Left) {
				this->dragging = true;
				this->mousePrevPos = sf::Mouse::getPosition(*this->window);
			}
			break;

		case Event::MouseButtonReleased:
			if (ev.mouseButton.button == sf::Mouse::Left) {
				this->dragging = false;
			}
			break;*/
		}
	}
}


void Game::update()
{
	this->pollEvents();
	/*if (dragging) {
		sf::Vector2i mouseCurrPos = sf::Mouse::getPosition(*this->window);
		sf::Vector2f worldPrev = this->window->mapPixelToCoords(mousePrevPos);
		sf::Vector2f worldCurr = this->window->mapPixelToCoords(mouseCurrPos);

		sf::Vector2f delta = worldPrev - worldCurr;
		this->view.move(delta);
		this->window->setView(this->view);

		this->mousePrevPos = mouseCurrPos;
	}*/
	/*if(cur < path.size()) {
		plr.setPosition(path[cur]);
		plr.setRotation(rotation[cur]);
		cur++;
	}*/
	float dx, dy, ang;
	const double pi = 3.14159265358979323846;
	ang = this->plr.getRotation();
	dx = speed * cosf(ang * pi / 180.f);
	dy = speed * sinf(ang * pi / 180.f);
	plr.move(dx, dy);
	if (Keyboard::isKeyPressed(Keyboard::Down)) {
		(this->speed > 0) ? this->speed -= 0.001 : this->speed = 0;
	}
	if (Keyboard::isKeyPressed(Keyboard::Up)) {
		this->speed = 0.1;
	}
	if (Keyboard::isKeyPressed(Keyboard::Right)) {
		plr.rotate(0.0862);
	}
	if (Keyboard::isKeyPressed(Keyboard::Left)) {
		plr.rotate(-0.0862);
	}
	
	view.setCenter(plr.getPosition());//Setting the view around the moving Car
	view.setSize(320, 180);
	//A red dot marking the position of the Player in the MiniMap
	mark.setPosition(plr.getPosition().x + (plr.getGlobalBounds().width / 2) * sinf(plr.getRotation() * pi / 180.f), plr.getPosition().y - (plr.getGlobalBounds().height / 2) * cosf(plr.getRotation() * pi / 180.f));
	mark.setRotation(plr.getRotation());
	/*for (int i = 0;i < 21;i++) { //To check all the paths 
		for (int j = 0;j < 21;j++) {
			if (i == j) continue;
			sleep(milliseconds(800));
			findPath(i, j);
			render();
		}
	}*/
}

void Game::findPath(int x, int y)
{
	Vector2f startNode = endpoints[x];
	Vector2f endNode = endpoints[y];

	this->shortestPath = findShortestPath(startNode, endNode);
}

vector<Vector2f> Game::findShortestPath(const Vector2f& start, const Vector2f& end)
{
	if (roadGraph.find(start) == roadGraph.end() || roadGraph.find(end) == roadGraph.end()) {
		cout << "Start or end node not in graph." << endl;
		return {}; // Return an empty path
	}

	queue<Vector2f> q;
	unordered_map<Vector2f, Vector2f, Vector2fHash> parent;
	unordered_set<Vector2f, Vector2fHash> visited;

	q.push(start);
	visited.insert(start);

	bool found = false;
	while (!q.empty()) {
		Vector2f current = q.front();
		q.pop();

		if (current == end) {
			found = true;
			break;
		}
		for (const auto& neighbor : roadGraph[current]) {
			if (visited.find(neighbor) == visited.end()) {
				visited.insert(neighbor);
				parent[neighbor] = current;
				q.push(neighbor);
			}
		}
	}
	vector<Vector2f> path;
	if (found) {
		Vector2f current = end;
		while (current != start) {
			path.push_back(current);
			current = parent[current];
		}
		path.push_back(start);
		reverse(path.begin(), path.end());
	}
	else {
		cout << "No path found." << endl;
	}
	return path;
}

void Game::render()
{
	this->window->clear(Color::Green);
	//this->window->setView(view);
	this->window->draw(rd);
	if (shortestPath.size() > 1) {
		for (size_t i = 0; i < shortestPath.size() - 1; ++i) {
			Vertex line[] = {
				Vertex(shortestPath[i], Color::Yellow),
				Vertex(shortestPath[i + 1], Color::Yellow)
			};
			window->draw(line, 2, sf::Lines);
		}
	}
	this->window->display();
}
