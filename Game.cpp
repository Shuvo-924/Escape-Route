#include "Game.h"

//Private functions
void Game::reset()
{
	if (currentState == PLAYING) {
		currentState = RESTARTING_FADE_OUT;
		fadeClock.restart();
	}
}

void Game::initVar()
{
	currentState = PROMPT;
	this->window = nullptr;
	this->disp.loadFromFile("Gravity.ttf");
	this->start = -1;
	this->end = -1;
	this->bg.loadFromFile("Routes_Map1.png");
	this->rd.setTexture(bg);
	this->rd.setScale(0.1171875, 0.1171875);
	this->playerCar.loadFromFile("Car(P).png");
	this->plr.setTexture(playerCar);
	this->plr.setScale(0.04,0.04);
	ar.loadFromFile("Arrow.png");
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
	isAnimatingPath = false;
	pathAnimationIndex = 0;
	this->plr.setPosition(300, 398);
	view.setCenter(plr.getPosition());
	this->plr.setRotation(0);
	FloatRect bounds = this->plr.getGlobalBounds();
	this->plr.setOrigin(this->plr.getPosition().x + bounds.width / 2, this->plr.getPosition().y + bounds.height / 2);
}

void Game::initWin()
{
	this->desktopMode = VideoMode::getDesktopMode();
	this->window = new RenderWindow(this->desktopMode, "Escape Route", Style::Close | Style::Titlebar);
	this->map = new TileMap("map_tiles", TILE_SIZE, MAP_WIDTH_TILES, MAP_HEIGHT_TILES);
	fadeShape.setSize(Vector2f(this->desktopMode.width, this->desktopMode.height));
	fadeShape.setFillColor(Color(0, 0, 0, 0));
}

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
	delete this->map;
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
			if (currentState == PROMPT && ev.key.code == Keyboard::Enter) {
				if (start == -1 && !myTextString.empty()) {
					start = stoi(myTextString);
					myTextString.clear();
				}
				else if (end == -1 && !myTextString.empty()) {
					end = stoi(myTextString);
					myTextString.clear();
					if (start != -1 && end != -1) {
						findPath(start, end);
					}
				}
			}
			if (this->ev.key.code == Keyboard::R && currentState != PROMPT) {
				reset();
			}
			break;
		case Event::KeyReleased:
			rt = 0;
			break;
		case Event::TextEntered:
			if (currentState == PROMPT) {
				Uint32 unicodeChar = ev.text.unicode;

				if (unicodeChar >= 32 && unicodeChar <= 126 && myTextString.size() < 2)
				{
					if(stoi(myTextString + static_cast<char>(unicodeChar)) < 21)
					   myTextString += static_cast<char>(unicodeChar);
				}
				else if (unicodeChar == 8)
				{
					if (!myTextString.empty())
					{
						myTextString.erase(myTextString.size() - 1);
					}
				}
			}
		}
	}
}

void Game::update()
{
	this->pollEvents();
	if (isAnimatingPath) {
		if (pathAnimTimer.getElapsedTime() > pathAnimInterval && pathAnimationIndex < shortestPath.size()) {
			if (pathAnimationIndex < shortestPath.size() - 15) {
				Sprite newArrow;
				newArrow.setTexture(ar);
				newArrow.setScale(0.03, 0.03);
				newArrow.setOrigin(ar.getSize().x / 2.f, ar.getSize().y / 2.f);

				const Vector2f& currentPoint = shortestPath[pathAnimationIndex];
				const Vector2f& nextPoint = shortestPath[pathAnimationIndex + 15];

				newArrow.setPosition(currentPoint);
				Vector2f direction = nextPoint - currentPoint;
				float angle = atan2(direction.y, direction.x) * 180.f / pi;
				newArrow.setRotation(angle);

				pathArrowSprites.push_back(newArrow);
				plr.setPosition(pathArrowSprites[0].getPosition().x * RENDER_WORLD_SCALE_X, pathArrowSprites[0].getPosition().y * RENDER_WORLD_SCALE_Y);
				plr.setRotation(pathArrowSprites[0].getRotation());
				plr.setPosition(plr.getPosition().x - plr.getGlobalBounds().width * 10 * cosf(plr.getRotation() * pi / 180.f), plr.getPosition().y - plr.getGlobalBounds().width * 10 * sinf(plr.getRotation() * pi / 180.f));
				speed = 1.f;
			}
			pathAnimationIndex += 15;
			pathAnimTimer.restart();
		}

		if (pathAnimationIndex >= shortestPath.size()) {
			isAnimatingPath = false;
			currentState = FADING_OUT;
			fadeClock.restart();
		}
	}

	switch (currentState)
	{
	case PROMPT:
		break;

	case FADING_OUT:
	{
		// Calculate progress (0.0 to 1.0)
		float progress = fadeClock.getElapsedTime().asSeconds() / fadeDuration.asSeconds();
		if (progress > 1.0f) {
			progress = 1.0f;
		}

		// Fade from transparent (0) to opaque (255)
		Uint8 alpha = static_cast<Uint8>(progress * 255);
		fadeShape.setFillColor(Color(0, 0, 0, alpha));

		// When fully faded out...
		if (progress >= 1.0f) {
			// ...instantly position the camera on the player...
			const Vector2f mapSize(29335.0f, 16504.0f);
			view.setSize(1920, 1080);
			Vector2f viewSize = view.getSize();
			Vector2f desiredCenter = plr.getPosition();
			float minX = viewSize.x / 2.0f;
			float maxX = mapSize.x - viewSize.x / 2.0f;
			float minY = viewSize.y / 2.0f;
			float maxY = mapSize.y - viewSize.y / 2.0f;
			view.setCenter(
				clamp(desiredCenter.x, minX, maxX),
			    clamp(desiredCenter.y, minY, maxY)
			);
			currentState = FADING_IN;
			fadeClock.restart();
		}
		break;
	}

	case FADING_IN:
	{
		float progress = fadeClock.getElapsedTime().asSeconds() / fadeDuration.asSeconds();
		if (progress > 1.f) {
			progress = 1.f;
		}

		Uint8 alpha = static_cast<Uint8>(255 - (progress * 255));
		fadeShape.setFillColor(Color(0, 0, 0, alpha));

		if (progress >= 1.f) {
			currentState = PLAYING;
		}
		break;
	}
	case PLAYING:
	{
		float dx, dy, ang;
		const double pi = 3.14159265358979323846;
		ang = this->plr.getRotation();
		dx = speed * cosf(ang * pi / 180.f);
		dy = speed * sinf(ang * pi / 180.f);
		plr.move(dx, dy);
		if (Keyboard::isKeyPressed(Keyboard::Down)) {
			(this->speed > 0) ? this->speed -= 0.008 : this->speed = 0;
		}
		if (Keyboard::isKeyPressed(Keyboard::Up)) {
			if (this->speed < 2.5)
			this->speed += 0.0025;
			else this->speed = 2.5;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right)) {
			if (rt < 0.3) {
				rt += 0.0012;
			}
			plr.rotate(rt);
		}
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			if (rt < 0.3) {
				rt += 0.0012;
			}
			plr.rotate(-rt);
		}
		double fact = speed;
		const Vector2f mapSize(29335.0f, 16504.0f);
		view.setSize(960 * (1 + fact), 540 * (1 + fact));
		Vector2f viewSize = view.getSize();

		Vector2f desiredCenter = plr.getPosition();

		Vector2f finalCenter = desiredCenter;

		float minX = viewSize.x / 2.0f;
		float maxX = mapSize.x - viewSize.x / 2.0f;
		finalCenter.x = max(minX, min(finalCenter.x, maxX));

		float minY = viewSize.y / 2.0f;
		float maxY = mapSize.y - viewSize.y / 2.0f;
		finalCenter.y = max(minY, min(finalCenter.y, maxY));

		view.setCenter(finalCenter);

		//A red dot marking the position of the Player in the MiniMap
		mark.setPosition(plr.getPosition().x + (plr.getGlobalBounds().width / 2) * sinf(plr.getRotation() * pi / 180.f), plr.getPosition().y - (plr.getGlobalBounds().height / 2) * cosf(plr.getRotation() * pi / 180.f));
		mark.setRotation(plr.getRotation());
		break;
	}
	case RESTARTING_FADE_OUT:
	{
		float progress = fadeClock.getElapsedTime().asSeconds() / fadeDuration.asSeconds();
		if (progress > 1.0f) {
			progress = 1.0f;
		}

		// Fade from transparent (0) to opaque (255)
		Uint8 alpha = static_cast<Uint8>(progress * 255);
		fadeShape.setFillColor(Color(0, 0, 0, alpha));

		// When fully black...
		if (progress >= 1.0f) {
			currentState = RESTARTING_FADE_IN;
			start = -1;
			end = -1;
			shortestPath.clear();
			pathArrowSprites.clear();
			myTextString.clear();
			isAnimatingPath = false;
			pathAnimationIndex = 0;
			speed = 0.f;
			fadeClock.restart();
		}
		break;
	}
	case RESTARTING_FADE_IN:
	{
		float progress = fadeClock.getElapsedTime().asSeconds() / fadeDuration.asSeconds();
		if (progress > 1.0f) {
			progress = 1.0f;
		}

		Uint8 alpha = static_cast<Uint8>(255 - (progress * 255));
		fadeShape.setFillColor(Color(0, 0, 0, alpha));

		if (progress >= 1.0f) {
			currentState = PROMPT;
		}
		break;
	}
    }
}

void Game::findPath(int x, int y)
{
	Vector2f startNode = endpoints[x];
	Vector2f endNode = endpoints[y];

	this->shortestPath = findShortestPath(startNode, endNode);
	if (!shortestPath.empty()) {
		isAnimatingPath = true;       
		pathAnimationIndex = 0;       
		pathArrowSprites.clear();
		pathAnimTimer.restart();    
	}
	else {
		isAnimatingPath = false;    
	}
}

vector<Vector2f> Game::findShortestPath(const Vector2f& start, const Vector2f& end)
{
	if (roadGraph.find(start) == roadGraph.end() || roadGraph.find(end) == roadGraph.end()) {
		cout << "Start or end node not in graph." << endl;
		return {};
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
	if (currentState == PROMPT) {
		this->window->draw(rd);
		Text choose, st, en;
		choose.setString("Choose The Starting point ->");
		choose.setFont(disp);
		choose.setPosition(window->getSize().x / 2 - 25 * (choose.getLetterSpacing() + choose.getString().getSize()) / 2, 100);
		choose.setOutlineColor(Color::Black);
		choose.setOutlineThickness(1.f);
		choose.setCharacterSize(40);
		st.setString(to_string(start));
		st.setFont(disp);
		st.setPosition(700, 200);
		st.setOutlineColor(Color::Black);
		st.setOutlineThickness(1.f);
		en.setString(to_string(end));
		en.setFont(disp);
		en.setPosition(1000, 200);
		en.setOutlineColor(Color::Black);
		en.setOutlineThickness(1.f);
		this->window->draw(choose);
		this->window->draw(st);
		this->window->draw(en);
		if (isAnimatingPath) {
			for (const auto& arrowSprite : pathArrowSprites) {
				this->window->draw(arrowSprite);
			}
		}
	}
	else if (currentState == FADING_OUT || currentState == RESTARTING_FADE_IN) {
		this->window->draw(rd);
		Text choose, st, en;
		choose.setString("Choose The Starting point ->");
		choose.setFont(disp);
		choose.setPosition(window->getSize().x / 2 - 30 * (choose.getLetterSpacing() + choose.getString().getSize()) / 2, 100);
		choose.setOutlineColor(Color::Black);
		choose.setOutlineThickness(1.f);
		choose.setCharacterSize(40);
		st.setString(to_string(start));
		st.setFont(disp);
		st.setPosition(700, 200);
		st.setOutlineColor(Color::Black);
		st.setOutlineThickness(1.f);
		en.setString(to_string(end));
		en.setFont(disp);
		en.setPosition(1000, 200);
		en.setOutlineColor(Color::Black);
		en.setOutlineThickness(1.f);
		this->window->setView(this->window->getDefaultView());
		this->window->draw(choose);
		this->window->draw(st);
		this->window->draw(en);
		this->window->draw(fadeShape);
	}
	else if (currentState == RESTARTING_FADE_OUT || currentState == PLAYING || currentState == FADING_IN) {
		this->window->setView(view);
		if (this->map) {
			this->window->draw(*map);
		}
		this->window->draw(this->plr);
		if (currentState == FADING_IN || currentState == RESTARTING_FADE_OUT) {
			this->window->draw(fadeShape);
		}
	}
	this->window->display();
}
