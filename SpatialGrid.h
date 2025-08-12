#ifndef SPATIALGRID_H
#define SPATIALGRID_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <list>

using namespace std;
using namespace sf;

struct Car;

class SpatialGrid {
private:
    int gridWidth, gridHeight;
    float cellWidth, cellHeight;
    vector<vector<list<Car*>>> grid;

    // Helper to get grid coordinates from a world position
    Vector2i getCellCoords(const Vector2f& position);

public:
    // Constructor takes the total world size and the number of cells
    SpatialGrid(float worldWidth, float worldHeight, int cellsX, int cellsY);

    // Clears all cars from the grid (call once per frame)
    void clear();

    // Adds a car pointer to the appropriate cell
    void add(Car* car);

    // Gets all cars in the same cell and 8 neighboring cells
    vector<Car*> getNearbyCars(Car* car);
};

#endif