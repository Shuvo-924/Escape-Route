#include "Game.h"

//Private functions
void Game::initVar()
{
	this->window = nullptr;
	this->playerCar.loadFromFile("Car(G).png");
	this->plr.setTexture(playerCar);
	this->plr.setScale(0.03, 0.03);
	this->plr.setPosition(100, 100);
}

void Game::initWin()
{
	this->desktopMode = VideoMode::getDesktopMode();
	this->window = new RenderWindow(this->desktopMode, "Escape Route", Style::Close | Style::Titlebar);
	//Code for fetching Coordinate data from a text file
	/*ifstream file("data.txt");
	int x, y;
	while (file >> x >> y) {
		points.emplace_back(x, y);
	}
	file.close();*/
	/*for (auto& p : points) {
		CircleShape x(5.f);
		x.setFillColor(Color::Black);
		x.setPosition(p);
		this->window->draw(x);
	}*/
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
		}
	}
}


void Game::update()
{
	this->pollEvents();
	float dx, dy, ang;
	const double pi = 3.14159265358979323846;
	ang = this->plr.getRotation();
	dx = speed * cosf(ang * pi / 180.f);
	dy = speed * sinf(ang * pi / 180.f);
	plr.move(dx, dy);
	if (Keyboard::isKeyPressed(Keyboard::Down)) {
		(this->speed > 0) ? this->speed -= 0.01 : this->speed = 0;
	}
	if (Keyboard::isKeyPressed(Keyboard::Up)) {
		if (speed < 1);
		this->speed += 0.001;
	}
	if (Keyboard::isKeyPressed(Keyboard::Right)) {
		plr.rotate(0.45);
	}
	if (Keyboard::isKeyPressed(Keyboard::Left)) {
		plr.rotate(-0.45);
	}
	view.setCenter(plr.getPosition());//Setting the view around the moving Car
	view.setSize(160, 90);
	//A red dot marking the position of the Player in the MiniMap
	mark.setPosition(plr.getPosition().x + (plr.getGlobalBounds().width / 2) * sinf(plr.getRotation() * pi / 180.f), plr.getPosition().y - (plr.getGlobalBounds().height / 2) * cosf(plr.getRotation() * pi / 180.f));
	mark.setRotation(plr.getRotation());	
}

void Game::render()
{
	this->window->clear(Color::Green);
	//this->window->setView(view);
	this->window->draw(plr);
	/*this->window->draw(mark);
	this->window->draw(plc);*/
	this->window->display();
}
