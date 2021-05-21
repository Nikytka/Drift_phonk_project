#pragma once

#include "client.h"

void Client::recieve()
{
    // Connecting socket to server
    auto status = socket.connect(ip, port);
    if (status != sf::Socket::Status::Done)
    {
        std::cout << "Can't connect to server: " << status << "\n";
        return;
    }

    // Main cycle
    while (running)
    {
        // Recieving packets
        sf::Packet packet;
        if (socket.receive(packet) == sf::Socket::Done)
        {
            // Packet type
            int type;
            packet >> type;

            // Client created packet processing
            if (type == Message::ClientCreated)
            {
                sf::Vector2f pos;
                packet >> clientId >> pos.x >> pos.y >> serverTime;

                world.get_players()[clientId].set_pos(pos);
                std::cout << "Client created, id: " << clientId << std::endl;
                
                // Printing currently online players
                std::cout << "Online players by their id\'s: ";
                int online_players_num;
                packet >> online_players_num;
                for (int i = 0; i < online_players_num; i++)
                {
                    int id;
                    packet >> id;
                    std::cout << id << " ";
                }
                std::cout << std::endl;

                clock.restart();

                // Sending client nickname to server
                sf::Packet toServer;
                toServer << Message::ClientNickname << id() << this->clientNickname;

                // Sending packet
                if (socket.send(toServer) != sf::Socket::Done)
                {
                    std::cout << "Can't send client nickname to server\n";
                }

                world.SetScene(Scene::Lobby);
            }

            // Nicknames packet processing
            if (type == Message::PlayerNicknames)
            {
                {
                    int n; // Number of players
                    packet >> n; // Getting number of players from packet
                    std::lock_guard<std::mutex> m(world.mutex);

                    for (int i = 0; i < n; i++)
                    {
                        int index; // Player id
                        std::string nickname; // Player nickname

                        packet >> index >> nickname;

                        world.get_players()[index].setNickname(nickname);
                    }
                }
            }

            // Update world packet processing
            if (type == Message::UpdateWorld)
            {
                {
                    int n; // Number of players
                    packet >> n; // Getting number of players from packet
                    std::lock_guard<std::mutex> m(world.mutex);

                    for (int i = 0; i < n; ++i)
                    {
                        int index; // Player id
                        float angle;
                        sf::Vector2f pos, v; // Positon and velocity from server

                        packet >> index >> pos.x >> pos.y >> v.x >> v.y >> angle;

                        world.get_players()[index].set_pos(pos); // Updating position for players
                        world.get_players()[index].set_vel(v); // Updating velocity for players
                        world.get_players()[index].set_angle(angle); // Updating car angle 
                    }
                }

                // Getting elapsed server time from packet
                float ts;
                packet >> ts;

                // Updating world
                world.update(clock.getElapsedTime().asSeconds() - (ts - serverTime));
            }

            // Remove player packet processing
            if (type == Message::RemovePlayer)
            {
                int remove_id;
                packet >> remove_id; // Getting player id
                this->world.remove_player(remove_id); // Removing this player

                std::cout << "Player " << remove_id << " disconnected\n";

                this->world.show_players(); // Printing currently online players
            }

            // Start game packet processing
            if (type == Message::SceneGameplay)
            {
                world.SetScene(Scene::Gameplay);

                std::cout << "Started the game" << std::endl;
            }
        }
    }
}

Client::Client(const std::string& ip, int port, World& world) :
    ip(ip),
    port(port),
    world(world)
{
    running = true; // Client now running
}

Client::~Client()
{
    this->disconnect();
    running = false; // No longer running 
    syncThread.join(); // Joining thread
}

void Client::notify_server()
{
    // Creating a packet
    sf::Packet packet;

    // This client's player controls vector
    const auto& controls = world.get_players()[id()].get_controls();

    // Creating a packet for sending controls
    packet << Message::Movement << clientId << controls.x << controls.y;
    
    // Sending packet
    if (socket.send(packet) != sf::Socket::Done)
    {
        std::cout << "Can't send movement to server\n";
    }
}

void Client::change_gear()
{
    // Creating a packet
    sf::Packet packet;

    // The client's player gearbox controls 
    int gear = world.get_players()[id()].get_gear();

    // Creating a packet for sending controls
    packet << Message::Gear_change << clientId << gear;

    // Sending packet
    if (socket.send(packet) != sf::Socket::Done)
    {
        std::cout << "Can't send movement to server\n";
    }

}

int Client::id() const
{
    return clientId;
}

bool Client::isRunning() const
{
    return running;
}

void Client::connect()
{
    syncThread = std::thread(&Client::recieve, this); // Creating thread
    syncThread.detach(); // Detaching thread
}

void Client::disconnect()
{
    // Creating a packet
    sf::Packet packet;

    // Creating a packet for sending velocity
    packet << Message::ClientDisconnect << this->id();

    // Sending packet
    if (socket.send(packet) != sf::Socket::Done)
    {
        std::cout << "Can't send disconnect packet to server\n";
    }
}

void Client::events_connect(Viewer& viewer)
{
    sf::Event my_event; // Getting event

    // Processing events
    while (viewer.pollEvent(my_event))
    {
        // Close window event
        if (my_event.type == sf::Event::Closed)
        {
            viewer.close();
        }

        // Text input event
        if (viewer.get_connect_selected_button() < viewer.get_number_of_textboxes())
        {
            if (my_event.type == sf::Event::TextEntered)
            {
                viewer.getTextbox()[viewer.get_connect_selected_button()].typedOn(my_event);
            }
        }
    }

    // Changing buttons in connect scene
    // Going down
    if ((viewer.get_connect_selected_button() < (viewer.get_connect_menu_size() - 1)) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        viewer.set_connect_selected_button(viewer.get_connect_selected_button() + 1);
        while (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {}
    }
    // Going up
    if ((viewer.get_connect_selected_button() > 0) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        viewer.set_connect_selected_button(viewer.get_connect_selected_button() - 1);
        while (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {}
    }

    // Pressing connect button
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && (viewer.get_connect_selected_button() == 3))
    {
        this->clientNickname = viewer.getTextbox()[0].getText(); // Setting up client nickname
        this->ip = viewer.getTextbox()[1].getText(); // Setting up server ip
        this->port = std::stoi(viewer.getTextbox()[2].getText()); // Setting up server port

        this->connect();

        while (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {}
    }
}

void Client::events_lobby(Viewer& viewer)
{
    // Changing buttons in lobby
    // Going down
    if ((viewer.get_lobby_selected_button() < (viewer.get_lobby_buttons().size() - 1)) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        viewer.set_lobby_selected_button(viewer.get_lobby_selected_button() + 1);
        while (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {}
    }
    // Going up
    if ((viewer.get_lobby_selected_button() > 0) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        viewer.set_lobby_selected_button(viewer.get_lobby_selected_button() - 1);
        while (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {}
    }

    // Pressing ready button
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && (viewer.get_lobby_selected_button() == 0))
    {
        if (!world.get_players()[clientId].isReady())
        {
            debug("Ready");
            world.get_players()[this->clientId].player_ready();

            // Creating a packet
            sf::Packet packet;

            // Creating a packet for sending velocity
            packet << Message::ClientReady << this->id();

            // Sending packet
            if (socket.send(packet) != sf::Socket::Done)
            {
                std::cout << "Can't send ready packet to server\n";
            }
        }
    }

    // Pressing hero selection button
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && (viewer.get_lobby_selected_button() == 1))
    {
        world.SetScene(Scene::CarSelection);
    }

    // Pressing disconnect button
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && (viewer.get_lobby_selected_button() == 2))
    {
        this->disconnect();
        this->running = false;
    }
}

void Client::events_car_selection(Viewer& viewer)
{
    // ESC to get back to lobby
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        world.SetScene(Scene::Lobby);
        while (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {}
    }

    // Going left in hero selection
    if ((world.get_players()[this->clientId].get_selected_car() > 0) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        if (!world.get_players()[this->clientId].isCarSelected())
        {
            int current_car = world.get_players()[this->clientId].get_selected_car();
            current_car -= 1;
            world.get_players()[this->clientId].set_selected_car(current_car);
            while (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {}
        }
    }

    // Going right in hero selection
    if ((world.get_players()[this->clientId].get_selected_car() < (viewer.get_number_of_cars() - 1)) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        if (!world.get_players()[this->clientId].isCarSelected())
        {
            int current_hero = world.get_players()[this->clientId].get_selected_car();
            current_hero += 1;
            world.get_players()[this->clientId].set_selected_car(current_hero);
            while (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {}
        }
    }

    // Confirming hero selection
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
    {
        if (!world.get_players()[this->clientId].isCarSelected())
        {
            // Creating a packet
            sf::Packet packet;

            // Forming a packet for sending selected hero id
            packet << Message::ClientCarSelected << this->id() << world.get_players()[this->clientId].get_selected_car();

            // Sending packet
            if (socket.send(packet) != sf::Socket::Done)
            {
                std::cout << "Can't send hero selection packet to server\n";
            }

            while (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {}
        }
    }
}
