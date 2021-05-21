#include <iostream>
#include <SFML\Graphics.hpp>

#include "server.h"
#include "viewer.h"

// Server main function
int main()
{
    World world; // Creating world

    Server server(1234, world); // Creating server
    Viewer viewer("My server"); // Creating server viewer

    sf::Clock gameClock;
    sf::Time tick;

    viewer.setWholeMapView(true);

    // Main cycle
    while (server.isRunning() && viewer.isOpen())
    {
        viewer.handleEvents(); // Handling events

        server.process_packets();

        auto dt = gameClock.restart(); // Calculating dt
        server.update(dt.asSeconds()); // Updating

        tick += dt;

        // Synchronizing clients if needed
        if (tick.asMilliseconds() > 1000 || server.isDirty())
        {
            server.update_clients();
            tick = sf::Time();
        }

        viewer.draw_gameplay(world, -1); // Drawing world
    }

    return 0;
}