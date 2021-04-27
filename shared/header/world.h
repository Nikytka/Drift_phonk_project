#pragma once

#include <mutex>
#include <unordered_map>
#include <iostream>
#include <random>

#include "player.h"
#include "scene.h"

// Player id type
using PlayerId = int;

// Game world structure
class World
{
    static const int WORLD_WIDTH = 800; // World x span
    static const int WORLD_HEIGHT = 800; // World y span

    std::unordered_map<PlayerId, Player> players; // Players map

    Scene GameScene = Scene::Lobby; // Default scene is lobby

public:
    std::mutex mutex; // Mutex
    
    void update(float dt); // Update world function (Updates all players positions)
    
    std::unordered_map<PlayerId, Player>& get_players(); // Get players map
    
    sf::Vector2f get_random_pos(); // Get random position on map
    
    static sf::Vector2i get_size(); // Get world size

    void remove_player(PlayerId clientId); // Remove player from the world

    void show_players(); // Print out all player currently in the world

    Scene GetScene(); // Get current game scene
    void SetScene(Scene scene); // Set game scene
};
