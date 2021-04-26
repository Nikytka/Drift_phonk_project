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
                        int index, score; // Player id
                        sf::Vector2f pos, v; // Positon and velocity from server

                        packet >> index >> pos.x >> pos.y >> v.x >> v.y >> score;

                        world.get_players()[index].set_pos(pos); // Updating position for players
                        world.get_players()[index].set_vel(v); // Updating velocity for players
                        world.get_players()[index].set_score(score); // Set player score
                    }
                }

                // Updating target position
                sf::Vector2f target_pos;
                packet >> target_pos.x >> target_pos.y;
                world.get_target().set_pos(target_pos);

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

            // Gameover scene packet processing
            if (type == Message::SceneGameover)
            {
                int id;
                packet >> id;

                world.SetScene(Scene::Gameover);
                world.get_players()[id].won_the_game();

                std::cout << "Player " << id << " won the game\n";
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
    syncThread = std::thread(&Client::recieve, this); // Creating thread
    syncThread.detach(); // Detaching thread
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

int Client::id() const
{
    return clientId;
}

bool Client::isRunning() const
{
    return running;
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
        if (!world.get_players()[clientId].IfReady())
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
        world.SetScene(Scene::HeroSelection);
    }

    // Pressing disconnect button
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && (viewer.get_lobby_selected_button() == 2))
    {
        this->disconnect();
        this->running = false;
    }
}

void Client::events_hero_selection(Viewer& viewer)
{
    // ESC to get back to lobby
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        world.SetScene(Scene::Lobby);
        while (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {}
    }
}
