#include "TileMap.h"
#include <iostream>
#include <fstream> 
#include <sstream>

TileMap::TileMap(const string& tilesetPath, int tileSize, int mapWidthInTiles, int mapHeightInTiles)
    : m_tileSize(tileSize), m_mapWidth(mapWidthInTiles), m_mapHeight(mapHeightInTiles) {
    map<string, IntRect> tileData;
    ifstream manifestFile(tilesetPath + "/tile_manifest.csv");
    string line;

    if (manifestFile.is_open()) {
        // Skip the header line
        getline(manifestFile, line);

        while (getline(manifestFile, line)) {
            stringstream ss(line);
            string filename;
            string width_str;
            string height_str;

            getline(ss, filename, ',');
            getline(ss, width_str, ',');
            getline(ss, height_str, ',');

            tileData[filename] = IntRect(0, 0, stoi(width_str), stoi(height_str));
        }
    }
    else {
        cerr << "Error: Could not open tile_manifest.csv!" << endl;
        // Handle error - maybe exit or throw an exception
    }

    for (int y = 0; y < m_mapHeight; ++y) {
        for (int x = 0; x < m_mapWidth; ++x) {
            // Construct the filename to look up in our manifest data
            string filename_key = "tile_" + to_string(y) + "_" + to_string(x) + ".png";
            string texturePath = tilesetPath + "/" + filename_key;

            // Load the texture if not already loaded
            if (m_textures.find(filename_key) == m_textures.end()) {
                if (!m_textures[filename_key].loadFromFile(texturePath)) {
                    cerr << "Error: Could not load tile texture: " << texturePath << endl;
                }
            }

            // Create a sprite for this tile
            Sprite sprite;
            sprite.setTexture(m_textures[filename_key]);

            if (tileData.count(filename_key)) {
                sprite.setTextureRect(tileData[filename_key]);
            }

            // Set the position in the world. This logic does not change.
            sprite.setPosition(static_cast<float>(x * m_tileSize), static_cast<float>(y * m_tileSize));
            m_sprites.push_back(sprite);
        }
    }
}

// --- THIS IS THE CULLING IMPLEMENTATION ---
void TileMap::draw(RenderTarget& target, RenderStates states) const {
    // Get the view from the render target, which tells us what the camera can see
    View view = target.getView();
    FloatRect viewBounds(view.getCenter() - view.getSize() / 2.f, view.getSize());

    // Calculate which tile indices are potentially visible
    int startCol = max(0, static_cast<int>(viewBounds.left / m_tileSize));
    int endCol = min(m_mapWidth, static_cast<int>((viewBounds.left + viewBounds.width) / m_tileSize) + 1);

    int startRow = max(0, static_cast<int>(viewBounds.top / m_tileSize));
    int endRow = min(m_mapHeight, static_cast<int>((viewBounds.top + viewBounds.height) / m_tileSize) + 1);

    // Iterate ONLY through the visible tiles and draw them
    for (int y = startRow; y < endRow; ++y) {
        for (int x = startCol; x < endCol; ++x) {
            // Sprites are stored in a 1D vector, so calculate the correct index
            int index = y * m_mapWidth + x;
            if (index >= 0 && index < m_sprites.size()) {
                target.draw(m_sprites[index], states);
            }
        }
    }
}