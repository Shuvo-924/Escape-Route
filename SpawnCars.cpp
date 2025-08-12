#include "SpawnCars.h"
#include "SpeedControl.h"

unsigned int getNextCarId() {
	static unsigned int nextId = 0;
	return nextId++;
}

void Cars::initVar()
{
	srand(time(NULL));
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
	int n = 10;
	while (n--) AddCars();
	SpawnDelay.restart();
	plr_id = getNextCarId();
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
		double spd = 1.f;
		newCar.speed = spd;

		pair<Vector2f, float> start = points[rand() % points.size()];
		newCar.body.setPosition(Vector2f(start.first.x * scaleX, start.first.y * scaleY) - Vector2f(newCar.body.getGlobalBounds().width * cosf(start.second * 3.14159265358979323846 / 180.f), newCar.body.getGlobalBounds().width * sinf(start.second * 3.14159265358979323846 / 180.f)));
		newCar.body.setRotation(start.second);

		FloatRect bounds = newCar.body.getLocalBounds();
		newCar.body.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

		bodies.push_back(newCar);
	}
}

Cars::Cars(float worldWidth, float worldHeight) : grid(worldWidth, worldHeight, 8, 8)
{
	initVar();
}

void Cars::Spawn()
{
	for (int i = 0;i < bodies.size();i++) {
		moving.push_back(bodies[i]);
	}
	bodies.clear();
}

void Cars::drawCars(RenderWindow& window, double RENDER_WORLD_SCALE_X, double RENDER_WORLD_SCALE_Y, Sprite& plr, double& spd)
{
	grid.clear();
	Car player;
	player.body = plr;
	player.speed = spd;
	player.id = plr_id;
	grid.add(&player);
	for (auto it = moving.begin(); it != moving.end(); ++it) {
		grid.add(&(*it));
	}

	View view = window.getView();
	FloatRect viewBounds(view.getCenter() - view.getSize() / 2.f, view.getSize());

	// 3. Update and draw each car
	for (auto it = moving.begin(); it != moving.end(); ++it) {
		vector<Car*> nearbyCars = grid.getNearbyCars(&(*it));

		speedctrl(*it, nearbyCars);

		const double pi = 3.14159265358979323846;
		float ang = it->body.getRotation();
		double speed = it->speed;
		float dx = speed * cosf(ang * pi / 180.f);
		float dy = speed * sinf(ang * pi / 180.f);

		it->body.move(dx, dy);

		if (viewBounds.intersects(it->body.getGlobalBounds())) {
			window.draw(it->body);
		}
	}
}
