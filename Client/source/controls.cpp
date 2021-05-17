#pragma once

#include "controls.h"

float controls_arrows()
{
    float controls;

    controls = sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ? 1 : (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ? -1 : 0);

    return controls;
}

sf::Vector2f controls_wasd()
{
    sf::Vector2f controls;

    controls.x = float(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) - float(sf::Keyboard::isKeyPressed(sf::Keyboard::A));
    controls.y = float(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) - float(sf::Keyboard::isKeyPressed(sf::Keyboard::W));

    return controls;
}
