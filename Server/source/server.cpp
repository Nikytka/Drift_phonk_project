#pragma once

#include "server.h"

const float PI = 3.141592;

const float forw_acc = 5;
const float back_acc = 3.5;
const float max_vel_abs = 2000;

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

bool Server::isDirty()
{
    return this->dirty;
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

                        // Sending all world data to new player
                        // (data about players that are already connected)
                        outPacket << world.get_players().size();
                        for (auto& elem : world.get_players())
                        {
                            outPacket << elem.first << elem.second.get_pos().x << elem.second.get_pos().y <<
                                elem.second.get_vel().x << elem.second.get_vel().y 
                                << elem.second.get_selected_car() << elem.second.isCarSelected();
                        }

                        dirty = true; // Server dirty

                        // Sending a packet to a new client
                        if (tempSocket->send(outPacket) != sf::Socket::Done)
                        {
                            std::cout << "Error sending data to new player" << std::endl;
                        }
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

void Server::process_packets()
{
    while (!receivedPackets.empty())
    {
        int messageType; // Message type
        int clientId; // Client id

        // Getting the "front" packet
        sf::Packet packet = receivedPackets.dequeue();
        packet >> messageType;

        // Processing nickname packets
        if (messageType == Message::ClientNickname)
        {
            std::string clientNick;
            packet >> clientId >> clientNick; // Data from packet
            world.get_players()[clientId].setNickname(clientNick); // Updating player velocity
            std::cout << "New player nickname: " << clientNick << " (player id: " << clientId << ")" << std::endl;

            // Forming packet with nicknames to send to all players
            sf::Packet toSend;
            toSend << Message::PlayerNicknames << world.get_players().size();
            for (auto& elem : world.get_players())
            {
                // Players position and velocity to packet
                toSend << elem.first << elem.second.getNickname();
            }

            // Sending packets to all clients
            for (const auto& elem : sockets)
            {
                if (elem.second->send(toSend) != sf::Socket::Done)
                {
                    std::cout << "Can't send nicknames packet to player " << elem.first << " \n";
                }
            }
        }

        // Processing movement packets
        if (messageType == Message::Movement)
        {
            sf::Vector2f controls; // Recieved controls vector
            packet >> clientId >> controls.x >> controls.y; // Data from packet
            world.get_players()[clientId].set_controls(controls); // Updating player velocity

            dirty = true; // Server dirty
        }

        // Processing hero selection packets
        if (messageType == Message::ClientCarSelected)
        {
            int selected_hero;
            packet >> clientId >> selected_hero;

            world.get_players()[clientId].set_selected_car(selected_hero);
            world.get_players()[clientId].setCarSelectionConfirm(true);

            // Creating a packet for other players
            sf::Packet toSend;
            toSend << Message::PlayerCarSelected << clientId << selected_hero;

            // Sending to all players
            for (const auto& elem : sockets)
            {
                if (elem.second->send(toSend) != sf::Socket::Done)
                {
                    std::cout << "Can't send player hero selection packet to player " << elem.first << " \n";
                }
            }
        }

        // Gear change packet processing
        if (messageType == Message::Gear_change)
        {
            int gear; // Recieved gear 
            packet >> clientId >> gear; // Data from packet
            world.get_players()[clientId].set_gear(gear); // Updating player gearbox

            dirty = true;
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
                if (it.second.isReady())
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
}

void Server::update(float dt)
{
    // Updating player's speed based on player controls
    for (auto& it : world.get_players())
    {
        sf::Vector2f velocity; // New velocity vector
        float acs, angle; // Neccesary angles

        float prev_vel_abs = sqrt(pow(it.second.get_vel().x, 2) + pow(it.second.get_vel().y, 2)); // Absolute of player previous velocity
        float curr_vel_abs; // Absolute of player current velocity
        
        // Inside the world
      
        if ((0 < it.second.get_pos().x) && (it.second.get_pos().x < world.get_size().x) && (0 < it.second.get_pos().y) && (it.second.get_pos().y < world.get_size().y))
        {
            if (it.second.get_gear() != 0)
            {
                
                if ( it.second.get_controls().y < 0) 
                {
                    velocity.x = it.second.get_vel().x + (it.second.get_gear() >= 0 ? it.second.get_controls().y : -it.second.get_controls().y) * forw_acc * (6 - abs(it.second.get_gear())) * cos(it.second.get_angle() * PI / 180);
                    velocity.y = it.second.get_vel().y + (it.second.get_gear() >= 0 ? it.second.get_controls().y : -it.second.get_controls().y) * forw_acc * (6 - abs(it.second.get_gear())) * sin(it.second.get_angle() * PI / 180);
                }
                else
                {
                    if (it.second.get_controls().y > 0)
                    {
                        velocity.x = it.second.get_vel().x / 1.05;
                        velocity.y = it.second.get_vel().y / 1.05;
                    }
                    else
                    {
                        if (it.second.get_vel().y <= 0)
                            acs = acos(-it.second.get_vel().x / sqrt(pow(it.second.get_vel().x, 2) + pow(it.second.get_vel().y, 2))) * 180 / PI;
                        else
                            acs = acos(it.second.get_vel().x / sqrt(pow(it.second.get_vel().x, 2) + pow(it.second.get_vel().y, 2))) * 180 / PI + 180;
                        if (fmod(it.second.get_angle(), 360) >= 0)
                            angle = fmod(it.second.get_angle(), 360);
                        else
                            angle = 360.0f - fmod(-it.second.get_angle(), 360);
                        if (abs(angle - acs) > 30 && abs(angle - acs) < 330 && abs(fmod(angle + 180, 360) - acs) > 30 && abs(fmod(angle + 180, 360) - acs) < 330)
                        {
                            if (angle < 90)
                            {
                                velocity.x = it.second.get_vel().x - forw_acc * cos(angle / 180 * PI);
                                velocity.y = it.second.get_vel().y - forw_acc * sin(angle / 180 * PI);
                            }
                            if (angle >= 90 && angle <= 180)
                            {
                                velocity.x = it.second.get_vel().x + forw_acc * cos(PI - angle / 180 * PI);
                                velocity.y = it.second.get_vel().y - forw_acc * sin(PI - angle / 180 * PI);

                            }
                            if (angle > 180 && angle <= 270)
                            {
                                velocity.x = it.second.get_vel().x + forw_acc * cos((angle - 180)/ 180 * PI);
                                velocity.y = it.second.get_vel().y + forw_acc * sin((angle - 180)/ 180 * PI);
                            }
                            if (angle > 270)
                            {
                                velocity.x = it.second.get_vel().x - forw_acc * cos((360 - angle) / 180 * PI);
                                velocity.y = it.second.get_vel().y + forw_acc * sin((360 - angle) / 180 * PI);
                            }
                        }
                        else
                        {
                            velocity.x = it.second.get_vel().x;
                            velocity.y = it.second.get_vel().y;
                        }
                    }
                }
            }
            else
            {
                velocity.x = it.second.get_vel().x;
                velocity.y = it.second.get_vel().y;
            }
                if (it.second.get_vel().x != 0 || it.second.get_vel().y != 0)
                {
                    velocity.x -= back_acc * it.second.get_vel().x / prev_vel_abs + (it.second.get_vel().x > 0 ? 1 : (it.second.get_vel().x < 0 ? -1 : 0)) * back_acc * (1 - prev_vel_abs / max_vel_abs) * abs(cos((it.second.get_angle() - 90) * PI / 180));
                    velocity.y -= back_acc * it.second.get_vel().y / prev_vel_abs + (it.second.get_vel().y > 0 ? 1 : (it.second.get_vel().y < 0 ? -1 : 0)) * back_acc * (1 - prev_vel_abs / max_vel_abs) * abs(sin((it.second.get_angle() - 90) * PI / 180));
                }
            
        }
        // On the left border
        if (it.second.get_pos().x <= 0)
        {
            sf::Vector2f new_pos;
            new_pos.x = 0.0f;
            new_pos.y = it.second.get_pos().y;
            it.second.set_pos(new_pos);
            velocity.x = -it.second.get_vel().x;
            velocity.y = it.second.get_vel().y;

        }
        
        // On the right border
        if (it.second.get_pos().x >= world.get_size().x)
        {
            sf::Vector2f new_pos;
            new_pos.x = world.get_size().x;
            new_pos.y = it.second.get_pos().y;
            it.second.set_pos(new_pos);
            velocity.x = -it.second.get_vel().x;
            velocity.y = it.second.get_vel().y;
        }

        // On the top border
        if (it.second.get_pos().y <= 0)
        {
            sf::Vector2f new_pos;
            new_pos.y = 0.0f;
            new_pos.x = it.second.get_pos().x;
            it.second.set_pos(new_pos);
            velocity.y = -it.second.get_vel().y;
            velocity.x = it.second.get_vel().x;
        }
        
        // On the bottom border
        if (it.second.get_pos().y >= world.get_size().y)
        {
            sf::Vector2f new_pos;
            new_pos.x = it.second.get_pos().x;
            new_pos.y = world.get_size().y;
            it.second.set_pos(new_pos);
            velocity.y = -it.second.get_vel().y;
            velocity.x = it.second.get_vel().x;
        }      

        curr_vel_abs = sqrt(pow(velocity.x, 2) + pow(velocity.y, 2));

        if (curr_vel_abs > max_vel_abs / 5 * abs(it.second.get_gear()) && it.second.get_gear() != 0)
            it.second.set_vel({ velocity.x / curr_vel_abs * (max_vel_abs / 5 * abs(it.second.get_gear())), velocity.y / curr_vel_abs * (max_vel_abs / 5 * abs(it.second.get_gear())) });
        else
            it.second.set_vel(velocity);

        if (curr_vel_abs < 10)
            it.second.set_vel({ 0,0 });

        this->dirty = true;

        // Checking if the angle has changed
        if (abs(it.second.get_controls().y) != 0) {
            if (prev_vel_abs >= 10 && it.second.get_controls().x != 0)
            {
                it.second.add_angle(it.second.get_controls().x, it.second.get_controls().y);
            }
        }
        else
            if (prev_vel_abs >= 200 && it.second.get_controls().x != 0)
            {
                it.second.add_angle1(it.second.get_controls().x, it.second.get_controls().y);
            }
    }

    // Updating player's positions
    for (auto& it : world.get_players())
    {
        it.second.update(dt);
    }
}

void Server::update_clients()
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
            elem.second.get_vel().x << elem.second.get_vel().y << elem.second.get_angle();
    }

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