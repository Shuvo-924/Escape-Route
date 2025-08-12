#ifndef MINIMAP_H
#define MINIMAP_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>

using namespace sf;
using namespace std;

class Minimap {
public:
	Texture minimapBackgroundTexture;
	Texture playerDotTexture;
	View minimapView;
	Sprite minimapBackgroundSprite;
	Sprite playerDotSprite;
	RectangleShape minimapBorder;
	Minimap();
	void expand();
	void reset();
	void setcenter(float x, float y);
};

#endif