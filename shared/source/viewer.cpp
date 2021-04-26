#pragma once

#include "viewer.h"
    
Viewer::Viewer(const std::string& name) : sf::RenderWindow(sf::VideoMode(800, 800), name)
{
    // Loading player textures
    this->player_textures[0].loadFromFile("hero0.png");
    this->player_textures[1].loadFromFile("hero1.png");
    this->player_textures[2].loadFromFile("hero2.png");

    // Loading font
    if (!font.loadFromFile("PressStart2P.ttf"))
    {
        std::cout << "Could not load font" << std::endl;
    }

    setFramerateLimit(60);

    // Setting up lobby buttons
    for (int i = 0; i < NUMBER_OF_LOBBY_BUTTONS; i++)
    {
        lobby_buttons[i].setFont(font);
    }
    lobby_buttons[0].setString("ready");
    lobby_buttons[1].setString("pick hero");
    lobby_buttons[2].setString("disconnect");
    for (int i = 0; i < NUMBER_OF_LOBBY_BUTTONS; i++)
    {
        float total_menu_height = (NUMBER_OF_LOBBY_BUTTONS - 1) * VIEWER_HEIGHT * 0.1f;
        for (int i = 0; i < NUMBER_OF_LOBBY_BUTTONS; i++)
        {
            total_menu_height += lobby_buttons[i].getGlobalBounds().height;
        }

        lobby_buttons[i].setOrigin(lobby_buttons[i].getGlobalBounds().width / 2.0f, lobby_buttons[i].getGlobalBounds().height / 2.0f);
        sf::Vector2f pos;
        pos.x = float(VIEWER_WIDTH) / 2.0f;
        pos.y = float(VIEWER_HEIGHT) / 2.0f + float(i) * float(VIEWER_HEIGHT) * 0.1f
            - total_menu_height / 2.0f;
        lobby_buttons[i].setPosition(pos);
    }
}

void Viewer::handleEvents()
{
    sf::Event event;
    while (pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            close();
    }
}

void Viewer::draw_lobby(World& world)
{
    // Setting black color as a background
    clear(sf::Color::Black);

    for (auto& it : lobby_buttons)
    {
        if (it.first == lobby_selected_button)
        {
            it.second.setColor(sf::Color::Red);
        }
        else
        {
            it.second.setColor(sf::Color::White);
        }

        sf::RenderWindow::draw(it.second);
    }

    // Displaying
    display();
}

void Viewer::draw_hero_selection(World& world)
{
    // Setting black color as a background
    clear(sf::Color::Black);



    // Displaying
    display();
}

void Viewer::draw_gameplay(World& world)
{
    // Setting black color as a background
    clear(sf::Color::Black);

    // Drawing players
    for (auto& it : world.get_players())
    {
        sf::Sprite player_model(this->player_textures[it.second.get_selected_car()]); // Set texture
        auto player_rect = player_model.getGlobalBounds();
        player_model.setOrigin(player_rect.width / 2.0f, player_rect.height / 2.0f); // Set sprite origin
        player_model.setPosition(it.second.get_pos()); // Set position
        player_model.setScale({ 0.7f, 0.7f });
        sf::RenderWindow::draw(player_model); // Draw
    }

    sf::Text target_text;
    target_text.setFont(font);
    target_text.setColor(sf::Color::White);
    target_text.setString("OTL10");
    target_text.setOrigin(target_text.getGlobalBounds().width / 2.0f, target_text.getGlobalBounds().height / 2.0f);
    target_text.setPosition(world.get_target().get_pos());
    sf::RenderWindow::draw(target_text); // Draw

    // Displaying
    display();
}

void Viewer::draw_gameover()
{
    // Setting black color as a background
    clear(sf::Color::Black);

    sf::Text gameover_text;
    gameover_text.setFont(font);
    gameover_text.setColor(sf::Color::White);
    gameover_text.setString("BOTANIX");
    gameover_text.setOrigin(gameover_text.getGlobalBounds().width / 2.0f, gameover_text.getGlobalBounds().height / 2.0f);
    gameover_text.setPosition({ float(VIEWER_WIDTH) / 2.0f, float(VIEWER_HEIGHT) / 2.0f });
    sf::RenderWindow::draw(gameover_text); // Draw

    // Displaying
    display();
}

int Viewer::get_lobby_selected_button()
{
    return this->lobby_selected_button;
}

void Viewer::set_lobby_selected_button(int button)
{
    this->lobby_selected_button = button;
}

std::map<int, sf::Text>& Viewer::get_lobby_buttons()
{
    return this->lobby_buttons;
}