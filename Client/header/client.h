#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <atomic>

#include "network.h"
#include "viewer.h"

class Client
{
    std::string ip; // Ip adress
    int port; // Port
    float serverTime = 0.f; // Server time

    std::string clientNickname; // Player nickname for this client

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

    void change_gear(); // Notify server about player gear change

    void change_score(); // Notify server about changing player score

    int id() const; // Get client id
    
    bool isRunning() const; // Is client running

    void connect(); // Connect to server
    void disconnect(); // Disconnect from the server

    void events_connect(Viewer& viewer); // Handle events in the connect screen
    void events_lobby(Viewer& viewer); // Handle events in the lobby
    void events_car_selection(Viewer& viewer); // Handle events in the hero selection screen
    void events_gameplay(Viewer& viewer); // Handle events in the gameplay scene
    void events_pause(Viewer& viewer); // Handle events in the pause scene
};
