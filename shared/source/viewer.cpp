#pragma once

#include "viewer.h"


Viewer::Viewer(const std::string& name) : sf::RenderWindow(sf::VideoMode(800, 800), name)
{
    // Loading player textures
    this->car_textures[0].loadFromFile("car_0.png");
    this->car_textures[1].loadFromFile("car_1.png");
    this->car_textures[2].loadFromFile("car_2.png");

    // Loading font
    if (!font.loadFromFile("PressStart2P.ttf"))
    {
        std::cout << "Could not load font" << std::endl;
    }

    setFramerateLimit(60); // Frame limit

    // Setting up connect scene buttons near the textboxes and textboxes
    connect_buttons[0].setString("nickname:");
    connect_buttons[1].setString("server ip:");
    connect_buttons[2].setString("server port:");
    connect_textboxes[0].setText("Player");
    connect_textboxes[1].setText("127.0.0.1");
    connect_textboxes[2].setText("1234");
    for (int i = 0; i < NUMBER_OF_CONNECT_TEXTBOXES; i++)
    {
        // Setting up textboxes
        connect_textboxes[i].setFont(font); // Setting font
        connect_textboxes[i].setOrigin({ 0.0f, connect_textboxes[i].getGlobalBounds().height / 2.0f });
        sf::Vector2f pos; // Creating position
        pos.x = float(VIEWER_WIDTH) / 2.0f + float(VIEWER_WIDTH) * SPACE_BETWEEN_CONNECT_BUTTONS / 4.0f;
        pos.y = float(VIEWER_HEIGHT) / 2.0f + float(i) * float(VIEWER_HEIGHT) * SPACE_BETWEEN_CONNECT_BUTTONS
            - float(VIEWER_HEIGHT) * float(NUMBER_OF_CONNECT_BUTTONS - 1) * SPACE_BETWEEN_CONNECT_BUTTONS / 2.0f;
        connect_textboxes[i].setPosition(pos); // Setting textbox position
        connect_textboxes[i].setLimit(true, 10); // Textbox has limit

        // Setting up buttons
        connect_buttons[i].setFont(font); // Setting font
        connect_buttons[i].setOrigin(connect_buttons[i].getGlobalBounds().width, connect_buttons[i].getGlobalBounds().height / 2.0f);
        sf::Vector2f buttonPos; // Creating position
        buttonPos.x = float(VIEWER_WIDTH) / 2.0f - float(VIEWER_WIDTH) * SPACE_BETWEEN_CONNECT_BUTTONS / 4.0f;
        buttonPos.y = float(VIEWER_HEIGHT) / 2.0f + float(i) * float(VIEWER_HEIGHT) * SPACE_BETWEEN_CONNECT_BUTTONS
            - float(VIEWER_HEIGHT) * float(NUMBER_OF_CONNECT_BUTTONS - 1) * SPACE_BETWEEN_CONNECT_BUTTONS / 2.0f;
        connect_buttons[i].setPosition(buttonPos); // Setting button position
    }

    // Setting up connect scene buttons (buttons by themselfes)
    connect_buttons[3].setString("connect");
    for (int i = NUMBER_OF_CONNECT_TEXTBOXES; i < NUMBER_OF_CONNECT_BUTTONS; i++)
    {
        connect_buttons[i].setFont(font); // Setting font
        connect_buttons[i].setOrigin(connect_buttons[i].getGlobalBounds().width / 2.0f, connect_buttons[i].getGlobalBounds().height / 2.0f);
        sf::Vector2f pos; // Creating position
        pos.x = float(VIEWER_WIDTH) / 2.0f;
        pos.y = float(VIEWER_HEIGHT) / 2.0f + float(i) * float(VIEWER_HEIGHT) * SPACE_BETWEEN_CONNECT_BUTTONS
            - float(VIEWER_HEIGHT) * float(NUMBER_OF_CONNECT_BUTTONS - 1) * SPACE_BETWEEN_CONNECT_BUTTONS / 2.0f;
        connect_buttons[i].setPosition(pos); // Setting button position
    }

    // Setting up lobby buttons
    for (int i = 0; i < NUMBER_OF_LOBBY_BUTTONS; i++)
    {
        lobby_buttons[i].setFont(font);
    }
    lobby_buttons[0].setString("ready");
    lobby_buttons[1].setString("select car");
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

void Viewer::draw_connect(World& world)
{
    // Setting black color as a background
    clear(sf::Color::Black);

    // Drawing textboxes
    for (auto& it : connect_textboxes)
    {
        if (it.first == connect_selected_button)
        {
            it.second.setColor(sf::Color::Red);
        }
        else
        {
            it.second.setColor(sf::Color::White);
        }

        sf::RenderWindow::draw(it.second.getDrawable());
    }

    // Drawing buttons
    for (auto& it : connect_buttons)
    {
        if (it.first == connect_selected_button)
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

void Viewer::draw_car_selection(World& world, int clientId)
{
    // Setting black color as a background
    clear(sf::Color::Black);

    // Calculatin total menu distance (for centering)
    float total_heroes_width = (NUMBER_OF_CARS - 1) * VIEWER_WIDTH * 0.1f;
    for (int i = 0; i < (NUMBER_OF_CARS - 1); i++)
    {
        sf::Sprite sample_hero_sprite(this->car_textures[0]);
        sample_hero_sprite.setScale(CAR_SCALE_FACTOR, CAR_SCALE_FACTOR); // Scaling

        total_heroes_width += sample_hero_sprite.getGlobalBounds().width;
    }

    // Setting up hero sprites
    for (auto& it : car_textures)
    {
        sf::Sprite hero_sprite(this->car_textures[it.first]); // Set texture
        hero_sprite.scale(CAR_SCALE_FACTOR, CAR_SCALE_FACTOR); // Scaling
        auto hero_rect = hero_sprite.getGlobalBounds();
        hero_sprite.setOrigin(hero_rect.width / CAR_SCALE_FACTOR / 2.0f,
            hero_rect.height / CAR_SCALE_FACTOR / 2.0f); // Set sprite origin

        // Calculating hero sprite position
        sf::Vector2f pos;
        pos.x = float(VIEWER_WIDTH) / 2.0f + float(it.first) * float(VIEWER_WIDTH) * 0.1f
            + float(it.first) * hero_rect.width - total_heroes_width / 2.0f;
        pos.y = float(VIEWER_HEIGHT) / 2.0f;

        hero_sprite.setPosition(pos);

        // Drawing selected hero rectangle
        if (it.first == world.get_players()[clientId].get_selected_car())
        {
            // Creating outer rectangle
            sf::RectangleShape selected_rect_out;
            selected_rect_out.setSize({ hero_rect.width * 1.1f, hero_rect.height * 1.1f });
            selected_rect_out.setOrigin({ selected_rect_out.getSize().x / 2.0f, selected_rect_out.getSize().y / 2.0f });
            if (!world.get_players()[clientId].isCarSelected())
            {
                // If selection is not confirmed - white
                selected_rect_out.setFillColor(sf::Color::White);
            }
            else
            {
                // If selection is confirmed - red
                selected_rect_out.setFillColor(sf::Color::Red);
            }
            selected_rect_out.setPosition(pos);

            // Creating inner rectangle
            sf::RectangleShape selected_rect_in;
            selected_rect_in.setSize({ hero_rect.width * 1.07f, hero_rect.height * 1.07f });
            selected_rect_in.setOrigin({ selected_rect_in.getSize().x / 2.0f, selected_rect_in.getSize().y / 2.0f });
            selected_rect_in.setFillColor(sf::Color::Black);
            selected_rect_in.setPosition(pos);

            sf::RenderWindow::draw(selected_rect_out); // Drawing outer rectangle
            sf::RenderWindow::draw(selected_rect_in); // Drawing inner rectangle
        }

        sf::RenderWindow::draw(hero_sprite); // Draw
    }

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
        sf::Sprite player_model(this->car_textures[it.second.get_selected_car()]); // Set texture
        //player_model.setRotation(180);
        auto player_rect = player_model.getGlobalBounds();
        //if ( )
        player_model.setOrigin(player_rect.width / 2.0f, player_rect.height / 1.6f); // Set sprite origin
        player_model.setPosition(it.second.get_pos()); // Set position
        player_model.setScale({ 0.7f, 0.7f });
        player_model.setRotation(it.second.get_angle() - 90);
        sf::RenderWindow::draw(player_model); // Draw
    }

    // Displaying
    display();
}

int Viewer::get_connect_menu_size()
{
    return NUMBER_OF_CONNECT_BUTTONS;
}

int Viewer::get_number_of_textboxes()
{
    return NUMBER_OF_CONNECT_TEXTBOXES;
}

int Viewer::get_connect_selected_button()
{
    return this->connect_selected_button;
}

void Viewer::set_connect_selected_button(int button)
{
    this->connect_selected_button = button;
}

std::map<int, sf::Text>& Viewer::get_connect_buttons()
{
    return this->connect_buttons;
}

std::map<int, Textbox>& Viewer::getTextbox()
{
    return this->connect_textboxes;
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

int Viewer::get_number_of_cars()
{
    return this->NUMBER_OF_CARS;
}
