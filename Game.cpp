#include "Game.h"
#include "SignalControl.h"

void Game::initMinimal()
{
	window = nullptr;
	map = nullptr;
	desktopMode = VideoMode::getDesktopMode();
	window = new RenderWindow(this->desktopMode, "Escape Route", Style::Close | Style::Titlebar);
	window->setPosition(Vector2i(-15, 0));
	icon.loadFromFile("Icon.png");
	window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	disp.loadFromFile("Gravity.ttf");
	RENDER_WORLD_SCALE_X = 29335.f / 1920.f;
	RENDER_WORLD_SCALE_Y = 16504.f / 1080.f;
}

void Game::loadAssetsAndData()
{
	loadingEllipsisText.setFont(disp);
	loadingEllipsisText.setCharacterSize(40);
	loadingEllipsisText.setFillColor(Color::White);
	loadingEllipsisText.setPosition(50, desktopMode.height - 100);
	ellipsisCount = 0;

	int numFrames = 240;
	vector<Texture> localFrames; // Create a temporary, local vector
	localFrames.resize(numFrames);
	for (int i = 0; i < numFrames; ++i) {
		stringstream ss;
		ss << "Frames/frame_" << setw(4) << setfill('0') << (i + 1) << ".png";
		if (!localFrames[i].loadFromFile(ss.str())) {
			cout << "Error loading frame: " << ss.str() << endl;
		}
	}

	{
		lock_guard<mutex> lock(loadingMutex);
		loadingFrames = move(localFrames); // Efficiently move the data

		if (!loadingFrames.empty() && loadingFrames[0].getSize().x > 0) {
			currentFrameIndex = 0;
			loadingSprite.setTexture(loadingFrames[currentFrameIndex]);
			float scaleX = (float)desktopMode.width / loadingSprite.getLocalBounds().width;
			float scaleY = (float)desktopMode.height / loadingSprite.getLocalBounds().height;
			float finalScale = min(scaleX, scaleY);
			loadingSprite.setScale(finalScale, finalScale);
			loadingSprite.setOrigin(loadingSprite.getLocalBounds().width / 2.f, loadingSprite.getLocalBounds().height / 2.f);
			loadingSprite.setPosition(desktopMode.width / 2.f, desktopMode.height / 2.f);
		}
	}
	loadRoadGraph("roadGraph.cache");

	if (!endpoints.empty()) {
		Vector2f initialNodeData = endpoints[0];
		plr.setPosition(initialNodeData.x * RENDER_WORLD_SCALE_X, initialNodeData.y * RENDER_WORLD_SCALE_Y);
		plr.setRotation(0);
	}

	map = new TileMap("map_tiles", TILE_SIZE, MAP_WIDTH_TILES, MAP_HEIGHT_TILES);
	start = -1; end = -1; isAnimatingPath = false; pathAnimationIndex = 0; speed = 0.f;

	bg.loadFromFile("Routes_Map1.png");
	playerCar.loadFromFile("Car(P).png");
	ar.loadFromFile("Arrow.png");
	fadeImg.create(1920, 1080);

	rd.setTexture(bg); rd.setScale(1920.0f / 16384.0f, 1080.0f / 9218.0f);
	minimap.minimapBackgroundTexture = bg;
	minimap.minimapBackgroundSprite.setTexture(minimap.minimapBackgroundTexture);
	cancel.loadFromFile("Cancel.png");
	cancelbtn.setTexture(cancel);
	cancelbtn.setScale(0.05, 0.05);
	cancelbtn.setPosition(1920.f - cancelbtn.getGlobalBounds().width * 1.5f, cancelbtn.getGlobalBounds().height / 2.f);
	plr.setTexture(playerCar); plr.setScale(0.04, 0.04);
	FloatRect bounds = plr.getLocalBounds();
	plr.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
	RandomCars.Spawn();
	spawnDelay.restart();

	float x, y;
	ifstream startFile("Nodes.txt"); while (startFile >> x >> y) { endpoints.push_back({ x, y }); } startFile.close();

	fadeShape.setSize(Vector2f(static_cast<float>(desktopMode.width), static_cast<float>(desktopMode.height)));
	fadeShape.setFillColor(Color(0, 0, 0, 0));

	{
		lock_guard<mutex> lock(loadingMutex);
		isLoadingFinished = true;
	}
}

void Game::updateAndDrawLoadingAnimation()
{
	if (ellipsisClock.getElapsedTime() > timePerEllipsis) {
		ellipsisCount = (ellipsisCount + 1) % 4;
		string base = "Loading";
		for (int i = 0; i < ellipsisCount; ++i) {
			base += ".";
		}
		loadingEllipsisText.setString(base);
		ellipsisClock.restart();
	}
	window->clear(Color::Black);

	if (!loadingFrames.empty() && frameClock.getElapsedTime() > timePerFrame) {
		currentFrameIndex = (currentFrameIndex + 1) % loadingFrames.size();
		loadingSprite.setTexture(loadingFrames[currentFrameIndex], true);
		frameClock.restart();
	}

	if (loadingSprite.getTexture() != nullptr) {
		window->draw(loadingSprite);
	}

	window->draw(loadingEllipsisText);
	window->display();
}

//Private functions
void Game::reset()
{
	if (currentState == PLAYING) {
		currentState = RESTARTING_FADE_OUT;
		fadeClock.restart();
	}
}

bool Game::loadRoadGraph(const string& filename)
{
	ifstream inFile(filename);
	if (!inFile.is_open()) {
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

void Game::prompt()
{
	this->window->clear(Color::Green);
	this->window->draw(rd);
	signals.drawsignals(*window, 1, 1);
	signals.signalctrl(*window, RENDER_WORLD_SCALE_X, RENDER_WORLD_SCALE_Y);
	Text choose, st, en;
	choose.setString("Choose The Starting point ->");
	choose.setFont(disp);
	choose.setPosition(window->getSize().x / 2.f - 25 * (choose.getLetterSpacing() + choose.getString().getSize()) / 2, 100);
	choose.setOutlineColor(Color::Black);
	choose.setOutlineThickness(1.f);
	choose.setCharacterSize(40);
	st.setString(((start != -1) ? to_string(start) : myTextString));
	st.setFont(disp);
	st.setPosition(700, 200);
	st.setOutlineColor(Color::Black);
	st.setOutlineThickness(1.f);
	en.setString(((start == -1) ? "" : ((end == -1) ? myTextString : to_string(end))));
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
	fadeImg.update(*window);
	fadeDraw.setTexture(fadeImg);
}

void Game::playing()
{
	this->window->setView(view);
	if (this->map) {
		this->window->draw(*map);
	}
	this->window->draw(this->plr);
	RandomCars.drawCars(*window, RENDER_WORLD_SCALE_X, RENDER_WORLD_SCALE_Y, plr, speed);
	signals.drawsignals(*window, RENDER_WORLD_SCALE_X, RENDER_WORLD_SCALE_Y);
	signals.signalctrl(*window, RENDER_WORLD_SCALE_X, RENDER_WORLD_SCALE_Y);
	window->setView(minimap.minimapView);
	window->draw(minimap.minimapBackgroundSprite);
	window->draw(minimap.playerDotSprite);

	// Draw the minimap border (using the default view)
	window->setView(window->getDefaultView());
	window->draw(minimap.minimapBorder);
	if (expand) window->draw(cancelbtn);
	fadeImg.update(*window);
	fadeDraw.setTexture(fadeImg);
}

//Constructors & Destructors
Game::Game() : RandomCars(29335.f, 16504.f) {
	map = nullptr;
	window = nullptr;
	currentState = LOADING;
	initMinimal();

	isLoadingFinished = false;
	loadingThread = thread([this]() { this->loadAssetsAndData(); });

	while (window->isOpen() && !isLoadingFinished) {
		Event event;
		while (window->pollEvent(event)) {
			if (event.type == Event::Closed) {
				window->close();
			}
		}
		{
			lock_guard<mutex> lock(loadingMutex);
			if (isLoadingFinished) break;
		}
		updateAndDrawLoadingAnimation();
	}

	if (loadingThread.joinable()) {
		loadingThread.join();
	}
	fadeImg.update(*window);
	fadeDraw.setTexture(fadeImg);
	currentState = FADING_OUT;
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
			if (this->ev.key.code == Keyboard::E)
				cout << plr.getPosition().x / RENDER_WORLD_SCALE_X << " " << plr.getPosition().y / RENDER_WORLD_SCALE_Y << endl;
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
		case Event::MouseButtonPressed:
			Vector2i mousepos = Mouse::getPosition();
			FloatRect area(1920 * 0.75f, 1080 * 0.05f, 1920 * 0.2f,1080 * 0.2f);
			FloatRect close(cancelbtn.getPosition().x , cancelbtn.getPosition().y, cancelbtn.getGlobalBounds().width, cancelbtn.getGlobalBounds().height);
			if (area.contains(Vector2f(mousepos)) && !expand) {
				expand = true;
				minimap.expand();
			}
			else if (close.contains(Vector2f(mousepos))) {
				expand = false;
				minimap.reset();
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
				speed = 2.f;
			}
			pathAnimationIndex += 15;
			pathAnimTimer.restart();
		}

		if (pathAnimationIndex >= shortestPath.size()) {
			isAnimatingPath = false;
			currentState = FADING_OUT;
			fadeClock.restart();
			pathAnimTimer.restart();
		}
	}

	if (spawnDelay.getElapsedTime().asSeconds() >= 30) {
		RandomCars.AddCars();
		RandomCars.Spawn();
		spawnDelay.restart();
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
			if (state == 1) prompt();
			else {
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
				playing();
			}
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
			if (state == 1) currentState = PROMPT;
			else currentState = PLAYING;
		}
		break;
	}
	case PLAYING:
	{
		float dx, dy, ang;
		const double pi = 3.14159265358979323846;
		/*if (pathAnimTimer.getElapsedTime().asMilliseconds() >= 720) {
			plr.setRotation(pathArrowSprites[idx].getRotation());
			idx++;
			pathAnimTimer.restart();
		}*/
		ang = this->plr.getRotation();
		dx = speed * cosf(ang * pi / 180.f);
		dy = speed * sinf(ang * pi / 180.f);
		plr.move(dx, dy);
		if (Keyboard::isKeyPressed(Keyboard::Down)) {
			(this->speed > 0) ? this->speed -= 0.01 : this->speed = 0;
		}
		if (Keyboard::isKeyPressed(Keyboard::Up)) {
			if (this->speed < 10)
			this->speed += 0.0025;
			else this->speed = 10;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right)) {
			plr.rotate(speed / 9.5f);
		}
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			plr.rotate(-(speed / 9.5f));
		}
		double fact = speed;
		const Vector2f mapSize(29335.0f, 16504.0f);
		view.setSize(960 * (1 + fact), 540 * (1 + fact));
		Vector2f viewSize = view.getSize();

		Vector2f desiredCenter = plr.getPosition();

		Vector2f finalCenter = desiredCenter;

		float minX = viewSize.x / 2.0f + 2.f;
		float maxX = (mapSize.x - viewSize.x / 2.0f) - 2.f;
		finalCenter.x = max(minX, min(finalCenter.x, maxX));

		float minY = viewSize.y / 2.0f + 2.f;
		float maxY = (mapSize.y - viewSize.y / 2.0f) - 2.f;
		finalCenter.y = max(minY, min(finalCenter.y, maxY));

		view.setCenter(finalCenter);
		minimap.setcenter(plr.getPosition().x * (16384.f / 29335.f), plr.getPosition().y * (9218.f / 16504.f));

		//A red dot marking the position of the Player in the MiniMap
		minimap.playerDotSprite.setPosition(plr.getPosition().x * (16384.f / 29335.f), plr.getPosition().y * (9218.f / 16504.f));
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
			prompt();
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
			window->setView(window->getDefaultView());
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
		prompt();
		state = 4;
	}
	else if (currentState == FADING_OUT || currentState == RESTARTING_FADE_IN || currentState == RESTARTING_FADE_OUT || currentState == FADING_IN) {
		if (currentState == RESTARTING_FADE_OUT) playing();
		this->window->draw(fadeDraw);
		this->window->draw(fadeShape);
	}
	else if (currentState == PLAYING) {
		playing();
		state = 1;
	}
	this->window->display();
}
