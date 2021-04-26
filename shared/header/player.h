#pragma once

#include "SFML\System.hpp"
#include "debug.h"

class Player
{
    const float RADIUS = 100.0f; // Player radius
    const int FINAL_SCORE = 3; // Score needed to win the game
    const float SPEED = 300.0f; // Pixels per second, used as a reference to calculate velocity

    int score = 0; // Player score, starting score is 0
    bool winner = false; // Did the player reach endgame score
    bool ready = false; // If player is ready to play (in game lobby)

    int selected_car_id = 0; // Id number of a selected car
    bool is_car_selected = false; // Did the player select a car to play

    sf::Vector2f pos; // Position
    sf::Vector2f v; // Velocity

    sf::Vector2f controls; // Current player controls status

public:
    void update(float dt); // Update player position

    float get_maxspeed(); // Get maximum player speed

    sf::Vector2f get_pos(); // Get player position
    void set_pos(sf::Vector2f new_pos); // Set new player position

    sf::Vector2f get_vel(); // Get player velocity
    void set_vel(sf::Vector2f vel); // Update player velocity to new value

    float get_rad(); // Get player radius
    
    void increase_score(); // Increase player score
    int get_score(); // Get player score
    void set_score(int score); // Set player score

    bool IfWinner(); // Did the player reach the score needed to win

    void won_the_game(); // This player won the game
    void no_longer_winner(); // Make this player no longer winner
    
    bool IfReady(); // If player is ready to play (in game lobby)
    
    sf::Vector2f get_controls(); // Get player controls
    void set_controls(sf::Vector2f controls); // Set player controls

    void player_ready(); // Player is ready
    void player_not_ready(); // Player is not ready

    int get_selected_car(); // Get a car id selected by this player
};