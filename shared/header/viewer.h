#pragma once

#include <SFML\Graphics.hpp>

#include "world.h"

class Viewer : public sf::RenderWindow
{
    const int VIEWER_WIDTH = 1920;
    const int VIEWER_HEIGHT = 1080;

    const int NUMBER_OF_CARS = 3;
    std::map<int, sf::Texture> car_textures; // Map of car textures

    sf::Font font; // Text font

    const int NUMBER_OF_LOBBY_BUTTONS = 3; // Number of buttons in lobby
    std::map<int, sf::Text> lobby_buttons; // Map of buttons in lobby
    int lobby_selected_button = 0; // Currently selected button in lobby

public:
    Viewer(const std::string& name); // Viewer constructor
    
    void handleEvents(); // Handling viewer events function
    
    void draw_lobby(World& world); // Draw lobby function
    void draw_car_selection(World& world); // Draw hero selection screen
    void draw_gameplay(World& world); // Draw world during the game
    
    int get_lobby_selected_button(); // Get selected lobby button
    void set_lobby_selected_button(int button); // Set selected lobby button
    
    std::map<int, sf::Text>& get_lobby_buttons(); // Get lobby buttons map
};
