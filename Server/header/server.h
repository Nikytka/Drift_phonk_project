#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "network.h"
#include "viewer.h"
#include "safequeue.h"

class Server
{
    World& world; // World pointer

    std::atomic<bool> running = false; // Is server running

    sf::TcpListener listener; // Listener
    sf::SocketSelector selector; // Socket selector

    const int port; // Server port
    const unsigned short MaxPlayers = 3; // Max allowed players

    std::unordered_map<PlayerId, std::unique_ptr<sf::TcpSocket>> sockets; // Sockets map

    SafeQueue<sf::Packet> receivedPackets; // Recieved packets (from clients)

    std::thread syncThread; // Thread
    std::mutex newPlayerMutex; // Player mutex
    sf::Clock clock; // Clock

    std::atomic<bool> dirty = false; // Is server dirty, default = false

    int currentPlayerId;
    
    void receive(); // Receiving packets from players

public:
    Server(int port, World& world); // Server constructor
    ~Server(); // Server destructor
    
    bool isRunning() const; // Is server running function
    
    void update(float dt); // Updating players with data from packets

    void synchronize(); // Synchronizing with all clients (sending up-to-date data from server)

    bool IsDirty(); // Is server dirty
};