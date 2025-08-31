#include "SpawnCars.h"
#include "SpeedControl.h"

static unsigned int getNextCarId() {
	static unsigned int nextId = 0;
	return nextId++;
}

void Cars::initVar()
{
	vector<String> loc = { {"Car(G).png"}, {"Car(P).png"}, {"Car(R).png"}};
	for (int i = 0;i < 3;i++) {
		Texture x;
		x.loadFromFile(loc[i]);
		skins.push_back(x);
	}
	ifstream deploy("RandNodes.txt");
	float x, y;
	double z;
	while (deploy >> x >> y >> z) {
		points.push_back({{x,y},(float)z});
	}
	size = { 96, 53.32};
	int n = 15;
	engineRunningBuffer.loadFromFile("Car_Running.ogg");
	brakeBuffer.loadFromFile("Car_Brake.ogg");
	hornBuffer.loadFromFile("Car_Horn.ogg");
	while (n--) AddCars();
	SpawnDelay.restart();
	plr_id = getNextCarId();
	ifstream curveStartsFile("ChangeDir.txt");
	while (curveStartsFile >> x >> y) {
		curveStartPoints.insert({ x, y });
	}
	curveStartsFile.close();
	ifstream allCurvesFile("Curves.txt");
	while (allCurvesFile >> x >> y) {
		allCurvePoints.insert({ x, y });
	}
	allCurvesFile.close();
}

vector<Vector2f> Cars::findOrderedCurvePath(const Vector2f& startNode)
{
	vector<Vector2f> orderedPath;
	if (allCurvePoints.find(startNode) == allCurvePoints.end()) {
		return orderedPath;
	}
	orderedPath.push_back(startNode);
	unordered_set<Vector2f, Vector2fHash> visitedInThisSearch;
	queue<Vector2f> search;
	search.push(startNode);
	while(!search.empty()) {
		Vector2f parent = search.front();
		visitedInThisSearch.insert(parent);
		vector<Vector2f> dir = { {1,0},{0,1},{-1,0},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1} };
		for (int i = 0;i < 8;i++) {
			Vector2f neighbor = parent + dir[i];
			if (visitedInThisSearch.count(neighbor)) continue;
			if (allCurvePoints.count(neighbor)) {
				orderedPath.push_back(neighbor);
				search.push(neighbor);
			}
		}
		search.pop();
	}
	return orderedPath;
}

void Cars::AddCars()
{
	int n = rand() % 5; 
	while (n--) {
		Texture& tex = skins[rand() % 3];
		Car newCar;
		newCar.id = getNextCarId();
		newCar.body.setTexture(tex);

		newCar.body.setScale(size.x / tex.getSize().x, size.y / tex.getSize().y);
		double spd = 1.5f;
		newCar.speed = spd;
		newCar.lastspeed = spd;

		pair<Vector2f, float> strt = points[rand() % 21];

		const double pi = 3.14159265358979323846;
		newCar.body.setPosition(Vector2f(strt.first.x * scaleX, strt.first.y * scaleY) - Vector2f(newCar.body.getGlobalBounds().width * cosf(strt.second * pi / 180.f), newCar.body.getGlobalBounds().width * sinf(strt.second * pi / 180.f)));
		newCar.body.setRotation(strt.second);
		FloatRect bounds = newCar.body.getGlobalBounds();
		float width = bounds.width;
		newCar.body.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
		
		int f = bodies.size();
		while (f--) {
			bool found = false;
			for (int i = 0;i < bodies.size();i++) {
				if (bounds.intersects(bodies[i].body.getGlobalBounds())) {
					newCar.body.move(-(width + 10.f) * cosf(strt.second * pi / 180.f), -(width + 10.f) * sinf(strt.second * pi / 180.f));
					found = true;
				}
			}
			if (!found) break;
		}
		
		newCar.engineSound.setBuffer(engineRunningBuffer);
		newCar.engineSound.setLoop(true);
		newCar.engineSound.setVolume(25.f);

		newCar.hornSound.setBuffer(hornBuffer);
		newCar.hornSound.setLoop(true);
		newCar.hornSound.setVolume(80.f);

		newCar.brakeSound.setBuffer(brakeBuffer);
		newCar.brakeSound.setVolume(40.f);
		newCar.honk.restart();

		bodies.push_back(newCar);
	}
}

Cars::Cars(float worldWidth, float worldHeight) : grid(worldWidth, worldHeight, 8, 8)
{
	initVar();
}

void Cars::Spawn()
{
	for (auto& cars : bodies) {
		moving.push_back(move(cars));
	}
	bodies.clear();
}

void Cars::drawCars(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y, Sprite& plr, double& spd) {
	grid.clear();
	for (auto it = moving.begin();it != moving.end();it++) {
		grid.add(&(*it));
	}
	Car pl;
	pl.body = plr;
	pl.speed = spd;
	pl.id = getNextCarId();
	pl.engineSound.setBuffer(engineRunningBuffer);
	pl.brakeSound.setBuffer(brakeBuffer);
	pl.hornSound.setBuffer(hornBuffer);
	pl.honk.restart();
	grid.add(&pl);

	const double pi = 3.14159265358979323846;
	const float maxDis = 10.0f; 
	View view = window.getView();
	FloatRect viewBounds(view.getCenter() - view.getSize() / 2.f, view.getSize());

	for (auto it = moving.begin(); it != moving.end();) {
		Car& car = *it;
		bool shouldIncrement = true;
		if ((car.body.getPosition().x > 29335.f || car.body.getPosition().x < 0.f || car.body.getPosition().y > 16504.f || car.body.getPosition().y < 0.f) && car.out == true) {
			car.out = false;
			shouldIncrement = false;
			it = moving.erase(it);
			continue;
		}
		if ((car.body.getPosition().x < 29335.f && car.body.getPosition().x > 0.f) && (car.body.getPosition().y < 16504.f && car.body.getPosition().y > 0.f)) {
			car.out = true;
		}

		if (car.isFollowingCurve) {
			if (car.curvePathIndex >= car.currentCurvePath.size()) car.isFollowingCurve = false;
			else {
				Vector2f targetPoint = car.currentCurvePath[car.curvePathIndex];

				targetPoint.x *= RENDER_WORLD_SCALE_X;
				targetPoint.y *= RENDER_WORLD_SCALE_Y;

				Vector2f direction = targetPoint - car.body.getPosition();
				float distanceToTarget = sqrt(direction.x * direction.x + direction.y * direction.y);
				float targetAngle = atan2(direction.y, direction.x) * 180.f / pi;

				if (distanceToTarget < maxDis) {
					car.curvePathIndex++;
				}

				float currentAngle = car.body.getRotation();
				float angleDiff = targetAngle - currentAngle;

				if (angleDiff > 180)  angleDiff -= 360;
				if (angleDiff < -180) angleDiff += 360;
				car.body.rotate(angleDiff * 1.f);
			}
		}
		else {
			Vector2f carPos = car.body.getPosition();
			Vector2f carPosMapCoords = { carPos.x / (float)RENDER_WORLD_SCALE_X, carPos.y / (float)RENDER_WORLD_SCALE_Y };

			for (const auto& startPoint : curveStartPoints) {
				float dx = startPoint.x - carPosMapCoords.x;
				float dy = startPoint.y - carPosMapCoords.y;
				if (dx * dx + dy * dy < (10.f * 10.f)) {

					vector<Vector2f> path = findOrderedCurvePath(startPoint);
					if (path.size() > 1) {
						car.speed = max(1.0, car.speed - 0.01);
						car.isFollowingCurve = true;
						car.currentCurvePath = path;
						car.curvePathIndex = 0;
						break;
					}
				}
				else if (car.speed < 1.5) car.speed = 1.5f;
			}
		}
		vector<Car*> nearbyCars = grid.getNearbyCars(&car);
		if (viewBounds.intersects(car.body.getGlobalBounds())) {

			if (car.speed <= car.lastspeed - 0.01 && car.speed > 0.f) {
				if (car.brakeSound.getStatus() != Sound::Playing) {
					car.brakeSound.play(); // Use car.brakeSound
				}
			}
			else if (car.brakeSound.getStatus() == Sound::Playing) {
				car.brakeSound.stop(); // Use car.brakeSound
			}

			if (car.speed > 0.f) {
				if (car.engineSound.getStatus() != Sound::Playing) {
					car.engineSound.play(); // Use car.engineSound
				}
				float max_ai_speed = 1.5f;
				float pitch = 1.0f + (car.speed / max_ai_speed);
				car.engineSound.setPitch(pitch); // Use car.engineSound
			}
			else {
				if (car.engineSound.getStatus() == Sound::Playing) {
					car.engineSound.stop(); // Use car.engineSound
				}
			}

			if (car.speed == 0 && !car.still) {
				car.still = true;
				car.honk.restart();
			}

			else if (car.speed > 0) {
				car.still = false;
			}

			if (car.honk.getElapsedTime().asSeconds() >= 60 && car.still) {
				if (car.hornSound.getStatus() != Sound::Playing) {
					car.hornSound.play();
				}
			}
			else if (car.hornSound.getStatus() == Sound::Playing) {
				car.hornSound.stop();
			}
		}
		else {
			if (car.engineSound.getStatus() == Sound::Playing) {
				car.engineSound.stop(); // Use car.engineSound
			}
			if (car.hornSound.getStatus() == Sound::Playing) {
				car.hornSound.stop();
			}
		}

		car.lastspeed = car.speed;
		speedctrl(car, nearbyCars, RENDER_WORLD_SCALE_X, RENDER_WORLD_SCALE_Y); 
		float ang = car.body.getRotation();
		float dx = car.speed * cosf(ang * pi / 180.f);
		float dy = car.speed * sinf(ang * pi / 180.f);
		car.body.move(dx, dy);

		if (viewBounds.intersects(car.body.getGlobalBounds())) {
			window.draw(car.body);
		}
		if (shouldIncrement) it++;
	}
}
