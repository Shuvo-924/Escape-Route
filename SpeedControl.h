#ifndef SPEEDCONTROL_H
#define SPEEDCONTROL_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include "CustomFunctions.h"
#include "SpawnCars.h"

using namespace sf;
using namespace std;

struct Car;

void speedctrl(Car& car, const vector<Car*>& nearbyCars);

#endif