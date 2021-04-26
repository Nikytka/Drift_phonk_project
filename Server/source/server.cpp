#pragma once

#include "server.h"

Server::Server(int port, World& world) :
    port(port),
    world(world)
{
    this->currentPlayerId = 0;

    // Starting listener
    if (listener.listen(port) == sf::Socket::Done)
    {
        std::cout << "Server is started on port: " << port << ". Waiting for clients.\n";
        running = true;
    }
    else
    {
        std::cout << "Error - Failed to bind the port " << port << std::endl;
        return;
    }

    sf::Vector2f new_pos = world.get_random_pos();

    world.get_target().set_pos(new_pos); // Creating first target

    // Creating and detaching a thread for receiving packets
    syncThread = std::thread(&Server::receive, this);
    syncThread.detach();
}

Server::~Server()
{
    if (running)
    {
        // Stopping server, joining thread
        running = false;
        syncThread.join();
    }
}

bool Server::isRunning() const
{
    return running;
}

void Server::receive()
{
    selector.add(listener);

    while (isRunning())
    {
        if (selector.wait())
        {
            if (selector.isReady(listener))
            {
                // Creating the socket that will wait for new connections
                auto tempSocket = std::make_unique<sf::TcpSocket>();

                // New client connection
                // If a new client is connected this code executes
                if (listener.accept(*tempSocket) == sf::Socket::Done)
                {
                    if (this->world.get_players().size() < MaxPlayers) //if server is not full
                    {
                        std::lock_guard<std::mutex> guard(newPlayerMutex);

                        // Setting random player spawn position
                        sf::Vector2f new_pos = world.get_random_pos();
                        world.get_players()[this->currentPlayerId].set_pos(new_pos);

                        // Adding new client's socket
                        selector.add(*tempSocket);

                        // Creating a spawn packet for the new client
                        sf::Packet outPacket;
                        outPacket << Message::ClientCreated << this->currentPlayerId << world.get_players()[this->currentPlayerId].get_pos().x
                            << world.get_players()[this->currentPlayerId].get_pos().y << clock.getElapsedTime().asSeconds();

                        // Sending online player id's to the new player
                        outPacket << this->world.get_players().size();
                        for (auto& it : this->world.get_players())
                        {
                            outPacket << it.first;
                        }

                        dirty = true; // Server dirty

                        // Sending a packet to a new client
                        if (tempSocket->send(outPacket) != sf::Socket::Done)
                            std::cout << "Error sending player index" << std::endl;
                        else
                        {
                            std::cout << "Player " << this->currentPlayerId << " connected (spawn position: "
                            << world.get_players()[this->currentPlayerId].get_pos().x << " " <<
                                world.get_players()[this->currentPlayerId].get_pos().y << ")\n";
                        }

                        sockets[this->currentPlayerId] = std::move(tempSocket);
                        this->currentPlayerId++;

                        this->world.show_players();
                    }
                    else
                    {
                        std::cout << "Could not connect new player, server is full\n";
                    }
                }
            }
            else
            {
                // Receiving packets from all players
                for (const std::pair<const PlayerId, Player>& elem : world.get_players())
                {
                    auto& socket = *sockets[elem.first]; // Selecting player socket
                    if (selector.isReady(socket))
                    {
                        // Receiving a packet
                        sf::Packet received;
                        if (socket.receive(received) == sf::Socket::Done)
                        {
                            // Pushing received packet to safequeues
                            receivedPackets.enqueue(received);
                        }
                    }
                }
            }
        }
    }
}

void Server::update(float dt)
{
    while (!receivedPackets.empty())
    {
        int messageType; // Message type
        int clientId; // Client id

        // Getting the "front" packet
        sf::Packet packet = receivedPackets.dequeue();
        packet >> messageType;

        // Processing movement packets
        if (messageType == Message::Movement)
        {
            sf::Vector2f controls; // Recieved controls vector
            packet >> clientId >> controls.x >> controls.y; // Data from packet
            world.get_players()[clientId].set_controls(controls); // Updating player velocity

            dirty = true; // Server dirty
        }

        // Processing disconnect packets
        if (messageType == Message::ClientDisconnect)
        {
            packet >> clientId;

            sf::Packet toSend;
            toSend << Message::RemovePlayer << clientId; // Forming packet

            // Sending to everybody except the player who is disconnecting
            for (const auto& elem : sockets)
            {
                if (elem.first != clientId)
                {
                    if (elem.second->send(toSend) != sf::Socket::Done)
                    {
                        std::cout << "Can't send disconnect packet to player " << elem.first << " \n";
                    }
                }
            }

            this->selector.remove(*sockets[clientId].get());
            this->world.remove_player(clientId); // Removing player from the world
            this->sockets.erase(clientId); // Removing player socket

            std::cout << "Player " << clientId << " disconnected\n";
            this->world.show_players();

            dirty = true; // Server dirty
        }

        // Processing "ready to play" packets
        if (messageType == Message::ClientReady)
        {
            debug("ready packet");
            packet >> clientId; // Data from packet
            world.get_players()[clientId].player_ready();

            debug("Client", clientId, "ready");

            // Counting ready players
            int number_of_ready_players = 0;
            for (auto& it : world.get_players())
            {
                if (it.second.IfReady())
                {
                    number_of_ready_players++;
                }
            }

            // If everybody is ready start the game
            if (number_of_ready_players == world.get_players().size())
            {
                debug("All ready");
                // Creating a packet
                sf::Packet toSend;
                toSend << Message::SceneGameplay;

                // Sending to all players
                for (const auto& elem : sockets)
                {
                    if (elem.second->send(toSend) != sf::Socket::Done)
                    {
                        std::cout << "Can't send start game packet to player " << elem.first << " \n";
                    }
                }

                world.SetScene(Scene::Gameplay);
            }
        }
    }

    // Updating player's speed based on player controls
    for (auto& it : world.get_players())
    {
        sf::Vector2f velocity; // New velocity vector

        // X axis velocity
        // Inside the world
        if ((0 < it.second.get_pos().x) && (it.second.get_pos().x < world.get_size().x))
        {
            velocity.x = it.second.get_controls().x * it.second.get_maxspeed();
        }
        // On the left border
        if (it.second.get_pos().x == 0)
        {
            if (it.second.get_controls().x > 0.0f)
            {
                velocity.x = it.second.get_controls().x * it.second.get_maxspeed();
            }
            else
            {
                velocity.x = 0;
            }
        }
        // Outside the left border
        if (it.second.get_pos().x < 0.0f)
        {
            sf::Vector2f new_pos;
            new_pos.x = 0.0f;
            new_pos.y = it.second.get_pos().y;
            it.second.set_pos(new_pos);
        }
        // On the right border
        if (it.second.get_pos().x == world.get_size().x)
        {
            if (it.second.get_controls().x < 0.0f)
            {
                velocity.x = it.second.get_controls().x * it.second.get_maxspeed();
            }
            else
            {
                velocity.x = 0;
            }
        }
        // Outside the right border
        if (it.second.get_pos().x > world.get_size().x)
        {
            sf::Vector2f new_pos;
            new_pos.x = world.get_size().x;
            new_pos.y = it.second.get_pos().y;
            it.second.set_pos(new_pos);
        }

        // Y axis velocity
        // Inside the world
        if ((0 < it.second.get_pos().y) && (it.second.get_pos().y < world.get_size().y))
        {
            velocity.y = it.second.get_controls().y * it.second.get_maxspeed();
        }
        // On the top border
        if (it.second.get_pos().y == 0)
        {
            if (it.second.get_controls().y > 0.0f)
            {
                velocity.y = it.second.get_controls().y * it.second.get_maxspeed();
            }
            else
            {
                velocity.y = 0;
            }
        }
        // Outside the top border
        if (it.second.get_pos().y < 0.0f)
        {
            sf::Vector2f new_pos;
            new_pos.x = it.second.get_pos().x;
            new_pos.y = 0.0f;
            it.second.set_pos(new_pos);
        }
        // On the bottom border
        if (it.second.get_pos().y == world.get_size().y)
        {
            if (it.second.get_controls().y < 0.0f)
            {
                velocity.y = it.second.get_controls().y * it.second.get_maxspeed();
            }
            else
            {
                velocity.y = 0;
            }
        }
        // Outside the bottom border
        if (it.second.get_pos().y > world.get_size().y)
        {
            sf::Vector2f new_pos;
            new_pos.x = it.second.get_pos().x;
            new_pos.y = world.get_size().y;
            it.second.set_pos(new_pos);
        }

        // Checking if the velocity has changed
        if (it.second.get_vel() != velocity)
        {
            it.second.set_vel(velocity);
            this->dirty = true; // Server is dirty if not the same velocity as used to be
        }
    }

    // Updating player's positions
    for (auto& it : world.get_players())
    {
        it.second.update(dt);
    }

    // Checking if anybody reached the target
    for (auto& it : world.get_players())
    {
        if (sqrt(pow((world.get_target().get_pos().x - it.second.get_pos().x), 2) 
            + pow((world.get_target().get_pos().y - it.second.get_pos().y), 2)) < it.second.get_rad())
        {
            world.get_target().set_pos(world.get_random_pos()); // Setting new target pos
            it.second.increase_score(); // Increasing player score

            dirty = true; // Server dirty now
        }
    }

    // Checking if anybody won the game
    for (auto& it : world.get_players())
    {
        if (it.second.IfWinner())
        {
            world.SetScene(Scene::Gameover);
            it.second.won_the_game();

            sf::Packet toSend; // Forming packet
            toSend << Message::SceneGameover << it.first; // Sending id of the winner

            for (const auto& elem : this->sockets)
            {
                if (elem.second->send(toSend) != sf::Socket::Done)
                {
                    std::cout << "Can't send gameover scene packet to player " << elem.first << " \n";
                }
            }

            it.second.no_longer_winner();

            break;
        }
    }
}

void Server::synchronize()
{
    // If server is not dirty
    if (!dirty)
        return;

    dirty = false; // Server not dirty
    std::lock_guard<std::mutex> guard(newPlayerMutex);

    // Forming a packet to send to clients
    sf::Packet toSend;
    toSend << Message::UpdateWorld << world.get_players().size();
    for (auto& elem : world.get_players())
    {
        // Players position and velocity to packet
        toSend << elem.first << elem.second.get_pos().x << elem.second.get_pos().y <<
            elem.second.get_vel().x << elem.second.get_vel().y << elem.second.get_score();
    }

    // Pushing target position to packet
    toSend << world.get_target().get_pos().x;
    toSend << world.get_target().get_pos().y;

    // Pushing server elapsed time to packet
    toSend << clock.getElapsedTime().asSeconds();

    // Sending packets to all clients
    for (const auto& elem : sockets)
    {
        if (elem.second->send(toSend) != sf::Socket::Done)
        {
            std::cout << "Can't send update to player " << elem.first << " \n";
        }
    }
}

bool Server::IsDirty()
{
    return this->dirty;
}

