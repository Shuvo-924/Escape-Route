#include "Game.h"

//Private functions
void Game::initVar()
{
	this->window = nullptr;
}

void Game::initWin()
{
	this->desktopMode = VideoMode::getDesktopMode();
	this->window = new RenderWindow(this->desktopMode, "Traffic Simulation", Style::Close | Style::Titlebar);
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
}

void Game::render()
{
	this->window->clear(Color::Blue);
	
	this->window->display();
}