#include <iostream>
#include <SFML\Graphics.hpp>

#include "client.h"
#include "viewer.h"
#include "controls.h"

// Client main function
int main()
{
    // Creating game world
    World world;

    // Creating client (args: ip, port, World)
    Client client("127.0.0.1", 1234, world);

    Viewer viewer("My client"); // Creating viewer
    sf::Clock gameClock; // Game clock

    sf::Vector2f prevControls; // Previous player controls vector

    // Main cycle
    while (client.isRunning() && viewer.isOpen())
    {
        // Lobby scene
        if (world.GetScene() == Scene::Lobby)
        {
            viewer.handleEvents();
            client.events_lobby(viewer);
            viewer.draw_lobby(world);
        }

        // Hero selection scene
        if (world.GetScene() == Scene::HeroSelection)
        {
            viewer.handleEvents();
            client.events_hero_selection(viewer);
            viewer.draw_hero_selection(world);
        }

        // Gameplay scene
        if (world.GetScene() == Scene::Gameplay)
        {
            viewer.handleEvents(); // Handling events

            sf::Vector2f currControls; // Current player controls vector

            if (client.id() == 0)
            {
                // WASD for player 0
                currControls = controls_wasd();
            }
            else
            {
                // Arrow controls for others
                currControls = controls_arrows();
            }
            
            // Changing player's velocity to new velocity
            world.get_players()[client.id()].set_controls(currControls);

            // Restarting clock and updating world
            const auto dt = gameClock.restart();
            world.update(dt.asSeconds());

            // On any change notify server
            if (currControls != prevControls)
            {
                client.notify_server();
                prevControls = currControls;
            }

            // Drawing world
            viewer.draw_gameplay(world);
        }

        // Gameover scene
        if (world.GetScene() == Scene::Gameover)
        {
            viewer.handleEvents();
            viewer.draw_gameover();
        }
    }

    return 0;
}