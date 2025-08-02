#ifndef TILEMAP_H
#define TILEMAP_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>

using namespace sf;
using namespace std;

class TileMap : public Drawable {
public:
    // Constructor
    TileMap(const string& tilesetPath, int tileSize, int mapWidthInTiles, int mapHeightInTiles);

    // The main draw method that performs culling
    virtual void draw(RenderTarget& target, RenderStates states) const override;

private:
    int m_tileSize;
    int m_mapWidth;  // Map width in number of tiles
    int m_mapHeight; // Map height in number of tiles

    // We use a map to store textures to avoid loading the same one multiple times.
    // In a real game, this would be part of a larger resource manager.
    map<string, Texture> m_textures;
    vector<Sprite> m_sprites;
};

#endif