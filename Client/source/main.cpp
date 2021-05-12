#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>

#include "client.h"
#include "viewer.h"
#include "controls.h"
#include <math.h>

const float PI = 3.141592;

// Client main function
int main()
{
    // Creating game world
    World world;

    // Creating vector of samples default music
    std::vector<sf::Music> def_samples(5);
    def_samples[0].openFromFile("mus0.ogg");
    def_samples[1].openFromFile("mus1.ogg");

    // Creating vector of samples phonk music
    std::vector<sf::Music> ph_samples(5);
    ph_samples[0].openFromFile("phonk1.ogg");
    ph_samples[1].openFromFile("phonk2.ogg");
    ph_samples[2].openFromFile("phonk3.ogg");
    ph_samples[3].openFromFile("phonk4.ogg");
    ph_samples[4].openFromFile("phonk5.ogg");

    // Start playing menu default music sample
    def_samples[0].setVolume(50);
    def_samples[0].play();

    // Start playing first phonk music sample
    ph_samples[0].play();
    ph_samples[0].setVolume(0);

    // Creating client (args: ip, port, World)
    Client client("127.0.0.1", 1234, world);

    Viewer viewer("My client"); // Creating viewer
    sf::Clock gameClock; // Game clock

    int i1, j1, i2, j2;

    float acs, angle;

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
        if (world.GetScene() == Scene::CarSelection)
        {
            viewer.handleEvents();
            client.events_car_selection(viewer);
            viewer.draw_car_selection(world);
        }

        // Gameplay scene
        if (world.GetScene() == Scene::Gameplay)
        {
            // Cheking which default sample is playing
            for (i1 = 0; i1 < 5; i1++)
                if (def_samples[i1].getStatus() == sf::Music::Playing)
                    j1 = i1;
            // Stop menu music sample
            def_samples[0].stop();

            // If current default sample is over, the next starts
            if (def_samples[j1].getStatus() == sf::Music::Stopped) {
                def_samples[(j1 + 1) % 5].setVolume(50);
                def_samples[(j1 + 1) % 5].play();
            }

            // Cheking which phonk sample is playing
            for (i2 = 0; i2 < 5; i2++)
                if (ph_samples[i2].getStatus() == sf::Music::Playing)
                    j2 = i2;

            // If current phonk sample is over, the next starts
            if (ph_samples[j2].getStatus() == sf::Music::Stopped) {
                ph_samples[(j2 + 1) % 5].play();
                ph_samples[(j2 + 1) % 5].setVolume(0);
            }

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


            //Checking if car is drifting now (checking difference between angles of sprite and velocity
            if (sqrt(pow(world.get_players()[client.id()].get_vel().x, 2) + pow(world.get_players()[client.id()].get_vel().y, 2)) >= 50) {
                if (world.get_players()[client.id()].get_vel().y<=0)
                    acs = acos(-world.get_players()[client.id()].get_vel().x / sqrt(pow(world.get_players()[client.id()].get_vel().x, 2) + pow(world.get_players()[client.id()].get_vel().y, 2))) * 180 / PI ;
                else 
                    acs = acos(world.get_players()[client.id()].get_vel().x / sqrt(pow(world.get_players()[client.id()].get_vel().x, 2) + pow(world.get_players()[client.id()].get_vel().y, 2))) * 180 / PI + 180;
                if (fmod(world.get_players()[client.id()].get_angle(), 360) >= 0)
                    angle = fmod(world.get_players()[client.id()].get_angle(), 360);
                else {
                    angle = 360.0f - fmod(-world.get_players()[client.id()].get_angle(),360);
                }

                std::cout << angle << " ";
                std::cout << acs << std::endl;

                if (abs(angle-acs) > 30 && abs(angle-acs) < 330 && abs(fmod(angle+180,360)-acs) > 30 && abs(fmod(angle+180,360) - acs) < 330) {
                    def_samples[j1].setVolume(0);
                    ph_samples[j2].setVolume(100);
                }

                else {
                    def_samples[j1].setVolume(50);
                    ph_samples[j2].setVolume(0);
                }
            }
            else {
                def_samples[j1].setVolume(50);
                ph_samples[j2].setVolume(0);
            }


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
    }

    return 0;
}