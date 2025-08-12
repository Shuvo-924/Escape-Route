#include "SpatialGrid.h"
#include "SpawnCars.h"

Vector2i SpatialGrid::getCellCoords(const Vector2f& position)
{
    int x = static_cast<int>(position.x / cellWidth);
    int y = static_cast<int>(position.y / cellHeight);

    if (x < 0) x = 0;
    if (x >= gridWidth) x = gridWidth - 1;
    if (y < 0) y = 0;
    if (y >= gridHeight) y = gridHeight - 1;

    return Vector2i(x, y);
}

SpatialGrid::SpatialGrid(float worldWidth, float worldHeight, int cellsX, int cellsY)
{
    this->gridWidth = cellsX;
    this->gridHeight = cellsY;
    this->cellWidth = worldWidth / cellsX;
    this->cellHeight = worldHeight / cellsY;

    grid.resize(gridWidth, vector<list<Car*>>(gridHeight));
}

void SpatialGrid::clear() {
    for (int x = 0; x < gridWidth; ++x) {
        for (int y = 0; y < gridHeight; ++y) {
            grid[x][y].clear();
        }
    }
}

void SpatialGrid::add(Car* car)
{
    Vector2i cell = getCellCoords(car->body.getPosition());
    grid[cell.x][cell.y].push_back(car);
}

vector<Car*> SpatialGrid::getNearbyCars(Car* car)
{
    vector<Car*> nearby;
    Vector2i centerCell = getCellCoords(car->body.getPosition());

    for (int x = centerCell.x - 1; x <= centerCell.x + 1; ++x) {
        for (int y = centerCell.y - 1; y <= centerCell.y + 1; ++y) {
            if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
                for (Car* otherCar : grid[x][y]) {
                    nearby.push_back(otherCar);
                }
            }
        }
    }
    return nearby;
}
