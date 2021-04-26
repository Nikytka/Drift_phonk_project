#pragma once

#include <SFML\System.hpp>

class Target
{
	const float RADIUS = 10.0f; // Target radius

	sf::Vector2f pos = { 0.0f, 0.0f }; // Target position

public:
	sf::Vector2f get_pos(); // Get target position
	void set_pos(sf::Vector2f new_pos); // Set new target position

	float get_rad(); // Get target radius
};