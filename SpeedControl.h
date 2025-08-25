#ifndef SPEEDCONTROL_H
#define SPEEDCONTROL_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "CustomFunctions.h"
#include "SpawnCars.h"

using namespace sf;
using namespace std;

struct Car;

void speedctrl(Car& car, const vector<Car*>& nearbyCars); //Function to increase or decrease the Car speed
bool ObeySignals(Car& car); //Function to check for Red signals in front of the car
void getBorders(Sprite& plr, double& speed);

#endif
