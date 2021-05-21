#pragma once

#include <SFML\Graphics.hpp>

#include "world.h"
#include "textbox.h"

class Viewer : public sf::RenderWindow
{
    // Some standart values: 800 800, 1920 1080, 3840 2160, 5760 3240, 7680 4320
    const int VIEWER_WIDTH = 800;
    const int VIEWER_HEIGHT = 800;

    const int NUMBER_OF_CARS = 3;
    std::map<int, sf::Texture> car_textures; // Map of car textures

    sf::Font font; // Text font

    const int NUMBER_OF_CONNECT_TEXTBOXES = 3; // Number of textboxes in connect scene
    const int NUMBER_OF_CONNECT_BUTTONS = 4; // Number of buttons in connect scene (total, with textboxes)
    const float SPACE_BETWEEN_CONNECT_BUTTONS = 0.1f; // Space between connect scene buttons (relative to viewer size)
    std::map<int, Textbox> connect_textboxes; // Map of textboxes in connect scene
    std::map<int, sf::Text> connect_buttons; // Map of buttons in connect scene
    int connect_selected_button = 0; // Currently selected button in connect scene

    const int NUMBER_OF_LOBBY_BUTTONS = 3; // Number of buttons in lobby
    std::map<int, sf::Text> lobby_buttons; // Map of buttons in lobby
    int lobby_selected_button = 0; // Currently selected button in lobby

public:
    Viewer(const std::string& name); // Viewer constructor
    
    void handleEvents(); // Handling viewer events function
    
    void draw_connect(World& world); // Draw connection screen scene
    void draw_lobby(World& world); // Draw lobby function
    void draw_car_selection(World& world); // Draw hero selection screen
    void draw_gameplay(World& world); // Draw world during the game
    
    int get_lobby_selected_button(); // Get selected lobby button
    void set_lobby_selected_button(int button); // Set selected lobby button
    
    std::map<int, sf::Text>& get_lobby_buttons(); // Get lobby buttons map
};
