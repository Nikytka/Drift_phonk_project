#pragma once

#include "controls.h"

sf::Vector2f controls_arrows()
{
    sf::Vector2f controls;

    controls.x = float(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) - float(sf::Keyboard::isKeyPressed(sf::Keyboard::Left));
    controls.y = float(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) - float(sf::Keyboard::isKeyPressed(sf::Keyboard::Up));

    return controls;
}

sf::Vector2f controls_wasd()
{
    sf::Vector2f controls;

    controls.x = float(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) - float(sf::Keyboard::isKeyPressed(sf::Keyboard::A));
    controls.y = float(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) - float(sf::Keyboard::isKeyPressed(sf::Keyboard::W));

    return controls;
}
