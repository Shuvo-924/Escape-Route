#ifndef CUSTOMFUNCTIONS_H
#define CUSTOMFUNCTIONS_H

using namespace sf;
using namespace std;

struct Vector2fHash {
	size_t operator()(const Vector2f& v) const {
		return hash<float>()(v.x) ^ (hash<float>()(v.y) << 1);
	}
};

//std::vector<sf::Vector2f> generateSmoothTurn(
//    const sf::Vector2f& p_before,
//    const sf::Vector2f& p_corner,
//    const sf::Vector2f& p_after,
//    int num_segments = 50,
//    float turn_tightness = 1.0f)
//{
//    std::vector<sf::Vector2f> curve_points;
//
//    // 1. Calculate the direction vectors for the incoming and outgoing lines.
//    sf::Vector2f dir_in = p_corner - p_before;
//    sf::Vector2f dir_out = p_after - p_corner;
//
//    // 2. Determine the distance from the corner to start and end the curve.
//    // We take the shorter of the two distances to ensure the curve fits.
//    float distance_in = std::sqrt(dir_in.x * dir_in.x + dir_in.y * dir_in.y);
//    float distance_out = std::sqrt(dir_out.x * dir_out.x + dir_out.y * dir_out.y);
//    float curve_distance = std::min(distance_in, distance_out) / turn_tightness;
//
//    // 3. Normalize the direction vectors (make them length 1).
//    dir_in /= distance_in;
//    dir_out /= distance_out;
//
//    // 4. Define the four points for the Cubic Bézier curve.
//    // P0: Start of the curve (on the incoming line).
//    sf::Vector2f P0 = p_corner - dir_in * curve_distance;
//    // P1: First control point (the corner itself). This makes the curve start smoothly.
//    sf::Vector2f P1 = p_corner;
//    // P2: Second control point (also the corner). This makes the curve end smoothly.
//    sf::Vector2f P2 = p_corner;
//    // P3: End of the curve (on the outgoing line).
//    sf::Vector2f P3 = p_corner + dir_out * curve_distance;
//
//    // 5. Generate the points along the curve using the Bézier formula.
//    // B(t) = (1-t)^3 * P0 + 3(1-t)^2 * t * P1 + 3(1-t) * t^2 * P2 + t^3 * P3
//    for (int i = 0; i <= num_segments; ++i)
//    {
//        float t = static_cast<float>(i) / num_segments;
//        float u = 1.0f - t;
//
//        // Calculate the blending functions for the cubic curve
//        float b0 = u * u * u;
//        float b1 = 3 * u * u * t;
//        float b2 = 3 * u * t * t;
//        float b3 = t * t * t;
//
//        // Calculate the point on the curve
//        sf::Vector2f point = b0 * P0 + b1 * P1 + b2 * P2 + b3 * P3;
//        curve_points.push_back(point);
//    }
//
//    return curve_points;
//}

#endif 
