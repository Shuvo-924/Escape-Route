#include "SpeedControl.h"
#include "SpawnCars.h"
#include "SignalControl.h"

const double pi = 3.14159265358979323846;

void speedctrl(Car& car, const vector<Car*>& nearbyCars)
{
    float left = car.body.getGlobalBounds().left + car.body.getGlobalBounds().width * cosf(car.body.getRotation() * pi / 180.f);
    float top = car.body.getGlobalBounds().top + car.body.getGlobalBounds().width * sinf(car.body.getRotation() * pi / 180.f);
    FloatRect colArea(Vector2f(left, top), car.body.getGlobalBounds().getSize());
    bool collision_imminent = false;
    for (Car* otherCar : nearbyCars) {
        if (otherCar->id == car.id) {
            continue;
        }

        if (colArea.intersects(otherCar->body.getGlobalBounds())) {
            if (otherCar->speed < car.speed || otherCar->body.getRotation() != car.body.getRotation()) {
                collision_imminent = true;
                car.speed = max(0.0, car.speed - 0.01);
            }
        }

    }
    if (!collision_imminent && car.speed < 1.0f) {
        car.speed += 0.001;
    }
}
