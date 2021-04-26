#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <atomic>

#include "network.h"
#include "viewer.h"

class Client
{
    const std::string ip; // Ip adress
    const int port; // Port
    float serverTime = 0.f; // Server time

    std::atomic<bool> running = false; // Is running, default = false

    World& world; // World pointer

    int clientId = -1; // Client id, default = -1

    sf::Clock clock; // Clock

    sf::TcpSocket socket; // Connection socket

    std::thread syncThread;

    void recieve(); // Client start function

public:
    Client(const std::string& ip, int port, World& world); // Client constructor
    ~Client(); // Client destructor
    
    void notify_server(); // Notify server about new player controls

    int id() const; // Get client id
    
    bool isRunning() const; // Is client running

    void disconnect(); // Disconnect from the server

    void events_lobby(Viewer& viewer); // Handle events in the lobby
    void events_hero_selection(Viewer& viewer); // Handle events in the hero selection screen
};
