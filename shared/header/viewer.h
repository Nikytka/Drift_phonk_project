#pragma once

#include <SFML\Graphics.hpp>

#include "world.h"
#include "textbox.h"

class Viewer : public sf::RenderWindow
{
    // Some standart values: 800 800, 1920 1080, 3840 2160, 5760 3240, 7680 4320, 8889 5000
    const int VIEWER_WIDTH = 1920;
    const int VIEWER_HEIGHT = 1080;

    const int NUMBER_OF_CARS = 3;
    const float CAR_SCALE_FACTOR = 0.7f;
    std::map<int, sf::Texture> car_textures; // Map of car textures

    sf::Texture map_texture;
    sf::Sprite map_sprite;

    bool wholeMapView = false;

    sf::Font font; // Text font

    // CONNECT SCENE PARAMETERS
    const int NUMBER_OF_CONNECT_TEXTBOXES = 3; // Number of textboxes in connect scene
    const int NUMBER_OF_CONNECT_BUTTONS = 4; // Number of buttons in connect scene (total, with textboxes)
    const float SPACE_BETWEEN_CONNECT_BUTTONS = 0.1f; // Space between connect scene buttons (relative to viewer size)
    std::map<int, Textbox> connect_textboxes; // Map of textboxes in connect scene
    std::map<int, sf::Text> connect_buttons; // Map of buttons in connect scene
    int connect_selected_button = 0; // Currently selected button in connect scene

    // LOBBY SCENE PARAMETERS
    const int NUMBER_OF_LOBBY_BUTTONS = 3; // Number of buttons in lobby scene
    const float SPACE_BETWEEN_LOBBY_BUTTONS = 0.1f; // Space between lobby buttons (relative to viewer size)
    std::map<int, sf::Text> lobby_buttons; // Map of buttons in lobby scene
    int lobby_selected_button = 0; // Currently selected button in lobby scene

    // GAMEPLAY SCENE PARAMETERS
    const float SPACE_BETWEEN_HUD_ITEMS = 0.05f; // Space between gameplay scene hud items (relative to viewer size)
    const float SPEED_REDUCE_FACTOR = 17.0f; // A number by which the hud speed is reduced (for reality)
    sf::Text hudSpeed; // Car speed in gameplay scene
    sf::Text hudGear; // Car gear in gameplay scene

    // PAUSE SCENE PARAMETERS
    const int NUMBER_OF_PAUSE_BUTTONS = 2; // Number of buttons in pause scene
    const float SPACE_BETWEEN_PAUSE_BUTTONS = 0.1f; // Space between pause buttons (relative to viewer size)
    std::map<int, sf::Text> pause_buttons; // Map of buttons in pause scene
    int pause_selected_button = 0; // Currently selected button in pause scene

public:
    Viewer(const std::string& name); // Viewer constructor
    
    void handleEvents(); // Handling viewer events function
    
    void draw_connect(World& world); // Draw connection screen scene
    void draw_lobby(World& world); // Draw lobby function
    void draw_car_selection(World& world, int clientId); // Draw car selection screen
    void draw_gameplay(World& world, int myId); // Draw world during the game
    void draw_pause(World& world, int myId); // Draw pause scene

    int get_connect_menu_size(); // Get the number of total number of buttons (including textboxes)
    int get_number_of_textboxes(); // Get the number of textboxes in the connection scene
    int get_connect_selected_button(); // Get selected connect scene button
    void set_connect_selected_button(int button); // Set selected connect scene button
    std::map<int, sf::Text>& get_connect_buttons(); // Get connect scene buttons map
    std::map<int, Textbox>& getTextbox(); // Get connect scene textboxes map
    
    int get_lobby_selected_button(); // Get selected lobby button
    void set_lobby_selected_button(int button); // Set selected lobby button
    std::map<int, sf::Text>& get_lobby_buttons(); // Get lobby buttons map

    int get_pause_selected_button(); // Get selected pause button
    void set_pause_selected_button(int button); // Set selected pause button
    std::map<int, sf::Text>& get_pause_buttons(); // Get pause buttons map

    int get_number_of_cars(); // Get the number of cars in the game

    void updateHudSpeed(float x); // Set new speed in gameplay scene hud
    void updateHudGear(int x); // Set new gear in gameplay scene hud

    bool ifWholeMapView(); // If the whole map view right now
    void setWholeMapView(bool x); // Set whole map view
};
