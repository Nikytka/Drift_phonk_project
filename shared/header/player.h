#pragma once

#include "SFML\System.hpp"
#include "debug.h"


class Player
{
    std::string nickname; // Player nickname

    const float RADIUS = 100.0f; // Player radius
    const float SPEED = 300.0f; // Pixels per second, used as a reference to calculate velocity

    bool ready = false; // If player is ready to play (in game lobby)

    int selected_car_id = 0; // Id number of a selected car
    bool is_car_selected = false; // Did the player select a car to play

    sf::Vector2f pos; // Position
    sf::Vector2f v; // Velocity

    float car_angle; // Angle between direction of speed and horizontal(initial) direction

    sf::Vector2f controls; // Current player controls status

    int gear = 1; // Number of gear

    float gearbox_controls; // Current player gear controls

public:
    void update(float dt); // Update player position

    float get_maxspeed(); // Get maximum player speed

    sf::Vector2f get_pos(); // Get player position
    void set_pos(sf::Vector2f new_pos); // Set new player position

    float getAbsoluteSpeed(); // Get absolute player speed

    sf::Vector2f get_vel(); // Get player velocity
    void set_vel(sf::Vector2f vel); // Update player velocity to new value

    float get_rad(); // Get player radius
    
    bool isReady(); // Is the player ready to play (in game lobby)
    
    sf::Vector2f get_controls(); // Get player controls
    void set_controls(sf::Vector2f controls); // Set player controls

    float get_gearbox_controls(); // Get player gearbox controls
    void set_gear_controls(float gear_controls); // Set player gear controls

    void player_ready(); // Player is ready
    void player_not_ready(); // Player is not ready

    int get_selected_car(); // Get a car id selected by this player
    void set_selected_car(int car); // Set selected hero number

    bool isCarSelected(); // Did the player select car
    void setCarSelectionConfirm(bool x); // Set car selection confirmation

    float get_angle(); // Get angle of velocity of a car
    void add_angle(float, float); // Add little angle to the angle of velocity
    void add_angle1(float, float); // Add little angle to the angle of velocity
    void set_angle(float); // Set the angle of velocity of player

    int get_gear(); // Get current gear
    void set_gear(int); // Set gear
    void add_gear(float); // Add gear

    std::string getNickname(); // Get player nickname
    void setNickname(std::string name); // Set player nickname
};