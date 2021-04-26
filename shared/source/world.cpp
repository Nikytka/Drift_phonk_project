#pragma once

#include "world.h"

void World::update(float dt)
{
    std::lock_guard<std::mutex> guard(mutex);

    // Updating players positions
    for (auto& it : players)
    {
        it.second.update(dt);
    }
}

std::unordered_map<PlayerId, Player>& World::get_players()
{
    return this->players;
}

Target& World::get_target()
{
    return this->target;
}

sf::Vector2f World::get_random_pos()
{
    // Generating random position on the map
    // that is not inside any player

    sf::Vector2f pos; // New position
    int is_inside = 1; // For controlling the while cycle

    while (is_inside)
    {
        is_inside = 0;

        // Generating random position
        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count(); // Random seed
        std::default_random_engine generator(seed); // Generator
        std::uniform_real_distribution<float> distribution_x(float(this->get_size().x) * 0.05, float(this->get_size().x) * 0.95); // Creating x distribution
        std::uniform_real_distribution<float> distribution_y(float(this->get_size().y) * 0.05, float(this->get_size().y) * 0.95); // Creating y distribution
        pos = { distribution_x(generator), distribution_y(generator) }; // Generating position

        // Checking if new position is inside any player
        for (auto& it : this->players)
        {
            if (sqrt(pow((it.second.get_pos().x - pos.x), 2) + pow((it.second.get_pos().y
                - pos.y), 2)) < it.second.get_rad())
            {
                is_inside++;
            }
        }
    }

    return pos;
}

sf::Vector2i World::get_size()
{
    sf::Vector2i result;
    result.x = World::WORLD_WIDTH;
    result.y = World::WORLD_HEIGHT;
    return result;
}

void World::remove_player(PlayerId clientId)
{
    this->players.erase(clientId);
}

void World::show_players()
{
    if (this->get_players().size() == 0)
    {
        std::cout << "No players online" << std::endl;
        return;
    }

    std::cout << "Online players by their id\'s: ";
    for (auto it : this->get_players())
    {
        std::cout << it.first << " ";
    }
    
    std::cout << std::endl;
}

Scene World::GetScene()
{
    return this->GameScene;
}

void World::SetScene(Scene scene)
{
    this->GameScene = scene;
}
