#pragma once

#include <SFML\Graphics.hpp>

#include "world.h"

class Viewer : public sf::RenderWindow
{
    const int VIEWER_WIDTH = 800;
    const int VIEWER_HEIGHT = 800;

    const int NUMBER_OF_HEROES = 3;
    std::map<int, sf::Texture> player_textures; // Map of player textures

    sf::Font font; // Text font

    const int NUMBER_OF_LOBBY_BUTTONS = 3; // Number of buttons in lobby
    std::map<int, sf::Text> lobby_buttons; // Map of buttons in lobby
    int lobby_selected_button = 0; // Currently selected button in lobby

public:
    Viewer(const std::string& name); // Viewer constructor
    
    void handleEvents(); // Handling viewer events function
    
    void draw_lobby(World& world); // Draw lobby function
    void draw_hero_selection(World& world); // Draw hero selection screen
    void draw_gameplay(World& world); // Draw world during the game
    void draw_gameover(); // Draw endgame screen
    
    int get_lobby_selected_button(); // Get selected lobby button
    void set_lobby_selected_button(int button); // Set selected lobby button
    
    std::map<int, sf::Text>& get_lobby_buttons(); // Get lobby buttons map
};
