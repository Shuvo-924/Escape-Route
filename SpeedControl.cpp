#include "SpeedControl.h"
#include "SpawnCars.h"
#include "SignalControl.h"

const double pi = 3.14159265358979323846;
vector<Vector2f> rt = { {135, 45}, {45, 315}, {315, 225}, {225, 135} };

bool ObeySignals(Car& car)
{
    Vector2f car_pos = car.body.getPosition();

    Vector2f front_unscaled = {
        car_pos.x + 96 * cosf(car.body.getRotation() * pi / 180.f),
        car_pos.y + 96 * sinf(car.body.getRotation() * pi / 180.f)
    };

    View v;
    v.setSize(1300.f, 600.f);
    v.setCenter(car_pos.x, car_pos.y);
    v.setRotation(car.body.getRotation());
    FloatRect viewBounds(v.getCenter() - v.getSize() / 2.f, v.getSize());

    for (size_t k = 0; k < Signal::points.size(); k++) {
        if (!viewBounds.contains(Vector2f((Signal::points[k]).x * 29335.f / 1920.f, (Signal::points[k]).y * 16504.f / 1080.f))) continue;
        for (int i = 0; i < 4; i++) {
            // Check if the car is aligned with the direction of a red light
            if (((int)car.body.getRotation() % 360) < rt[i].x && car.body.getRotation() > rt[i].y && Signal::stop[k][i]) {
                Vector2f stop_line_pos = Vector2f((Signal::points[k] + Vector2f(Signal::dir[i].x * 15.f, Signal::dir[i].y * 15.f)).x * 29335.f / 1920.f, (Signal::points[k] + Vector2f(Signal::dir[i].x * 15.f, Signal::dir[i].y * 15.f)).y * 16504.f / 1080.f);
              
                float distance_to_stop_line = 0.f;
                float offset_from_lane_center = 0.f;

                const float stopping_distance = 300.f; // How many units before the line to start stopping
                const float lane_width_tolerance = 480.f; // How far from the center of the lane is acceptable

                switch (i) {
                case 2: // Northbound
                    distance_to_stop_line = front_unscaled.y - stop_line_pos.y;
                    offset_from_lane_center = fabs(front_unscaled.x - stop_line_pos.x);
                    if (distance_to_stop_line > 0.f && distance_to_stop_line < stopping_distance && offset_from_lane_center < lane_width_tolerance) return true;
                    break;
                case 3: // Westbound
                    distance_to_stop_line = front_unscaled.x - stop_line_pos.x;
                    offset_from_lane_center = fabs(front_unscaled.y - stop_line_pos.y);
                    if (distance_to_stop_line > 0.f && distance_to_stop_line < stopping_distance && offset_from_lane_center < lane_width_tolerance) return true;
                    break;
                case 0:  // Southbound
                    distance_to_stop_line = stop_line_pos.y - front_unscaled.y;
                    offset_from_lane_center = fabs(front_unscaled.x - stop_line_pos.x);
                    if (distance_to_stop_line > 0.f && distance_to_stop_line < stopping_distance && offset_from_lane_center < lane_width_tolerance) return true;
                    break;
                case 1:   // Eastbound
                    distance_to_stop_line = stop_line_pos.x - front_unscaled.x;
                    offset_from_lane_center = fabs(front_unscaled.y - stop_line_pos.y);
                    if (distance_to_stop_line > 0.f && distance_to_stop_line < stopping_distance && offset_from_lane_center < lane_width_tolerance) return true;
                    break;
                }
            }
        }
    }

    return false; 
}

void getBorders(Sprite& plr, double& speed)
{
    Vector2f points;
    if (plr.getGlobalBounds().contains(points)) speed = max(0., speed - 0.5);
}

void speedctrl(Car& car, const vector<Car*>& nearbyCars, double& RENDER_WORLD_SCALE_X, double& RENDER_WORLD_SCALE_Y) {
    float left = car.body.getGlobalBounds().left + car.body.getGlobalBounds().width * cosf(car.body.getRotation() * pi / 180.f);
    float top = car.body.getGlobalBounds().top + car.body.getGlobalBounds().width * sinf(car.body.getRotation() * pi / 180.f);
    FloatRect colArea(Vector2f(left, top), Vector2f(car.body.getGlobalBounds().getSize().x * RENDER_WORLD_SCALE_X, car.body.getGlobalBounds().getSize().y * RENDER_WORLD_SCALE_Y));
    bool collision_imminent = false;

    for (Car* otherCar : nearbyCars) {
        if (otherCar->id == car.id) continue;
        if (colArea.intersects(otherCar->body.getGlobalBounds())) {
            if (otherCar->speed < car.speed || otherCar->body.getRotation() != car.body.getRotation()) {
                collision_imminent = true;
                break; // A collision is likely, no need to check other cars
            }
        }
    }

    bool is_signal_red = ObeySignals(car);

    if (is_signal_red || collision_imminent) {
        car.speed = max(0.0, car.speed - 0.015);
        if (car.speed < 0.1) car.speed = 0;
    }
    else {
        if (car.speed < 1.5f) {
            car.speed += 0.0015;
        }
    }
}
